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

HLooker::HLooker(int minlooktime_ms, int minlookawaytime_ms, HEventLog& log, bool bLive)
: QStateMachine()
, m_minLookTimeMS(minlooktime_ms)
, m_minLookAwayTimeMS(minlookawaytime_ms)
, m_log(log)
, m_bLive(bLive)
, m_bLookStarted(false)
, m_pdirectionPendingLook(&HLookDirection::LookAway)
, m_bLookAwayStarted(false)
{
	m_ptimerLookAwayTimeout = new QTimer();
	m_ptimerLookAwayTimeout->setInterval(minlookawaytime_ms);
	m_ptimerLookAwayTimeout->setSingleShot(true);
	QObject::connect(m_ptimerLookAwayTimeout, SIGNAL(timeout()), this, SLOT(minLookAwayTimeout()));

	// Create states and transitions for state machine.
#if 0
	HLookerInitialState* sInitial = new HLookerInitialState(this, NULL);
	HLookingState* sLooking = new HLookingState(this, NULL);
	HLookingAwayState* sLookingAway = new HLookingAwayState(this, NULL);
#endif
	QState* sInitial = new QState();
	QState* sLooking = new QState();
	QState* sLookingAway = new QState();

	HTransToLookTransition* pTransToLook;
	HTransToLookAwayTransition* pTransToLookAway;

	pTransToLook = new HTransToLookTransition();
	pTransToLook->setTargetState(sLooking);
	sInitial->addTransition(pTransToLook);
	addState(sInitial);

	pTransToLook = new HTransToLookTransition();
	pTransToLook->setTargetState(sLooking);
	sLookingAway->addTransition(pTransToLook);
	addState(sLookingAway);

	pTransToLookAway = new HTransToLookAwayTransition();
	pTransToLookAway->setTargetState(sLookingAway);
	sLooking->addTransition(pTransToLookAway);
	addState(sLooking);

	setInitialState(sInitial);

	connect(sInitial, SIGNAL(entered()), this, SLOT(onInitialStateEntered()));
	connect(sLookingAway, SIGNAL(entered()), this, SLOT(onLookingAwayStateEntered()));
	connect(sLooking, SIGNAL(entered()), this, SLOT(onLookingStateEntered()));

};

void HLooker::addTrans(const HLookTrans& type, int tMS)
{
	qDebug() << "addTrans " << type << " @ " << tMS;
	m_log.append(new HLookTransEvent(type, tMS));
	m_transitions.append(qMakePair(&type, tMS));
	// Create a new event for the state machine to digest.
	if (type.isTransToLook())
		postEvent(new HTransToLookQEvent(type, tMS));
	else
		postEvent(new HTransToLookAwayQEvent(type, tMS));
	return;
}

void HLooker::onInitialStateEntered()
{
	m_bLookStarted = m_bLookAwayStarted = false;
	m_pdirectionPendingLook = &HLookDirection::UnknownLookDirection;
	m_lookStartTimeMS = m_lookLastStartTimeMS = m_cumulativeLookTimeMS = 0;
	m_lookAwayStartTimeMS = m_cumulativeLookAwayTimeMS = 0;
}

void HLooker::onLookingStateEntered()
{
	const QPair<const HLookTrans*, int>& p = m_transitions.last();

	qDebug() << "onLookingStateEntered";

	// Stop timer if it is active
	if (m_ptimerLookAwayTimeout->isActive())
	{
		qDebug() << "Stop timer...";
		m_ptimerLookAwayTimeout->stop();
	}

	if (m_bLookStarted)
	{
		// A look has started. Since we are entering looking state then the only possibility is
		// that a look-away had also started and now the subject is looking again.
		if (m_bLookAwayStarted)
		{

			// check if the time spent looking away is longer than the min look-away time,
			// OR if the looking away time was short, but the the look back is to a different screen.
			if ((p.second - m_lookAwayStartTimeMS) >= m_minLookAwayTimeMS ||
				((p.second - m_lookAwayStartTimeMS) < m_minLookAwayTimeMS && *m_pdirectionPendingLook != directionTo(*p.first)))
			{
				// Any accumulated looking time should be flushed.
				// See if there was a complete look and emit look() if necessary. Then emit a look-away().
				// Finally, start a new look.
				if (m_cumulativeLookTimeMS > m_minLookTimeMS)
				{
					HLook l(*m_pdirectionPendingLook, m_lookStartTimeMS, m_lookAwayStartTimeMS, m_cumulativeLookTimeMS);
					m_looks.append(l);
					emit look(l);
				}

				// emit lookAway()
				HLook l (HLookDirection::LookAway, m_lookAwayStartTimeMS, p.second);
				m_looks.append(l);
				emit lookAway(l);

				// Starting a new potential look
				m_bLookStarted = true;
				m_lookStartTimeMS = p.second;
				m_lookLastStartTimeMS = p.second;
				qDebug() << "clear cumulative look time";
				m_cumulativeLookTimeMS = 0;
				m_pdirectionPendingLook = &directionTo(*p.first);

				m_bLookAwayStarted = false;
				m_lookAwayStartTimeMS = 0;
				m_cumulativeLookAwayTimeMS = 0;
			}
			else if (*m_pdirectionPendingLook == directionTo(*p.first))
			{
				// looked back in same direction as before. resume previous look.
				// Add in cumulative look-away time but wipe out look-away info.
				m_bLookAwayStarted = false;
				m_cumulativeLookAwayTimeMS += p.second - m_lookAwayStartTimeMS;
				m_lookAwayStartTimeMS = 0;
				m_lookLastStartTimeMS = p.second;
			}
		}
		else
		{
			qWarning() << "onLookStateEntered(): lookStarted=true, lookAwayStarted=false!?!";
			m_lookLastStartTimeMS = p.second;
		}
	}
	else		// !m_bLookStarted
	{
		// Emit a look-away if necessary, then start a new look.
		if (!m_bLookAwayStarted)
		{
			// This only occurs on initial look transition.
			emit lookStarted();
		}
		else
		{
			m_cumulativeLookAwayTimeMS += p.second - m_lookAwayStartTimeMS;
			HLook l(HLookDirection::LookAway, m_lookAwayStartTimeMS, p.second, m_cumulativeLookAwayTimeMS);
			m_looks.append(l);
			emit lookAway(l);
		}
		// start fresh new look
		m_bLookStarted = true;
		m_bLookAwayStarted = false;
		m_pdirectionPendingLook = &directionTo(*p.first);
		m_lookStartTimeMS = p.second;
		m_lookLastStartTimeMS = p.second;
		m_cumulativeLookTimeMS = 0;
		qDebug() << "clear cumulative look time";
		m_lookAwayStartTimeMS = 0;
	}
	qDebug() << "onLookingStateEntered -- done.";
}

void HLooker::onLookingAwayStateEntered()
{
	const QPair<const HLookTrans*, int>& p = m_transitions.last();

	qDebug() << "onLookingAwayStateEntered";

	// If a look is started, tally the looking time and start the look-away timer
	if (m_bLookStarted)
	{
		m_cumulativeLookTimeMS += p.second - m_lookLastStartTimeMS;
		qDebug() << "increment cumulative look time = " << m_cumulativeLookTimeMS;
		m_ptimerLookAwayTimeout->start();
	}
	m_bLookAwayStarted = true;
	m_lookAwayStartTimeMS = p.second;
	m_cumulativeLookAwayTimeMS = 0;
	qDebug() << "onLookingAwayStateEntered -- done.";
}

void HLooker::onLookingAwayStateExited()
{
	if (m_ptimerLookAwayTimeout->isActive())
		m_ptimerLookAwayTimeout->stop();
}

void HLooker::minLookAwayTimeout()
{
	qDebug() << "minLookAwayTimeout()";
	// Was the look complete?
	if (m_bLookStarted && m_cumulativeLookTimeMS > m_minLookTimeMS)
	{
		HLook l(*m_pdirectionPendingLook, m_lookStartTimeMS, m_lookAwayStartTimeMS, m_cumulativeLookTimeMS);
		m_looks.append(l);
		emit look(l);
	}
	m_bLookStarted = false;
	m_lookStartTimeMS = 0;
	m_lookLastStartTimeMS = 0;
	m_cumulativeLookTimeMS = 0;
	qDebug() << "clear cumulative look time";
	emit lookAwayStarted();
}


void HLooker::stopLooker(int tMS)
{
	qDebug() << "stopLooker(" << tMS << ")";

	// the looker will be stopped, it will not enter a final state.
	// When it is stopped, flush out any look or lookaway pending.
	if (m_bLookStarted)
	{
		if (m_bLookAwayStarted && m_cumulativeLookTimeMS > m_minLookTimeMS)
		{
			HLook l(*m_pdirectionPendingLook, m_lookStartTimeMS, m_lookAwayStartTimeMS, m_cumulativeLookTimeMS);
			m_looks.append(l);
			emit look(l);
		}
		else
		{
			m_cumulativeLookTimeMS += tMS - m_lookLastStartTimeMS;
			qDebug() << "increment cumulative look time = " << m_cumulativeLookTimeMS;
			if (m_cumulativeLookTimeMS > m_minLookTimeMS)
			{
				HLook l(*m_pdirectionPendingLook, m_lookStartTimeMS, tMS, m_cumulativeLookTimeMS);
				m_looks.append(l);
				emit look(l);
			}
		}
	}
	else
	{
		HLook l(HLookDirection::LookAway, m_lookAwayStartTimeMS, tMS);
		m_looks.append(l);
		emit lookAway(l);
	}
	stop();
}

