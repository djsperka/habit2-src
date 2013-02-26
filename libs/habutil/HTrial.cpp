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
#include "HElapsedTimer.h"
#include "HMediaManager.h"
#include <QFinalState>
#include <QTimerEvent>

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
	m_ptransNoLook->reset();
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
	eventLog().append(new HTrialEndEvent(kTrialEndGotLook, HElapsedTimer::elapsed()));
};

void HNoLookTimeoutState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	trial().incrementRepeatNumber();
	eventLog().append(new HTrialEndEvent(kTrialEndNoLookTimeout, HElapsedTimer::elapsed()));
};

void HFixedTimeoutState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HState::onEntry(e);
	eventLog().append(new HTrialEndEvent(kTrialEndFixedTimeout, HElapsedTimer::elapsed()));
};
	
					  
					  
bool HNoLookTransition::eventTest(QEvent* e)
{
	bool bVal = false;
	if (!m_bGotLook && (e->type() == QEvent::Type(QEvent::Timer)))
	{
		QTimerEvent* te = static_cast<QTimerEvent*>(e);
		bVal = (te->timerId() == m_timerId);
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
	HNoLookTransition* pNoLookTrans = new HNoLookTransition(m_ptimerMaxNoLookTime->timerId());
	HStimRunningState* sStimRunning = new HStimRunningState(*this, log, maxTrialLengthMS, pNoLookTrans, m_ptimerMaxTrialLength, maxNoLookTimeMS, m_ptimerMaxNoLookTime);

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
		sStimRunning->addTransition(m_ptimerMaxTrialLength, SIGNAL(timeout()), sNoLookTimeout);
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
	
	// Post to log - console-type debug log, not event log 
	HState::onEntry(e);

	// post to event log
	eventLog().append(new HTrialStartEvent());										   
}


void HTrial::onStimRunningEntered()
{
	phase().experiment().getLookDetector().enableLook();
}

void HTrial::onStimRunningExited()
{
	phase().experiment().getLookDetector().disable();
}

void HTrial::onAGRunningEntered()
{
	phase().experiment().getLookDetector().enableAGLook();
}

void HTrial::onAGRunningExited()
{
	phase().experiment().getLookDetector().disable();
}
