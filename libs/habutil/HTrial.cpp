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
#include "HElapsedTimer.h"
#include <QFinalState>
#include <QTimerEvent>

void HStimRequestState::setNextStim(int i, const Habit::StimulusSettings& ss) 
{ 
	m_nextStimID = i; 
	m_nextStimulusSettings = ss;
};

void HStimRequestState::onEntry(QEvent* e) 
{
	Q_UNUSED(e);
	HState::onEntry(e);
	if (m_nextStimID < 0)
	{
		qWarning("HStimRequestState::onEntry : must call setNextStim with valid stimID");
	}
	emit playStim(m_nextStimID);
	
	// TODO emit TrialStarted(m_nextStimulusSettings)
};

void HAGRequestState::onEntry(QEvent* e) 
{
	Q_UNUSED(e);
	HState::onEntry(e);
	emit playAG();
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
}

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

HTrial::HTrial(HPhase& phase, HEventLog& log, QObject* pMediaManager, HLookDetector* pLD, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG) 
	: HPhaseChildState(phase, log, "HTrial")
	, m_pLD(pLD)
	, m_maxTrialLengthMS(maxTrialLengthMS)
	, m_maxNoLookTimeMS(maxNoLookTimeMS)
	, m_bFixedLength(bFixedLength)
	, m_bAG(bUseAG)
{
	// create timer for stim
	m_ptimerMaxTrialLength = new QTimer();
	m_ptimerMaxTrialLength->setSingleShot(true);

	m_ptimerMaxNoLookTime = new QTimer();
	m_ptimerMaxNoLookTime->setSingleShot(true);
	
	// Create initial state. Do not define its transition yet. 
	HTrialInitialState* sInitial = new HTrialInitialState(*this, log, this);
	setInitialState(sInitial);
	
	// This is an initial state for repeated trials
	HTrialBailInitialState* sBailInitial = new HTrialBailInitialState(*this, log, this);

	// AG states
	HAGRequestState* sAGRequest = new HAGRequestState(*this, log, this);
	HAGRunningState* sAGRunning = new HAGRunningState(*this, log, this);

	// Stim request state and StimRunning states are similar to AGRequest and AGRunning. 
	HStimRequestState* sStimRequest = new HStimRequestState(*this, log, this);
	HNoLookTransition* pNoLookTrans = new HNoLookTransition(m_ptimerMaxNoLookTime->timerId());
	HStimRunningState* sStimRunning = new HStimRunningState(*this, log, maxTrialLengthMS, pNoLookTrans, m_ptimerMaxTrialLength, maxNoLookTimeMS, m_ptimerMaxNoLookTime, this);

	// Create final and bail state. Bail state is for timed-out trials, and leads to repeating the same trial.
	HState* sBail = new HBailState(*this, log, this);
	QFinalState* sFinal = new QFinalState(this);
		
	// Set connection for AGRequest state, which emits playAG() in its 'onEntry' method. 
	// That goes to the media manager, which starts the playhing of the AG.
	QObject::connect(sAGRequest, SIGNAL(playAG()), pMediaManager, SLOT(ag()));

	// Once that happens the media manager starts playing the ag, it emits
	// agStarted(). We use that signal as a transition to the AGRunning state. 
	sAGRequest->addTransition(pMediaManager, SIGNAL(agStarted()), sAGRunning);
	
	// When AGRunning state is entered, it tells the look detector to start looking for 
	// attention. It will emit an attention() signal when that happens (see below). 
	QObject::connect(sAGRunning, SIGNAL(entered()), this, SLOT(onAGRunningEntered()));
	QObject::connect(sAGRunning, SIGNAL(exited()), this, SLOT(onAGRunningExited()));

	// When the look detector emits the attention() signal enter StimRequest state. 
	sAGRunning->addTransition(pLD, SIGNAL(attention()), sStimRequest);
		
	// StimRequest emits a playStim() signal, which is connected to the media manager. 
	// When media manager starts playing the stim it emits stimStarted(). That signal 
	// is used to exit to StimRunning.
 	QObject::connect(sStimRequest, SIGNAL(playStim(int)), pMediaManager, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()
	QObject::connect(sStimRunning, SIGNAL(entered()), this, SLOT(onStimRunningEntered()));
	QObject::connect(sStimRunning, SIGNAL(exited()), this, SLOT(onStimRunningExited()));
	sStimRequest->addTransition(pMediaManager, SIGNAL(stimStarted(int)), sStimRunning);		// on entry, emits playStim()

	// Transition from sStimRunning depends on what the trial type is.
	// Fixed length trials end on a timeout signal (and we don't bother with look signals).
	// Subject-controlled trials (bFixedLength==false) will end on a look signal, or they can be
	// aborted on a timeout signal. 
	if (bFixedLength)
	{			
		sStimRunning->addTransition(m_ptimerMaxTrialLength, SIGNAL(timeout()), sFinal);
	}
	else
	{
		sStimRunning->addTransition(m_ptimerMaxTrialLength, SIGNAL(timeout()), sBail);
		sStimRunning->addTransition(pLD, SIGNAL(look(HLook)), sFinal);
	}

	
	if (bUseAG)
	{
		sInitial->addTransition(sAGRequest);
		sBailInitial->addTransition(sAGRequest);
	}
	else 
	{
		sInitial->addTransition(sStimRequest);
		sBailInitial->addTransition(sStimRequest);
	}
	sBail->addTransition(sBailInitial);					// bailed trial automatically repeats		
}

void HTrial::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	eventLog().append(new HTrialStartEvent(

void HTrial::setNextStim(int i, const Habit::StimulusSettings& ss)	
{ 
//FIX THIS	m_sStimRequest->setNextStim(i, ss); 
}

void HTrial::onStimRunningEntered()
{
	m_pLD->enableLook();
}

void HTrial::onStimRunningExited()
{
	m_pLD->disable();
}

void HTrial::onAGRunningEntered()
{
	m_pLD->enableAGLook();
}

void HTrial::onAGRunningExited()
{
	m_pLD->disable();
}
