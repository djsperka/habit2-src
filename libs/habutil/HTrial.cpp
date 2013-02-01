/*
 *  HTrial.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 6/22/12.
 *  Copyright 2012 UC Davis Infant Cognition Lab. All rights reserved.
 *
 */

#include "HTrial.h"
#include "HElapsedTimer.h"
#include "HOutputGenerator.h"
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
	HOutputGenerator::instance()->changeTrial(m_nextStimulusSettings.getStimulusType());
	HOutputGenerator::instance()->setStimulusSettings(m_nextStimulusSettings);
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
	HState* m_sInitial = new HInitialState(HTrialLogItem::NEW_TRIAL, this);
	setInitialState(m_sInitial);
	
	// This is an initial state for repeated trials
	HState* m_sBailInitial = new HInitialState(HTrialLogItem::NEW_TRIAL_REPEAT, this);

	// AG states
	m_sAGRequest = new HAGRequestState(this);
	m_sAGRunning = new HAGRunningState(this);

	// Stim request state and StimRunning states are similar to AGRequest and AGRunning. 
	m_sStimRequest = new HStimRequestState(this);
	HNoLookTransition* pNoLookTrans = new HNoLookTransition(m_ptimerMaxNoLookTime->timerId());
	m_sStimRunning = new HStimRunningState(maxTrialLengthMS, pNoLookTrans, m_ptimerMaxTrialLength, maxNoLookTimeMS, m_ptimerMaxNoLookTime, this);

	// Create final and bail state. Bail state is for timed-out trials, and leads to repeating the same trial.
	HState* m_sBail = new HBailState(this);
	QFinalState* sFinal = new QFinalState(this);
		
	// Set connection for AGRequest state, which emits playAG() in its 'onEntry' method. 
	// That goes to the media manager, which starts the playhing of the AG.
	QObject::connect(m_sAGRequest, SIGNAL(playAG()), pMediaManager, SLOT(ag()));

	// Once that happens the media manager starts playing the ag, it emits
	// agStarted(). We use that signal as a transition to the AGRunning state. 
	m_sAGRequest->addTransition(pMediaManager, SIGNAL(agStarted()), m_sAGRunning);
	
	// When AGRunning state is entered, it tells the look detector to start looking for 
	// attention. It will emit an attention() signal when that happens (see below). 
	QObject::connect(m_sAGRunning, SIGNAL(entered()), this, SLOT(onAGRunningEntered()));
	QObject::connect(m_sAGRunning, SIGNAL(exited()), this, SLOT(onAGRunningExited()));

	// When the look detector emits the attention() signal enter StimRequest state. 
	m_sAGRunning->addTransition(pLD, SIGNAL(attention()), m_sStimRequest);
		
	// StimRequest emits a playStim() signal, which is connected to the media manager. 
	// When media manager starts playing the stim it emits stimStarted(). That signal 
	// is used to exit to StimRunning.
 	QObject::connect(m_sStimRequest, SIGNAL(playStim(int)), pMediaManager, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()
	QObject::connect(m_sStimRunning, SIGNAL(entered()), this, SLOT(onStimRunningEntered()));
	QObject::connect(m_sStimRunning, SIGNAL(exited()), this, SLOT(onStimRunningExited()));
	m_sStimRequest->addTransition(pMediaManager, SIGNAL(stimStarted(int)), m_sStimRunning);		// on entry, emits playStim()

	// Transition from sStimRunning depends on what the trial type is.
	// Fixed length trials end on a timeout signal (and we don't bother with look signals).
	// Subject-controlled trials (bFixedLength==false) will end on a look signal, or they can be
	// aborted on a timeout signal. 
	if (bFixedLength)
	{			
		m_sStimRunning->addTransition(m_ptimerMaxTrialLength, SIGNAL(timeout()), sFinal);
	}
	else
	{
		m_sStimRunning->addTransition(m_ptimerMaxTrialLength, SIGNAL(timeout()), m_sBail);
		m_sStimRunning->addTransition(pLD, SIGNAL(look(HLook)), sFinal);
	}

	
	if (bUseAG)
	{
		m_sInitial->addTransition(m_sAGRequest);
		m_sBailInitial->addTransition(m_sAGRequest);
	}
	else 
	{
		m_sInitial->addTransition(m_sStimRequest);
		m_sBailInitial->addTransition(m_sStimRequest);
	}
	m_sBail->addTransition(m_sBailInitial);					// bailed trial automatically repeats		
}

void HTrial::setNextStim(int i, const Habit::StimulusSettings& ss)	
{ 
	m_sStimRequest->setNextStim(i, ss); 
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
