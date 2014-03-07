/*
 *  HTrialChildState.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 3/15/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */



#include "HTrialChildState.h"
#include "HTrial.h"
#include "HElapsedTimer.h"

void HTrialInitialState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	
	// post TrialStart event to event log
	eventLog().append(new HTrialStartEvent(trial().getTrialNumber(), trial().getRepeatNumber(), HElapsedTimer::elapsed()));										   
}



HStimRunningState::HStimRunningState(HTrial& trial, HEventLog& log, const Habit::HPhaseSettings& phaseSettings)
: HTrialChildState(trial, log, "HStimRunning")
, m_phaseSettings(phaseSettings)
{

	// When "max no look time" is used, it is implemented here. This class will start a timer
	// on entry, set to go off at the max no look time. The noLookTimeout() slot will post a
	// QEvent to the state machine that will trigger a transition to HNoLookTimeout state.

	if (m_phaseSettings.getIsMaxNoLookTime())
	{
		m_ptimerNoLook = new QTimer();
		m_ptimerNoLook->setSingleShot(true);
		connect(m_ptimerNoLook, SIGNAL(timeout()), this, SLOT(noLookTimeout()));
	}
};

void HStimRunningState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	if (m_phaseSettings.getIsMaxNoLookTime())
	{
		if (m_ptimerNoLook->isActive()) m_ptimerNoLook->stop();
		m_ptimerNoLook->start(m_phaseSettings.getMaxNoLookTime());
	}
	m_bGotLook = false;
	m_bGotLookStarted = false;
	m_bGotLookAwayStarted = false;
};



void HStimRunningState::gotLookStarted()
{
	m_bGotLookStarted = true;
}

void HStimRunningState::gotLookAwayStarted()
{
	if (m_bGotLook) return;
	else if (m_phaseSettings.getIsMaxNoLookTime() && !m_ptimerNoLook->isActive())
	{
		machine()->postEvent(new HNoLookQEvent());
	}
	else
	{
		m_bGotLookAwayStarted = true;
		m_bGotLookStarted = false;
	}
}

void HStimRunningState::noLookTimeout()
{
	qDebug() << "HStimRunningState::noLookTimeout() m_bGotLook " << m_bGotLook << " m_bGotLookStarted " << m_bGotLookStarted;
	if (m_bGotLook || m_bGotLookStarted) return;
	else if (!m_bGotLookStarted)
	{
		machine()->postEvent(new HNoLookQEvent());
	}
}

void HStimRunningState::onExit(QEvent* e)
{
	Q_UNUSED(e);
	HState::onExit(e);
	if (m_phaseSettings.getIsMaxNoLookTime() && m_ptimerNoLook->isActive())
	{
		m_ptimerNoLook->stop();
	}
};

void HStimRequestState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	trial().phase().requestCurrentStim();		// phase directly calls mm play() function. mm emits *Started() signals, caught in transition from this state.
};

void HAGRequestState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	trial().phase().requestAG();
};

void HAGRunningState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
};

void HGotLookState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, HElapsedTimer::elapsed()));
};

void HMaxAccumulatedLookTimeState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndMaxAccumulatedLookTime, HElapsedTimer::elapsed()));
};

void HMaxLookAwayTimeState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndMaxLookAwayTime, HElapsedTimer::elapsed()));
};


void HNoLookTimeoutState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	trial().incrementRepeatNumber();
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndNoLookTimeout, HElapsedTimer::elapsed()));
};

void HMaxStimulusTimeState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndMaxStimulusTime, HElapsedTimer::elapsed()));
};


