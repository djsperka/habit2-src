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

	// buffer stimuli for this trial
	trial().phase().loadCurrentStimBuffers();

	// emit signal...
	emit trialStarted(trial().getTrialNumber(), trial().getRepeatNumber());
}



HStimRunningState::HStimRunningState(HTrial& trial, HEventLog& log, const Habit::HPhaseSettings& phaseSettings)
: HTrialChildState(trial, log, "HStimRunning")
, m_phaseSettings(phaseSettings)
, m_bGotLookStarted(false)
, m_bGotLookPending(false)
, m_bNeedPendingBeforeAbort(false)
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


/*
 * This is called before the state's entered() signal is emitted.
 * See HTrial::onStimRunningEntered(), which is connected to
 * HStimRunningState SIGNAL(entered()).
 */

void HStimRunningState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	if (m_phaseSettings.getIsMaxNoLookTime())
	{
		if (m_ptimerNoLook->isActive()) m_ptimerNoLook->stop();
		m_ptimerNoLook->start(m_phaseSettings.getMaxNoLookTime());
	}
	m_bGotLookStarted = false;
	m_bGotLookPending = false;
	m_bNeedPendingBeforeAbort = false;
};



void HStimRunningState::gotLookStarted()
{
	m_bGotLookStarted = true;
}

void HStimRunningState::gotLookPending()
{
	m_bGotLookPending = true;
}

void HStimRunningState::gotLookAborted(HLook)
{
	// Under certain conditions, this signal generates an HNoLookQEvent.
	m_bGotLookStarted = false;
	if (m_bNeedPendingBeforeAbort && !m_bGotLookPending)
	{
		machine()->postEvent(new HNoLookQEvent());
	}
}

void HStimRunningState::noLookTimeout()
{
	// The timer associated with the "Max No Look Time" has expired.
	// This timer will not have started if the "Max No Look Time" has not been set.
	// If this timeout has occurred, then either
	// 1) must have m_bGotLookPending == true, meaning a look has met the min look time criteria,
	//    though the look may not yet be complete. It WILL become a complete look eventually.
	// 2) if !m_bGotLookPending, then must have m_bLookStarted==true. If not, post a HNoLookQEvent()
	//    (which should trigger a transition from this state to a trial-ending state)
	//    If we DO HAVE m_bLookStarted==true, then we must receive SIGNAL(lookPending()) before we
	//    receive SIGNAL(lookAborted()). If SIGNAL(lookAborted()) is received first, then post a
	//    HNoLookQEvent(). If SIGNAL(lookPending()) is received first, do nothing -- the "Max No
	//    Look Time" requirement has been met.
	if (m_bGotLookPending) return;
	else if (!m_bGotLookStarted)
	{
		machine()->postEvent(new HNoLookQEvent());
	}
	else
	{
		m_bNeedPendingBeforeAbort = true;
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
	if (m_bRepeatTrialOnMaxLookAwayTime)
	{
		trial().incrementRepeatNumber();
		eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndMaxLookAwayTimeRepeat, HElapsedTimer::elapsed()));
	}
	else
	{
		eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndMaxLookAwayTime, HElapsedTimer::elapsed()));
	}
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

void HAGAbortedState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	trial().incrementRepeatNumber();
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndAGAbort, HElapsedTimer::elapsed()));
};

void HStimAbortedState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndStimAbort, HElapsedTimer::elapsed()));
};
