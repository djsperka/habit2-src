/*
 *  HLooker.cpp
 *  hstate
 *
 *  Created by Oakes Lab on 6/13/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HLooker.h"
#include "HElapsedTimer.h"
#include <QtDebug>
#include <QCoreApplication>

HLooker::HLooker(const HLooker& looker)
: QStateMachine()
, m_bIsLive(looker.isLive())
, m_minLookTimeMS(looker.m_minLookTimeMS)
, m_minLookAwayTimeMS(looker.m_minLookAwayTimeMS)
, m_maxLookAwayTimeMS(looker.m_maxLookAwayTimeMS)
, m_maxAccumulatedLookTimeMS(looker.m_maxAccumulatedLookTimeMS)
, m_phaseAccumulatedLookTimeMS(looker.m_phaseAccumulatedLookTimeMS)
, m_log(looker.m_log)
, m_bInclusiveLookTime(looker.m_bInclusiveLookTime)
, m_bLookStarted(looker.m_bLookStarted)
, m_pdirectionLookStarted(looker.m_pdirectionLookStarted)
, m_iLookStartedIndex(looker.m_iLookStartedIndex)
, m_bLookAwayStarted(looker.m_bLookAwayStarted)
{
	qDebug() << "HLooker(const HLooker&) -- ?????";
}

HLooker::HLooker(HEventLog& log, bool bIsLive)
: QStateMachine()
, m_bIsLive(bIsLive)
, m_minLookTimeMS(0)
, m_minLookAwayTimeMS(0)
, m_maxLookAwayTimeMS(0)
, m_maxAccumulatedLookTimeMS(0)
, m_phaseAccumulatedLookTimeMS(0)
, m_log(log)
, m_bInclusiveLookTime(false)
, m_bLookStarted(false)
, m_pdirectionLookStarted(&HLookDirection::LookAway)
, m_iLookStartedIndex(-1)
, m_bLookAwayStarted(false)
{
	// There are 4 timers created here. Each is single shot, and each will have its interval set when it is started.
	// Each can have its associated time values modified when the HLooker is not running.

	if (m_bIsLive)
	{
		m_ptimerMinLookAwayTime = new QTimer();
		m_ptimerMinLookAwayTime->setTimerType(Qt::PreciseTimer);
		m_ptimerMinLookAwayTime->setSingleShot(true);
		QObject::connect(m_ptimerMinLookAwayTime, SIGNAL(timeout()), this, SLOT(minLookAwayTimeout()));

		m_ptimerMinLookTime = new QTimer();
		m_ptimerMinLookTime->setTimerType(Qt::PreciseTimer);
		m_ptimerMinLookTime->setSingleShot(true);
		QObject::connect(m_ptimerMinLookTime, SIGNAL(timeout()), this, SLOT(minLookTimeReached()));

		m_ptimerMaxLookAway = new QTimer();
		m_ptimerMaxLookAway->setTimerType(Qt::PreciseTimer);
		m_ptimerMaxLookAway->setSingleShot(true);
		//QObject::connect(m_ptimerMaxLookAway, SIGNAL(timeout()), this, SIGNAL(maxLookAwayTime()));
		QObject::connect(m_ptimerMaxLookAway, SIGNAL(timeout()), this, SLOT(maxLookAwayTimeout()));

		m_ptimerMaxAccumulatedLook = new QTimer();
		m_ptimerMaxAccumulatedLook->setTimerType(Qt::PreciseTimer);
		m_ptimerMaxAccumulatedLook->setSingleShot(true);
		QObject::connect(m_ptimerMaxAccumulatedLook, SIGNAL(timeout()), this, SLOT(maxAccumulatedLookTimeReached()));

		m_ptimerPhaseAccumulatedLook = new QTimer();
		m_ptimerPhaseAccumulatedLook->setTimerType(Qt::PreciseTimer);
		m_ptimerPhaseAccumulatedLook->setSingleShot(true);
		QObject::connect(m_ptimerPhaseAccumulatedLook, SIGNAL(timeout()), this, SLOT(phaseAccumulatedLookTimeReached()));

	}

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
	connect(this, SIGNAL(runningChanged(bool)), this, SLOT(onRunningChanged(bool)));
};

void HLooker::onRunningChanged(bool b)
{
	if (b)
		qDebug() << "HLooker::onRunningChanged(): STARTED";
	else
		qDebug() << "HLooker::onRunningChanged(): STOPPED";

}

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

bool HLooker::setIsInclusiveLookTime(bool bInclusive)
{
	bool b = false;

	// cannot do this if the looker is running
	if (this->isRunning())
	{
		qWarning() << "Cannot set inclusive look time when HLooker is running.";
		b = false;
	}
	else
	{
		m_bInclusiveLookTime = bInclusive;
		b = true;
	}
	return b;
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
		m_maxLookAwayTimeMS = t;
		b = true;
	}
	return b;
}

bool HLooker::setPhaseAccumulatedLookTime(int tPhaseAccum)
{
	bool b = false;

	// cannot do this if the looker is running
	if (this->isRunning())
	{
		qWarning() << "Cannot set phase accum look time when HLooker is running.";
		b = false;
	}
	else
	{
		m_phaseAccumulatedLookTimeMS = tPhaseAccum;
		b = true;
	}
	return b;
}


// Look at the look transitions recorded, starting at m_transitions.at(iLookStartIndex), up to and including the last transition recorded.
// This function should only be called when iLookStartIndex is > -1, when the transition at iLookStartIndex is a transition to looking.
// Sums up all looking based on those transitions. Looks away from target(s) are ignored. No attempt is made to analyze where the looking
// is directed, so looks at different targets would be summed up together here (best take care of the different looking directions elsewhere).
// Returns true always;)
// lookStartTimeMS - the time of the first transition to looking
// lastLookStartTimeMS - if there are multiple transitions, the time of the last transition to looking
// cumulativeLookTimeMS - sum of all looking periods for the transitions analyzed
// lastLookAwayStartTimeMS - if there are multiple transitions, the time of the last transition to looking AWAY (-1 if no such transition)
//


bool HLooker::analyzeLooking(int iLookStartIndex, int& lookStartTimeMS, int& lastLookStartTimeMS, int& cumulativeLookTimeMS, int& lastLookAwayStartTimeMS, HLookList& sublooks)
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
				// create a Look for the sublooks list
				HLook sub(directionFrom(*(p.first)), tPrevious, p.second);
				sublooks.append(sub);

				// tally cumulative look time
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
	HLookList sublooks;

	if (!analyzeLooking(m_iLookStartedIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS, sublooks))
	{
		qCritical() << "Cannot analyze looking at index " << m_iLookStartedIndex;
		return;
	}

	// Stop timers if active.
	if (isLive())
	{
		if (m_ptimerMinLookAwayTime->isActive())
		{
			m_ptimerMinLookAwayTime->stop();
		}
		if (m_ptimerMaxLookAway->isActive())
		{
			m_ptimerMaxLookAway->stop();
		}
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
					l.setSublooks(sublooks);
					if (m_bInclusiveLookTime)
					{
						l.setLookMS(lastLookAwayStartTimeMS - lookStartTimeMS);
					}
					else
					{
						l.setLookMS(cumulativeLookTimeMS);
					}
					m_looks.append(l);
					qDebug() << "emit look(1): " << l;
					qDebug() << "sublooks follow" << endl << sublooks;

					// update looking parameters before emit
					// This look() may end a trial. In the steps ending a trial, the looker will
					// be stopped, and may re-emit the same look.
					m_bLookAwayStarted = false;
					m_bLookStarted = true;
					m_pdirectionLookStarted = &directionTo(*p.first);
					m_iLookStartedIndex = m_transitions.size()-1;

					emit look(l);
				}
				else
				{
					// Emit lookAborted once for each sublook that is in a look direction.
					//qDebug() << "emit lookAborted(1)";
					//qDebug() << "sublooks follow" << endl << sublooks;

					QListIterator<HLook> it(sublooks);
					while (it.hasNext())
					{
						HLook l = it.next();
						if (isLookToAnyStimulus(l))
						{
					//		qDebug() << "sublook: ";
					//		qDebug() << l;
							emit lookAborted(l);
						}
					}
				}

				// Starting a new potential look.
				// These two bools may have been set above before look emitted. Have to make
				// sure it happens there because the emit look() can lead to the look() being emitted twice.
				m_bLookAwayStarted = false;
				m_bLookStarted = true;
				m_pdirectionLookStarted = &directionTo(*p.first);
				m_iLookStartedIndex = m_transitions.size()-1;
				emit lookStarted();
				if (isLive())
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

				if (isLive())
				{
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
//								qDebug() << "HLooker::onLookingStateEntered() - !inclusive - started accum timer for (m_maxAccumulatedLookTimeMS - sumOfAllLooking)=" << (m_maxAccumulatedLookTimeMS - sumOfAllLooking) << " ms";
//								qDebug() << "                                m_maxAccumulatedLookTimeMS=" << m_maxAccumulatedLookTimeMS;
//								qDebug() << "                                sumOfCompleteLooks=" << getSumOfCompleteLooks();
//								qDebug() << "                                cumulativeLookTimeMS= " << cumulativeLookTimeMS;
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
//								qDebug() << "HLooker::onLookingStateEntered() - inclusive - started accum timer for (m_maxAccumulatedLookTimeMS - sumOfAllLooking)=" << (m_maxAccumulatedLookTimeMS - sumOfAllLooking) << " ms";
//								qDebug() << "                                m_maxAccumulatedLookTimeMS=" << m_maxAccumulatedLookTimeMS;
//								qDebug() << "                                sumOfCompleteLooks=" << getSumOfCompleteLooks();
//								qDebug() << "                                (lastLookAwayStartTimeMS - lookStartTimeMS)";
//								qDebug() << "                                lastLookAwayStartTimeMS= " << lastLookAwayStartTimeMS;
//								qDebug() << "                                lookStartTimeMS= " << lookStartTimeMS;
							}
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

		if (isLive())
		{
			// Since we are starting a new look, start the min look timer.
			//qDebug() << "onLookingStateEntered: minLookTimeMS " << m_minLookTimeMS << " cum look time " << cumulativeLookTimeMS;
			m_ptimerMinLookTime->start(m_minLookTimeMS - cumulativeLookTimeMS);
		}
	}
}

void HLooker::onLookingStateExited()
{
	if (isLive())
	{
		if (m_ptimerMinLookTime->isActive())
			m_ptimerMinLookTime->stop();
		if (m_ptimerMaxAccumulatedLook->isActive())
			m_ptimerMaxAccumulatedLook->stop();
		if (m_ptimerPhaseAccumulatedLook->isActive())
			m_ptimerPhaseAccumulatedLook->stop();
	}
}


void HLooker::onLookingAwayStateEntered()
{
	// If a look is already underway, start the look-away timer
	if (m_bLookStarted)
	{
		if (isLive())
			m_ptimerMinLookAwayTime->start(m_minLookAwayTimeMS);
	}
	m_bLookAwayStarted = true;

	// If there is a max look away time, start the timer. Note that when
	// the timer goes off, we will emit maxLookAway() ONLY IF
	// there has been at least one complete look. See maxLookAwayTimeout().
	if (m_maxLookAwayTimeMS > 0)
	{
		if (isLive())
			m_ptimerMaxLookAway->start(m_maxLookAwayTimeMS);
	}
}

void HLooker::onLookingAwayStateExited()
{
	if (isLive())
	{
		if (m_ptimerMinLookAwayTime->isActive())
			m_ptimerMinLookAwayTime->stop();
		if (m_ptimerMaxLookAway->isActive())
			m_ptimerMaxLookAway->stop();
	}
}

void HLooker::maxAccumulatedLookTimeReached()
{
	emit maxAccumulatedLookTime();
}

void HLooker::phaseAccumulatedLookTimeReached()
{
	emit phaseAccumulatedLookTime();
}

void HLooker::minLookTimeReached()
{
	int lastLookAwayStartTimeMS;
	int cumulativeLookTimeMS;
	int lookStartTimeMS;
	int lastLookStartTimeMS;
	HLookList sublooks;

	if (!analyzeLooking(m_iLookStartedIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS, sublooks))
	{
		qCritical() << "HLooker::minLookTimeReached() - Cannot analyze looking at index " << m_iLookStartedIndex;
		return;
	}

	//qDebug() << "minLookTimeReached: cum look time MS " << cumulativeLookTimeMS << " look start time " << lookStartTimeMS << " phase accum " << m_phaseAccumulatedLookTimeMS;
	// this timeout indicates that the current looking has reached the minimum necessary to consider it a look().
	// Now we start the timer for the max accumulated look time if needed.

	emit lookPending();

	if (m_maxAccumulatedLookTimeMS > 0)
	{
		// Its possible that the max accumulated look time was reached during the "min look" time.
		// Check if that's happened - if so then emit maxAccumulatedLookTime().
		// Otherwise, start max accumulated look timer.
		// djs 10-14-2015. Be careful when summing up total looking at this point. Why? Because we should be in a looking
		// state right now, but we don't know for how long. We cannot assume that its been for m_minLookTimeMS, because
		// there may have been a short look-away followed by a return to looking. In that case, the looking prior to the
		// look-away is included in cumulativeLookTimeMS, and it was taken into account when starting the minLookTime timer.
		// A better way is to add the time this timer was initialized with.
		// djs 8-16-2018. When using inclusive looks, the timer can be set with incorrect time, because the non-looking
		// periods are excluded. Thus, if you had a short look-away that was part of this look, the max accumulated looking
		// timer will be set with too much time on it (extra by the amount of look-away). Fix this by separating the inclusive and
		// !inclusive cases.


		if (!m_bInclusiveLookTime)
		{
			int sumOfAllLooking = getSumOfCompleteLooks() + cumulativeLookTimeMS + m_ptimerMinLookTime->interval();
			if (sumOfAllLooking > m_maxAccumulatedLookTimeMS)
			{
				emit maxAccumulatedLookTime();
			}
			else
			{
				if (isLive())
				{
//					qDebug() << "analyzeLooking(): m_iLookStartedIndex = " << m_iLookStartedIndex;
//					qDebug() << "analyzeLooking(): lookStartTimeMS = " << lookStartTimeMS;
//					qDebug() << "analyzeLooking(): lastLookStartTimeMS = " << lastLookStartTimeMS;
//					qDebug() << "analyzeLooking(): cumulativeLookTimeMS = " << cumulativeLookTimeMS;
//					qDebug() << "analyzeLooking(): lastLookAwayStartTimeMS = " << lastLookAwayStartTimeMS;
//					qDebug() << "analyzeLooking(): sublooks.size = " << sublooks.size();
					m_ptimerMaxAccumulatedLook->start(m_maxAccumulatedLookTimeMS - sumOfAllLooking);
				}
			}
		}
		else
		{
			// looking time ignores looks-away.
			int sumOfAllLooking = getSumOfCompleteLooks() + (HElapsedTimer::elapsed() - lookStartTimeMS);
			if (sumOfAllLooking > m_maxAccumulatedLookTimeMS)
			{
				emit maxAccumulatedLookTime();
			}
			else
			{
				if (isLive())
				{
					m_ptimerMaxAccumulatedLook->start(m_maxAccumulatedLookTimeMS - sumOfAllLooking);
				}
			}
		}
	}

	// check phaseAccumulatedLookTime
	if (m_phaseAccumulatedLookTimeMS > 0)
	{
		if (!m_bInclusiveLookTime)
		{
			int sumOfAllLooking = getSumOfCompleteLooks() + cumulativeLookTimeMS + m_ptimerMinLookTime->interval();
//			qDebug() << "set phase accum timer ms " << (m_phaseAccumulatedLookTimeMS - sumOfAllLooking);
//			qDebug() << "sumOfAllLooking= (getSumOfCompleteLooks()=" <<  getSumOfCompleteLooks() << ") + cumLookTime(" << cumulativeLookTimeMS << ") + interval(" << m_ptimerMinLookTime->interval() << ")";
			if (sumOfAllLooking > m_phaseAccumulatedLookTimeMS)
			{
				emit phaseAccumulatedLookTime();
			}
			else
			{
				if (isLive())
				{
					m_ptimerPhaseAccumulatedLook->start(m_phaseAccumulatedLookTimeMS - sumOfAllLooking);
				}
			}
		}
		else
		{
			// looking time ignores looks-away.
			int sumOfAllLooking = getSumOfCompleteLooks() + (HElapsedTimer::elapsed() - lookStartTimeMS);
//			qDebug() << "set phase accum timer ms " << (m_phaseAccumulatedLookTimeMS - sumOfAllLooking);
//			qDebug() << "sumOfAllLooking= (getSumOfCompleteLooks()=" <<  getSumOfCompleteLooks() << ") + (elapsed=" << HElapsedTimer::elapsed() << "-lookStartTimeMS=" << lookStartTimeMS;

			if (sumOfAllLooking > m_phaseAccumulatedLookTimeMS)
			{
				emit phaseAccumulatedLookTime();
			}
			else
			{
				if (isLive())
				{
					m_ptimerPhaseAccumulatedLook->start(m_phaseAccumulatedLookTimeMS - sumOfAllLooking);
				}
			}
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
		HLookList sublooks;
		if (!analyzeLooking(m_iLookStartedIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS, sublooks))
		{
			qCritical() << "HLooker::minLookAwayTimeout() - Cannot analyze looking at index " << m_iLookStartedIndex;
			return;
		}

		//qDebug() << "analyzeLooking(): m_iLookStartedIndex = " << m_iLookStartedIndex;
		//qDebug() << "analyzeLooking(): lookStartTimeMS = " << lookStartTimeMS;
		//qDebug() << "analyzeLooking(): lastLookStartTimeMS = " << lastLookStartTimeMS;
		//qDebug() << "analyzeLooking(): cumulativeLookTimeMS = " << cumulativeLookTimeMS;
		//qDebug() << "analyzeLooking(): lastLookAwayStartTimeMS = " << lastLookAwayStartTimeMS;
		//qDebug() << "analyzeLooking(): sublooks.size = " << sublooks.size();
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
			l.setSublooks(sublooks);
			if (m_bInclusiveLookTime)
			{
				l.setLookMS(lastLookAwayStartTimeMS - lookStartTimeMS);
				//qDebug() << "set look time to (lastLookAwayStartTimeMS - lookStartTimeMS)= " << lastLookAwayStartTimeMS << "-" << lookStartTimeMS;
			}
			else
			{
				l.setLookMS(cumulativeLookTimeMS);
				//qDebug() << "set look time to cumulativeLookTimeMS=" << cumulativeLookTimeMS;
			}

			m_looks.append(l);
			qDebug() << "emit look(2, inclusive=" << m_bInclusiveLookTime << "): " << l;
			qDebug() << "sublooks follow" << endl << sublooks;
			emit look(l);

		}
		else
		{
			// Emit lookAborted once for each sublook that is in a look direction.
			//qDebug() << "emit lookAborted(1)";
			//qDebug() << "sublooks follow" << endl << sublooks;

			QListIterator<HLook> it(sublooks);
			while (it.hasNext())
			{
				HLook l = it.next();
				if (isLookToAnyStimulus(l))
				{
//					qDebug() << "sublook: ";
//					qDebug() << l;
					emit lookAborted(l);
				}
			}
			m_bLookStarted = false;
			m_iLookStartedIndex = -1;
		}
	}
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

	qDebug() << "HLooker::stopLooker()";
	if (isLive())
	{
		qDebug() << "HLooker::stopLooker() - isLive() = true";
		if (m_ptimerMinLookTime->isActive()) m_ptimerMinLookTime->stop();
		if (m_ptimerMinLookAwayTime->isActive()) m_ptimerMinLookAwayTime->stop();
		if (m_ptimerMaxLookAway->isActive()) m_ptimerMaxLookAway->stop();
		if (m_ptimerMaxAccumulatedLook->isActive()) m_ptimerMaxAccumulatedLook->stop();
		if (m_ptimerPhaseAccumulatedLook->isActive()) m_ptimerPhaseAccumulatedLook->stop();
	}

	if (m_bLookStarted)
	{
		int lastLookAwayStartTimeMS;
		int cumulativeLookTimeMS;
		int lookStartTimeMS;
		int lastLookStartTimeMS;
		HLookList sublooks;
		qDebug() << "HLooker::stopLooker() - m_bLookStarted = true";
		if (!analyzeLooking(m_iLookStartedIndex, lookStartTimeMS, lastLookStartTimeMS, cumulativeLookTimeMS, lastLookAwayStartTimeMS, sublooks))
		{
			qCritical() << "Cannot analyze looking at index " << m_iLookStartedIndex;
			return;
		}

		qDebug() << "analyzeLooking(): m_iLookStartedIndex = " << m_iLookStartedIndex;
		qDebug() << "analyzeLooking(): lookStartTimeMS = " << lookStartTimeMS;
		qDebug() << "analyzeLooking(): lastLookStartTimeMS = " << lastLookStartTimeMS;
		qDebug() << "analyzeLooking(): cumulativeLookTimeMS = " << cumulativeLookTimeMS;
		qDebug() << "analyzeLooking(): lastLookAwayStartTimeMS = " << lastLookAwayStartTimeMS;
		qDebug() << "analyzeLooking(): sublooks.size = " << sublooks.size();


		if (m_bLookAwayStarted)
		{
			// Since a lookaway has started, the current state is LookAway, so the cumulativeLookTime is the
			// it, no need to add the additional from last transition to tMS. If the cumulativeLookTime is
			// sufficient, emit a look, otherwise do nothing.
			qDebug() << "HLooker::stopLooker() - m_bLookAwayStarted = true";
			if (cumulativeLookTimeMS >= m_minLookTimeMS)
			{
				//HLook l(*m_pdirectionLookStarted, lookStartTimeMS, lastLookAwayStartTimeMS, cumulativeLookTimeMS);
				HLook l;
				l.setDirection(*m_pdirectionLookStarted);
				l.setStartMS(lookStartTimeMS);
				l.setEndMS(lastLookAwayStartTimeMS);
				l.setSublooks(sublooks);
				if (m_bInclusiveLookTime)
				{
					l.setLookMS(lastLookAwayStartTimeMS - lookStartTimeMS);
				}
				else
				{
					l.setLookMS(cumulativeLookTimeMS);
				}
				m_looks.append(l);

				qDebug() << "emit look(3): " << l;
				qDebug() << "sublooks follow" << endl << sublooks;
				emit look(l);
				m_bLookStarted = false;
				m_iLookStartedIndex = -1;
			}
		}
		else
		{
			// Stuff a look into the sublooks
			sublooks.append(HLook(*m_pdirectionLookStarted, lastLookStartTimeMS, tMS));

			// we were in a looking state, so add in the time from tMS to the lastLookStartTime as additional look time and test.
			cumulativeLookTimeMS += tMS - lastLookStartTimeMS;
			if (cumulativeLookTimeMS >= m_minLookTimeMS)
			{
				HLook l;
				l.setDirection(*m_pdirectionLookStarted);
				l.setStartMS(lookStartTimeMS);
				l.setEndMS(tMS);
				l.setSublooks(sublooks);
				if (m_bInclusiveLookTime)
				{
					l.setLookMS(tMS - lookStartTimeMS);
				}
				else
				{
					l.setLookMS(cumulativeLookTimeMS);
				}
				m_looks.append(l);
				qDebug() << "emit look(4): " << l;
				qDebug() << "sublooks follow" << endl << sublooks;
				emit look(l);
				m_bLookStarted = false;
				m_iLookStartedIndex = -1;
			}
			else
			{
				// emit lookAborted for each sublook.
				HLook l;
				foreach (l, sublooks) {
					emit lookAborted(l);
				}
				m_bLookStarted = false;
				m_iLookStartedIndex = -1;
			}
		}
	}
	qDebug() << "HLooker::stopLooker(): calling stop()";
	stop();
	qDebug() << "HLooker::stopLooker(): calling QCoreApplication::processEvents(0)";
	QCoreApplication::processEvents(0);
	qDebug() << "HLooker::stopLooker(): calling QCoreApplication::processEvents(0) - done";
}
