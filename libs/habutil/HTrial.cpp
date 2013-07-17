/*
 *  HTrial.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 6/22/12.
 *  Copyright 2012 UC Davis Infant Cognition Lab. All rights reserved.
 *
 */

#include "HTrial.h"
#include "HTrialChildState.h"
#include "HPhase.h"
#include "HExperiment.h"
#include "HEvents.h"
#include "HElapsedTimer.h"
#include "HMediaManager.h"
#include <QFinalState>
#include <QTimerEvent>
#include <QtDebug>

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

void HStimRunningState::onEntry(QEvent* e) 
{
	Q_UNUSED(e);
	HState::onEntry(e);
	if (m_ptimerMax->isActive()) m_ptimerMax->stop();
	if (m_ptimerNoLook->isActive()) m_ptimerNoLook->stop();
	m_ptimerMax->start(m_msMax);
	m_ptimerNoLook->start(m_msNoLook);
};

void HStimRunningState::onExit(QEvent* e)
{
	Q_UNUSED(e);
	HState::onExit(e);
};

void HGotLookState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndGotLook, HElapsedTimer::elapsed()));
};

void HNoLookTimeoutState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	trial().incrementRepeatNumber();
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndNoLookTimeout, HElapsedTimer::elapsed()));
};

void HFixedTimeoutState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	eventLog().append(new HTrialEndEvent(HTrialEndType::HTrialEndFixedTimeout, HElapsedTimer::elapsed()));
};
	
					  
					  
bool HNoLookTransition::eventTest(QEvent* e)
{
	bool bVal = false;

	// Make sure this is the correct signal. If not get out and block transition.
    if (!QSignalTransition::eventTest(e))
        return false;

	// OK so our timer event has fired.
	// If there is a look pending, we don't want the transition to fire (return false).
    // I assume that the look() signal WILL come, and it will trigger a transition.
	// If there is no look pending, let this transition happen (return true).

	if (m_ld.isLookPending(HElapsedTimer::elapsed()))
	{
		bVal = false;
	}
	else
	{
		bVal = true;
	}

	return bVal;
}


HTrial::HTrial(HPhase& phase, HEventLog& log, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG) 
	: HPhaseChildState(phase, log, "HTrial")
	, m_maxTrialLengthMS(maxTrialLengthMS)
	, m_maxNoLookTimeMS(maxNoLookTimeMS)
	, m_bFixedLength(bFixedLength)
	, m_bAG(bUseAG)
	, m_trialNumber(0)
	, m_repeatNumber(0)
{
	// create timer for stim
	m_ptimerMaxTrialLength = new QTimer();
	m_ptimerMaxTrialLength->setSingleShot(true);
	m_ptimerMaxNoLookTime = new QTimer();
	m_ptimerMaxNoLookTime->setSingleShot(true);
	
	// Create initial state. Do not define its transition yet. 
	HTrialInitialState* sInitial = new HTrialInitialState(*this, log);
	setInitialState(sInitial);
	
	// AG states
	HAGRequestState* sAGRequest = new HAGRequestState(*this, log);
	HAGRunningState* sAGRunning = new HAGRunningState(*this, log);

	// Stim request state and StimRunning states are similar to AGRequest and AGRunning. 
	HStimRequestState* sStimRequest = new HStimRequestState(*this, log);
	HStimRunningState* sStimRunning = new HStimRunningState(*this, log, maxTrialLengthMS, m_ptimerMaxTrialLength, maxNoLookTimeMS, m_ptimerMaxNoLookTime);

	// Create final state.
	QFinalState* sFinal = new QFinalState(this);
		
	// Set connection for AGRequest state, which emits playAG() in its 'onEntry' method. 
	// That goes to the media manager, which starts the playhing of the AG.

	// 2/14/2013 stim request and ag request will no longer emit playStim() - instead will call phase().requestCurentStim()
	//QObject::connect(sAGRequest, SIGNAL(playAG()), &phase().getMediaManager(), SLOT(ag()));

	// Once that happens the media manager starts playing the ag, it emits
	// agStarted(). We use that signal as a transition to the AGRunning state. 
	sAGRequest->addTransition(&phase.experiment().getMediaManager(), SIGNAL(agStarted()), sAGRunning);
	
	// When AGRunning state is entered, it tells the look detector to start looking for 
	// attention. It will emit an attention() signal when that happens (see below). 
	QObject::connect(sAGRunning, SIGNAL(entered()), this, SLOT(onAGRunningEntered()));
	QObject::connect(sAGRunning, SIGNAL(exited()), this, SLOT(onAGRunningExited()));

	// When the look detector emits the attention() signal enter StimRequest state. 
	sAGRunning->addTransition(&phase.experiment().getLookDetector(), SIGNAL(attention()), sStimRequest);
		
	// StimRequest emits a playStim() signal, which is connected to the media manager. 
	// When media manager starts playing the stim it emits stimStarted(). That signal 
	// is used to exit to StimRunning.
	
	// 2/14/2013 stim request and ag request will no longer emit playStim() - instead will call phase().requestCurentStim()
 	//QObject::connect(sStimRequest, SIGNAL(playStim(int)), &phase().getMediaManager(), SLOT(stim(int)));// media player will receive this signal and emit stimStarted()
	QObject::connect(sStimRunning, SIGNAL(entered()), this, SLOT(onStimRunningEntered()));
	QObject::connect(sStimRunning, SIGNAL(exited()), this, SLOT(onStimRunningExited()));
	sStimRequest->addTransition(&phase.experiment().getMediaManager(), SIGNAL(stimStarted(int)), sStimRunning);		// on entry, emits playStim()

	// Transition from sStimRunning depends on what the trial type is.
	// Fixed length trials end on a timeout signal (and we don't bother with look signals).
	// Subject-controlled trials (bFixedLength==false) will end on a look signal, or they can be
	// aborted on a timeout signal. 
	if (bFixedLength)
	{			
		HFixedTimeoutState* sFixedTimeout = new HFixedTimeoutState(*this, log);
		sStimRunning->addTransition(m_ptimerMaxTrialLength, SIGNAL(timeout()), sFixedTimeout);
		sFixedTimeout->addTransition(sFinal);
	}
	else
	{
		HGotLookState* sGotLook = new HGotLookState(*this, log);
		HNoLookTimeoutState* sNoLookTimeout = new HNoLookTimeoutState(*this, log);

		// the No Look timeout is tricky. If a look is pending, we want that look to count
		// if the timeout hits. The timeout is supposed to indicate that we've waited long
		// enough, dammit, for a look and we're quitting the trial. But if a look is in
		// progress, we will BLOCK this transition and force a look() signal.

		HNoLookTransition* pNoLookTrans = new HNoLookTransition(phase.experiment().getLookDetector(), m_ptimerMaxNoLookTime);
		pNoLookTrans->setTargetState(sNoLookTimeout);
		sStimRunning->addTransition(pNoLookTrans);

		//sStimRunning->addTransition(m_ptimerMaxTrialLength, SIGNAL(timeout()), sNoLookTimeout);
		sNoLookTimeout->addTransition(sInitial);	// trial is repeated
		sStimRunning->addTransition(&phase.experiment().getLookDetector(), SIGNAL(look(HLook)), sGotLook);
		sGotLook->addTransition(sFinal);
	}

	
	if (bUseAG)
	{
		sInitial->addTransition(sAGRequest);
	}
	else 
	{
		sInitial->addTransition(sStimRequest);
	}
}


void HTrial::setTrialNumber(int i)
{
	m_trialNumber = i;
	m_repeatNumber = 0;
}

void HTrial::incrementRepeatNumber()
{
	m_repeatNumber++;
}

void HTrial::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
}


void HTrial::onStimRunningEntered()
{
	phase().experiment().getLookDetector().enableLook();
	eventLog().append(new HLookEnabledEvent(HElapsedTimer::elapsed()));
}

void HTrial::onStimRunningExited()
{
	phase().experiment().getLookDetector().disable();
	eventLog().append(new HLookDisabledEvent(HElapsedTimer::elapsed()));
}

void HTrial::onAGRunningEntered()
{
	phase().experiment().getLookDetector().enableAGLook();
	eventLog().append(new HAGLookEnabledEvent(HElapsedTimer::elapsed()));
}

void HTrial::onAGRunningExited()
{
	phase().experiment().getLookDetector().disable();
	eventLog().append(new HLookDisabledEvent(HElapsedTimer::elapsed()));
}
