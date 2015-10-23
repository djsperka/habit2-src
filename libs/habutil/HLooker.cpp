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
#include <QCoreApplication>

HLooker::HLooker(HEventLog& log, bool bInclusiveLookTime)
: QStateMachine()
, m_minLookTimeMS(0)
, m_minLookAwayTimeMS(0)
, m_maxLookAwayTimeMS(0)
, m_maxAccumulatedLookTimeMS(0)
, m_log(log)
, m_bInclusiveLookTime(bInclusiveLookTime)
, m_bLookStarted(false)
, m_pdirectionLookStarted(&HLookDirection::LookAway)
, m_iLookStartedIndex(-1)
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
	//QObject::connect(m_ptimerMaxLookAway, SIGNAL(timeout()), this, SIGNAL(maxLookAwayTime()));
	QObject::connect(m_ptimerMaxLookAway, SIGNAL(timeout()), this, SLOT(maxLookAwayTimeout()));

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
	connect(this, SIGNAL(started()), this, SLOT(onStarted()));
};

void HLooker::onStarted()
{
}

void HLooker::addTrans(const HLookTrans& type, int tMS)
{
	m_log.append(new HLookTransEvent(type, tMS));
	m_transitions.append(qMakePair(&type, tMS));

	// Create a new event for the state machine to digest.
	if (!isRunning())
		qDebug() << "State Machine is not running!";
	if (type.isTransToLook())
	{
		postEvent(new HTransToLookQEvent(type, tMS));
	}
	else
	{
		postEvent(new HTransToLookAwayQEvent(type, tMS));
	}

	emit lookingDirection(directionTo(type).name());

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


// Look at the look transitions recorded, starting at m_transitions.at(iLookStartIndex), up to and including the last transition recorded.
// This function should only be called when iLookStartIndex is < 0, or when the transition at iLookStartIndex is a transition to looking.
// Sums up all looking based on those transitions. Looks away from target(s) are ignored. No attempt is made to analyze where the looking
// is directed, so looks at different targets would be summed up together here (best take care of the different looking directions elsewhere).
// Returns true always;)
// lookStartTimeMS - the time of the first transition to looking
// lastLookStartTimeMS - if there are multiple transitions, the time of the last transition to looking
// cumulativeLookTimeMS - sum of all looking periods for the transitions analyzed
// lastLookAwayStartTimeMS - if there are multiple transitions, the time of the last transition to looking AWAY (-1 if no such transition)
//


bool HLooker::analyzeLooking(int iLookStartIndex, int& lookStartTimeMS, int& lastLookStartTimeMS, int& cumulativeLookTimeMS, int& lastLookAwayStartTimeMS)
{
	int tPrevious = -1;	// time of previous transition
	bool b = true;
	lookStartTimeMS = lastLookStartTimeMS = lastLookAwayStartTimeMS = -1;
	cumulativeLookTimeMS = 0;

	if (iLookStartIndex > -1)
	{
		const QPair<const HLookTrans*, int>& p0 = m_transitions.at(iLookStartIndex);
		lookStartTimeMS = p0.second;
		lastLookStartTimeMS = p0.second;
		tPrevious = p0.second;
		cumulativeLookTimeMS = 0;
		lastLookAwayStartTimeMS = -1;

		for (int i = iLookStartIndex+1; i < (int)m_transitions.size(); i++)
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
	m_bLookStarted = m_bLookAwayStarted = false;
	m_pdirectionLookStarted = &HLookDirection::UnknownLookDirection;
	m_iLookStartedIndex = -1;
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

	if (!analyzeLooking(m_iLookStartedIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS))
	{
		qCritical() << "Cannot analyze looking at index " << m_iLookStartedIndex;
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

	if (m_bLookStarted)
	{

		// A look has started. Since we are entering looking state then the only possibility is
		// that a look-away had also started and now the subject is looking again.
		if (m_bLookAwayStarted)
		{

			// check if the time spent looking away is longer than the min look-away time,
			// OR if the looking away time was short, but the the look back is to a different screen.
			if ((lastLookStartTimeMS - lastLookAwayStartTimeMS) >= m_minLookAwayTimeMS ||
				((lastLookStartTimeMS - lastLookAwayStartTimeMS) < m_minLookAwayTimeMS && *m_pdirectionLookStarted != directionTo(*m_transitions.last().first)))
			{

				// Any accumulated looking time should be flushed.
				// See if there was a complete look and emit look() if necessary. Then emit a look-away().
				// Finally, start a new look.
				if (	(!m_bInclusiveLookTime && cumulativeLookTimeMS >= m_minLookTimeMS) ||
						(m_bInclusiveLookTime && (lastLookAwayStartTimeMS - lookStartTimeMS) >= m_minLookTimeMS) )
				{
					HLook l;
					l.setDirection(*m_pdirectionLookStarted);
					l.setStartMS(lookStartTimeMS);
					l.setEndMS(lastLookAwayStartTimeMS);
					if (m_bInclusiveLookTime)
					{
						l.setLookMS(lastLookAwayStartTimeMS - lookStartTimeMS);
					}
					else
					{
						l.setLookMS(cumulativeLookTimeMS);
					}
					m_looks.append(l);
					emit look(l);
				}
				else
				{
					emit lookAborted();
				}

				// Starting a new potential look.
				emit lookStarted();
				m_bLookAwayStarted = false;
				m_bLookStarted = true;
				m_pdirectionLookStarted = &directionTo(*p.first);
				m_iLookStartedIndex = m_transitions.size()-1;
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
				// Update: This gets tedious, but separate into two cases, inclusive look time and not.

				if (!m_bInclusiveLookTime)
				{
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
				else
				{
					if ((lastLookAwayStartTimeMS - lookStartTimeMS) < m_minLookTimeMS)
					{
						m_ptimerMinLookTime->start(m_minLookTimeMS - (lastLookAwayStartTimeMS - lookStartTimeMS));
					}
					else if (m_maxAccumulatedLookTimeMS > 0)
					{
						int sumOfAllLooking = getSumOfCompleteLooks() + (lastLookAwayStartTimeMS - lookStartTimeMS);
						if (sumOfAllLooking < m_maxAccumulatedLookTimeMS)
						{
							m_ptimerMaxAccumulatedLook->start(m_maxAccumulatedLookTimeMS - sumOfAllLooking);
						}
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
		emit lookStarted();
		m_bLookStarted = true;
		m_iLookStartedIndex = m_transitions.size()-1;
		m_bLookAwayStarted = false;
		m_pdirectionLookStarted = &directionTo(*p.first);

		// Since we are starting a new look, start the min look timer.
		m_ptimerMinLookTime->start(m_minLookTimeMS - cumulativeLookTimeMS);
	}
}

void HLooker::onLookingStateExited()
{
	if (m_ptimerMinLookTime->isActive())
		m_ptimerMinLookTime->stop();
	if (m_ptimerMaxAccumulatedLook->isActive())
		m_ptimerMaxAccumulatedLook->stop();
}


void HLooker::onLookingAwayStateEntered()
{
	// If a look is already underway, start the look-away timer
	if (m_bLookStarted)
	{
		m_ptimerMinLookAwayTime->start(m_minLookAwayTimeMS);
	}
	m_bLookAwayStarted = true;

	// If there is a max look away time, start the timer ONLY IF
	// there has been at least one complete look.
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

	emit lookPending();
	if (m_maxAccumulatedLookTimeMS > 0)
	{
		int lastLookAwayStartTimeMS;
		int cumulativeLookTimeMS;
		int lookStartTimeMS;
		int lastLookStartTimeMS;

		if (!analyzeLooking(m_iLookStartedIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS))
		{
			qCritical() << "HLooker::minLookTimeReached() - Cannot analyze looking at index " << m_iLookStartedIndex;
			return;
		}

		// Its possible that the max accumulated look time was reached during the "min look" time.
		// Check if that's happened - if so then emit maxAccumulatedLookTime().
		// Otherwise, start max accumulated look timer.
		// djs 10-14-2015. Be careful when summing up total looking at this point. Why? Because we should be in a looking
		// state right now, but we don't know for how long. We cannot assume that its been for m_minLookTimeMS, because
		// there may have been a short look-away followed by a return to looking. In that case, the looking prior to the
		// look-away is included in cumulativeLookTimeMS, and it was taken into account when starting the minLookTime timer.
		// A better way is to add the time this timer was initialized with.
		int sumOfAllLooking = getSumOfCompleteLooks() + cumulativeLookTimeMS + m_ptimerMinLookTime->interval();
		if (sumOfAllLooking > m_maxAccumulatedLookTimeMS)
		{
			emit maxAccumulatedLookTime();
		}
		else
		{
			m_ptimerMaxAccumulatedLook->start(m_maxAccumulatedLookTimeMS - sumOfAllLooking);
		}
	}
}

void HLooker::minLookAwayTimeout()
{
	// Was the look complete?
	if (m_bLookStarted)
	{
		int lastLookAwayStartTimeMS;
		int cumulativeLookTimeMS;
		int lookStartTimeMS;
		int lastLookStartTimeMS;

		if (!analyzeLooking(m_iLookStartedIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS))
		{
			qCritical() << "HLooker::minLookAwayTimeout() - Cannot analyze looking at index " << m_iLookStartedIndex;
			return;
		}

		if (cumulativeLookTimeMS >= m_minLookTimeMS)
		{
			HLook l;
			//HLook l(*m_pdirectionLookStarted, lookStartTimeMS, lastLookAwayStartTimeMS, cumulativeLookTimeMS);

			// When the look() below is emitted a trial may end (if its subject-controlled). In that
			// case the looker will be stopped and any pending looks (like this one) will be flushed.
			// Thus, we have to clear out vars here - before the emit - so the look will not be emitted twice.

			m_bLookStarted = false;
			m_iLookStartedIndex = -1;

			l.setDirection(*m_pdirectionLookStarted);
			l.setStartMS(lookStartTimeMS);
			l.setEndMS(lastLookAwayStartTimeMS);
			if (m_bInclusiveLookTime)
			{
				l.setLookMS(lastLookAwayStartTimeMS - lookStartTimeMS);
			}
			else
			{
				l.setLookMS(cumulativeLookTimeMS);
			}

			m_looks.append(l);
			emit look(l);
		}
		else
		{
			emit lookAborted();
			m_bLookStarted = false;
			m_iLookStartedIndex = -1;
		}
	}
	//emit lookAwayStarted();
}

void HLooker::maxLookAwayTimeout()
{
	// We only trigger the transition if there is at least
	// one complete look.
	// CAUTION: If the maxLookAwayTime is LESS THAN the minLookAwayTime
	// then when you get here there is NOT a complete look.
	if (m_looks.count() > 0)
		emit maxLookAwayTime();
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

	if (m_bLookStarted)
	{
		int lastLookAwayStartTimeMS;
		int cumulativeLookTimeMS;
		int lookStartTimeMS;
		int lastLookStartTimeMS;

		if (!analyzeLooking(m_iLookStartedIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS))
		{
			qCritical() << "Cannot analyze looking at index " << m_iLookStartedIndex;
			return;
		}


		if (m_bLookAwayStarted)
		{
			// Since a lookaway has started, the current state is LookAway, so the cumulativeLookTime is the
			// it, no need to add the additional from last transition to tMS. If the cumulativeLookTime is
			// sufficient, emit a look, otherwise do nothing.
			if (cumulativeLookTimeMS >= m_minLookTimeMS)
			{
				//HLook l(*m_pdirectionLookStarted, lookStartTimeMS, lastLookAwayStartTimeMS, cumulativeLookTimeMS);
				HLook l;
				l.setDirection(*m_pdirectionLookStarted);
				l.setStartMS(lookStartTimeMS);
				l.setEndMS(lastLookAwayStartTimeMS);
				if (m_bInclusiveLookTime)
				{
					l.setLookMS(lastLookAwayStartTimeMS - lookStartTimeMS);
				}
				else
				{
					l.setLookMS(cumulativeLookTimeMS);
				}
				m_looks.append(l);
				emit look(l);
				m_bLookStarted = false;
				m_iLookStartedIndex = -1;
			}
		}
		else
		{
			// we were in a looking state, so add in the time from tMS to the lastLookStartTime as additional look time and test.
			cumulativeLookTimeMS += tMS - lastLookStartTimeMS;
			if (cumulativeLookTimeMS >= m_minLookTimeMS)
			{
				//HLook l(*m_pdirectionLookStarted, lookStartTimeMS, tMS, cumulativeLookTimeMS);
				HLook l;
				l.setDirection(*m_pdirectionLookStarted);
				l.setStartMS(lookStartTimeMS);
				l.setEndMS(tMS);
				if (m_bInclusiveLookTime)
				{
					l.setLookMS(tMS - lookStartTimeMS);
				}
				else
				{
					l.setLookMS(cumulativeLookTimeMS);
				}
				m_looks.append(l);
				emit look(l);
				m_bLookStarted = false;
				m_iLookStartedIndex = -1;
			}
		}
	}
	stop();
	QCoreApplication::processEvents(0);
}

