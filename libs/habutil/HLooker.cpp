/*
 *  HLooker.cpp
 *  hstate
 *
 *  Created by Oakes Lab on 6/13/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HLooker.h"
#include <QtDebug>

HLooker::HLooker(int minlooktime_ms, int minlookawaytime_ms, int maxlookawaytime_ms, int maxaccumlooktime_ms, HEventLog& log, bool bLive)
: QStateMachine()
, m_minLookTimeMS(minlooktime_ms)
, m_minLookAwayTimeMS(minlookawaytime_ms)
, m_maxLookAwayTimeMS(maxlookawaytime_ms)
, m_maxAccumulatedLookTimeMS(maxaccumlooktime_ms)
, m_log(log)
, m_bLive(bLive)
, m_bLookPending(false)
, m_pdirectionLookPending(&HLookDirection::LookAway)
, m_bLookAwayStarted(false)
{
	// There are 4 timers created here. Each is single shot, and each will have its interval set when it is started.
	// Each can have its associated time values modified when the HLooker is not running.

	m_ptimerMinLookAwayTime = new QTimer();
	m_ptimerMinLookAwayTime->setSingleShot(true);
	QObject::connect(m_ptimerMinLookAwayTime, SIGNAL(timeout()), this, SLOT(minLookAwayTimeout()));

	m_ptimerMinLookTime = new QTimer();
	m_ptimerMinLookTime->setSingleShot(true);
	QObject::connect(m_ptimerMinLookTime, SIGNAL(timeout()), this, SLOT(minLookTimeReached()));

	m_ptimerMaxLookAway = new QTimer();
	m_ptimerMaxLookAway->setSingleShot(true);
	QObject::connect(m_ptimerMaxLookAway, SIGNAL(timeout()), this, SIGNAL(maxLookAwayTime()));

	m_ptimerMaxAccumulatedLook = new QTimer();
	m_ptimerMaxAccumulatedLook->setSingleShot(true);
	QObject::connect(m_ptimerMaxAccumulatedLook, SIGNAL(timeout()), this, SLOT(maxAccumulatedLookTimeReached()));

	// Create states and transitions for state machine.
	QState* sInitial = new QState();
	QState* sLooking = new QState();
	QState* sLookingAway = new QState();

	HTransToLookTransition* pTransToLook1;
	HTransToLookTransition* pTransToLook2;
	HTransToLookAwayTransition* pTransToLookAway;

	pTransToLook1 = new HTransToLookTransition();
	pTransToLook1->setTargetState(sLooking);
	sInitial->addTransition(pTransToLook1);
	addState(sInitial);

	pTransToLook2 = new HTransToLookTransition();
	pTransToLook2->setTargetState(sLooking);
	sLookingAway->addTransition(pTransToLook2);
	addState(sLookingAway);

	pTransToLookAway = new HTransToLookAwayTransition();
	pTransToLookAway->setTargetState(sLookingAway);
	sLooking->addTransition(pTransToLookAway);
	addState(sLooking);

	setInitialState(sInitial);

	connect(sInitial, SIGNAL(entered()), this, SLOT(onInitialStateEntered()));
	connect(sLookingAway, SIGNAL(entered()), this, SLOT(onLookingAwayStateEntered()));
	connect(sLooking, SIGNAL(entered()), this, SLOT(onLookingStateEntered()));
	connect(sLooking, SIGNAL(exited()), this, SLOT(onLookingStateExited()));
	connect(sLookingAway, SIGNAL(exited()), this, SLOT(onLookingAwayStateExited()));
};

void HLooker::addTrans(const HLookTrans& type, int tMS)
{
	m_log.append(new HLookTransEvent(type, tMS));
	m_transitions.append(qMakePair(&type, tMS));

	// Create a new event for the state machine to digest.
	if (type.isTransToLook())
	{
		postEvent(new HTransToLookQEvent(type, tMS));
	}
	else
	{
		postEvent(new HTransToLookAwayQEvent(type, tMS));
	}
	return;
}



bool HLooker::setMinLookTime(int t)
{
	bool b = false;

	// cannot do this if the looker is running
	if (this->isRunning())
	{
		qWarning() << "Cannot set min look time when HLooker is running.";
		b = false;
	}
	else
	{

		// We manage the timer
		// QTimer *m_ptimerMinLookTime;
		// Interval set when timer is started.

		m_minLookTimeMS = t;
		b = true;
	}
	return b;
}

bool HLooker::setMinLookAwayTime(int t)
{
	bool b = false;

	// cannot do this if the looker is running
	if (this->isRunning())
	{
		qWarning() << "Cannot set min look away time when HLooker is running.";
		b = false;
	}
	else
	{

		// We manage the timer
		// QTimer *m_ptimerMinLookAwayTime;
		// The interval is set when the timer is started

		m_minLookAwayTimeMS = t;
		b = true;
	}
	return b;
}


bool HLooker::setMaxAccumulatedLookTime(int t)
{
	bool b = false;

	// cannot do this if the looker is running
	if (this->isRunning())
	{
		qWarning() << "Cannot set max accum time when HLooker is running.";
		b = false;
	}
	else
	{

		// We manage the timer
		// QTimer *m_ptimerMaxAccumulatedLook;
		// by saving the max accumulated time. When the timer is started
		// the interval is set again.

		m_maxAccumulatedLookTimeMS = t;
		b = true;
	}
	return b;
}

bool HLooker::setMaxLookAwayTime(int t)
{
	bool b = false;

	// cannot do this if the looker is running
	if (this->isRunning())
	{
		qWarning() << "Cannot set max look away time when HLooker is running.";
		b = false;
	}
	else
	{

		// We manage the timer
		// QTimer *m_ptimerMaxLookAway;
		// by saving the max accumulated time. When the timer is started
		// the interval is set again.

		m_maxLookAwayTimeMS = t;
		b = true;
	}
	return b;
}

bool HLooker::analyzeLooking(int m_iLookPendingStartIndex, int& lookStartTimeMS, int& lastLookStartTimeMS, int& cumulativeLookTimeMS, int& lastLookAwayStartTimeMS)
{
	int tPrevious = -1;	// time of previous transition
	bool b = true;
	lookStartTimeMS = lastLookStartTimeMS = lastLookAwayStartTimeMS = -1;
	cumulativeLookTimeMS = 0;

	if (m_iLookPendingStartIndex > -1)
	{
		const QPair<const HLookTrans*, int>& p0 = m_transitions.at(m_iLookPendingStartIndex);
		lookStartTimeMS = p0.second;
		lastLookStartTimeMS = p0.second;
		tPrevious = p0.second;
		cumulativeLookTimeMS = 0;
		lastLookAwayStartTimeMS = -1;

		for (int i = m_iLookPendingStartIndex+1; i < (int)m_transitions.size(); i++)
		{
			const QPair<const HLookTrans*, int>& p = m_transitions.at(i);
			if ((p.first)->isTransToLook())
			{
				// the interval from tPrevious to p.second is a look away period.
				lastLookStartTimeMS = p.second;
			}
			else
			{
				cumulativeLookTimeMS += (p.second - tPrevious);
				lastLookAwayStartTimeMS = p.second;
			}
			tPrevious = p.second;
		}
	}
	return b;
}

int HLooker::getSumOfCompleteLooks()
{
	int sum = 0;
	for (int i=0; i<m_looks.size(); i++)
	{
		if (m_looks[i].direction() != HLookDirection::LookAway)
			sum += m_looks[i].lookMS();
	}
	return sum;
}


void HLooker::onInitialStateEntered()
{
	m_bLookPending = m_bLookAwayStarted = false;
	m_pdirectionLookPending = &HLookDirection::UnknownLookDirection;
	m_iLookPendingStartIndex = -1;
	m_transitions.clear();
	m_looks.clear();
}

void HLooker::onLookingStateEntered()
{
	const QPair<const HLookTrans*, int>& p = m_transitions.last(); // the transition that brought us here
	int lastLookAwayStartTimeMS = -1;
	int cumulativeLookTimeMS = 0;
	int lookStartTimeMS = -1;
	int lastLookStartTimeMS = -1;

	if (!analyzeLooking(m_iLookPendingStartIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS))
	{
		qCritical() << "Cannot analyze looking at index " << m_iLookPendingStartIndex;
		return;
	}

	// Stop timers if active.
	if (m_ptimerMinLookAwayTime->isActive())
	{
		m_ptimerMinLookAwayTime->stop();
	}
	if (m_ptimerMaxLookAway->isActive())
	{
		m_ptimerMaxLookAway->stop();
	}

	if (m_bLookPending)
	{

		// A look has started. Since we are entering looking state then the only possibility is
		// that a look-away had also started and now the subject is looking again.
		if (m_bLookAwayStarted)
		{

			// check if the time spent looking away is longer than the min look-away time,
			// OR if the looking away time was short, but the the look back is to a different screen.
			if ((lastLookStartTimeMS - lastLookAwayStartTimeMS) >= m_minLookAwayTimeMS ||
				((lastLookStartTimeMS - lastLookAwayStartTimeMS) < m_minLookAwayTimeMS && *m_pdirectionLookPending != directionTo(*m_transitions.last().first)))
			{

				// Any accumulated looking time should be flushed.
				// See if there was a complete look and emit look() if necessary. Then emit a look-away().
				// Finally, start a new look.
				if (cumulativeLookTimeMS >= m_minLookTimeMS)
				{
					HLook l(*m_pdirectionLookPending, lookStartTimeMS, lastLookAwayStartTimeMS, cumulativeLookTimeMS);
					m_looks.append(l);
					emit look(l);
				}

				// Starting a new potential look.
				m_bLookAwayStarted = false;
				m_bLookPending = true;
				m_pdirectionLookPending = &directionTo(*p.first);
				m_iLookPendingStartIndex = m_transitions.size()-1;
				m_ptimerMinLookTime->start(m_minLookTimeMS);
			}

			// The if here is redundant but left for clarity.
			// If you fall through the "if" above then the only possibility here is that the looking is in the same direction.

			//else if (*m_pdirectionLookPending == directionTo(*p.first))
			else
			{
				// looked back in same direction as before. resume previous look.
				m_bLookAwayStarted = false;

				// Its possible that the min look time has not been reached yet, so if that's the case then
				// start the min look timer.
				// If the min look time has been reached, then we may need to start the max accumulated look timer.

				if (cumulativeLookTimeMS < m_minLookTimeMS)
				{
					m_ptimerMinLookTime->start(m_minLookTimeMS - cumulativeLookTimeMS);
				}
				else if (m_maxAccumulatedLookTimeMS > 0)
				{
					int sumOfAllLooking = getSumOfCompleteLooks() + cumulativeLookTimeMS;
					if (sumOfAllLooking < m_maxAccumulatedLookTimeMS)
					{
						m_ptimerMaxAccumulatedLook->start(m_maxAccumulatedLookTimeMS - sumOfAllLooking);
					}
				}
			}
		}
		else
		{
			qCritical() << "onLookStateEntered(): lookStarted=true, lookAwayStarted=false!?!";
		}
	}
	else		// !m_bLookStarted
	{
		if (!m_bLookAwayStarted)
		{
			// This only occurs on initial look transition.
			emit lookStarted();
		}

		// start fresh new look
		m_bLookPending = true;
		m_iLookPendingStartIndex = m_transitions.size()-1;
		m_bLookAwayStarted = false;
		m_pdirectionLookPending = &directionTo(*p.first);

		// Start max accumulated look timer if needed
		if (m_maxAccumulatedLookTimeMS > 0)
		{
			int sumOfAllLooking = getSumOfCompleteLooks() + cumulativeLookTimeMS;
			if (sumOfAllLooking < m_maxAccumulatedLookTimeMS)
			{
				m_ptimerMaxAccumulatedLook->start(m_maxAccumulatedLookTimeMS - sumOfAllLooking);
			}
		}

	}
}

void HLooker::onLookingStateExited()
{
	if (m_ptimerMaxAccumulatedLook->isActive())
		m_ptimerMaxAccumulatedLook->stop();
}


void HLooker::onLookingAwayStateEntered()
{
	// If a look is already underway, start the look-away timer
	if (m_bLookPending)
	{
		m_ptimerMinLookAwayTime->start(m_minLookAwayTimeMS);
	}
	m_bLookAwayStarted = true;

	if (m_maxLookAwayTimeMS > 0)
	{
		m_ptimerMaxLookAway->start(m_maxLookAwayTimeMS);
	}
}

void HLooker::onLookingAwayStateExited()
{
	if (m_ptimerMinLookAwayTime->isActive())
		m_ptimerMinLookAwayTime->stop();
	if (m_ptimerMaxLookAway->isActive())
		m_ptimerMaxLookAway->stop();
}

void HLooker::maxAccumulatedLookTimeReached()
{
	emit maxAccumulatedLookTime();
}

void HLooker::minLookTimeReached()
{
	// this timeout indicates that the current looking has reached the minimum necessary to consider it a look().
	// Now we start the timer for the max accumulated look time if needed.

	if (m_maxAccumulatedLookTimeMS > 0)
	{
		int lastLookAwayStartTimeMS;
		int cumulativeLookTimeMS;
		int lookStartTimeMS;
		int lastLookStartTimeMS;

		if (!analyzeLooking(m_iLookPendingStartIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS))
		{
			qCritical() << "HLooker::minLookTimeReached() - Cannot analyze looking at index " << m_iLookPendingStartIndex;
			return;
		}

		// Its possible that the max accumulated look time was reached during the "min look" time.
		// Check if that's happened - if so then emit maxAccumulatedLookTime().
		// Otherwise, start max accumulated look timer.
		if (getSumOfCompleteLooks() + cumulativeLookTimeMS + m_minLookTimeMS > m_maxAccumulatedLookTimeMS)
		{
			emit maxAccumulatedLookTime();
		}
		else
		{
			m_ptimerMaxAccumulatedLook->start(m_maxAccumulatedLookTimeMS - cumulativeLookTimeMS - m_minLookTimeMS);
		}
	}
}

void HLooker::minLookAwayTimeout()
{
	// Was the look complete?
	if (m_bLookPending)
	{
		int lastLookAwayStartTimeMS;
		int cumulativeLookTimeMS;
		int lookStartTimeMS;
		int lastLookStartTimeMS;

		if (!analyzeLooking(m_iLookPendingStartIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS))
		{
			qCritical() << "HLooker::minLookAwayTimeout() - Cannot analyze looking at index " << m_iLookPendingStartIndex;
			return;
		}

		if (cumulativeLookTimeMS >= m_minLookTimeMS)
		{
			HLook l(*m_pdirectionLookPending, lookStartTimeMS, lastLookAwayStartTimeMS, cumulativeLookTimeMS);

			// When the look() below is emitted a trial may end (if its subject-controlled). In that
			// case the looker will be stopped and any pending looks (like this one) will be flushed.
			// Thus, we have to clear out vars here - before the emit - so the look will not be emitted twice.

			m_bLookPending = false;
			m_iLookPendingStartIndex = -1;
			m_looks.append(l);
			emit look(l);
		}
		else
		{
			m_bLookPending = false;
		}
	}
	emit lookAwayStarted();
}


void HLooker::stopLooker(int tMS)
{
	// the looker will be stopped, it will not enter a final state.
	// Stop any active timers.
	// Flush out any look or lookaway pending.

	if (m_ptimerMinLookTime->isActive()) m_ptimerMinLookTime->stop();
	if (m_ptimerMinLookAwayTime->isActive()) m_ptimerMinLookAwayTime->stop();
	if (m_ptimerMaxLookAway->isActive()) m_ptimerMaxLookAway->stop();
	if (m_ptimerMaxAccumulatedLook->isActive()) m_ptimerMaxAccumulatedLook->stop();

	if (m_bLookPending)
	{
		int lastLookAwayStartTimeMS;
		int cumulativeLookTimeMS;
		int lookStartTimeMS;
		int lastLookStartTimeMS;

		if (!analyzeLooking(m_iLookPendingStartIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS))
		{
			qCritical() << "Cannot analyze looking at index " << m_iLookPendingStartIndex;
			return;
		}


		if (m_bLookAwayStarted)
		{
			// Since a lookaway has started, the current state is LookAway, so the cumulativeLookTime is the
			// it, no need to add the additional from last transition to tMS. If the cumulativeLookTime is
			// sufficient, emit a look, otherwise do nothing.
			if (cumulativeLookTimeMS >= m_minLookTimeMS)
			{
				HLook l(*m_pdirectionLookPending, lookStartTimeMS, lastLookAwayStartTimeMS, cumulativeLookTimeMS);
				m_looks.append(l);
				emit look(l);
				m_bLookPending = false;
				m_iLookPendingStartIndex = -1;
			}
		}
		else
		{
			// we were in a looking state, so add in the time from tMS to the lastLookStartTime as additional look time and test.
			cumulativeLookTimeMS += tMS - lastLookStartTimeMS;
			if (cumulativeLookTimeMS >= m_minLookTimeMS)
			{
				HLook l(*m_pdirectionLookPending, lookStartTimeMS, tMS, cumulativeLookTimeMS);
				m_looks.append(l);
				emit look(l);
				m_bLookPending = false;
				m_iLookPendingStartIndex = -1;
			}
		}
	}
	stop();
}

