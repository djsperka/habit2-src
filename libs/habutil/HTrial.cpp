/*
 *  HTrial.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 6/22/12.
 *  Copyright 2012 UC Davis Infant Cognition Lab. All rights reserved.
 *
 */

#include "HTrial.h"
#include <QFinalState>

void HStimRequestState::setNextStim(int i) 
{ 
	m_nextStimID = i; 
};

void HStimRequestState::onEntry(QEvent* e) 
{
	Q_UNUSED(e);
	HState::onEntry(e);
	if (m_nextStimID < 0)
	{
		qWarning("HStimRequestState::onEntry : must call setNextStim with valid stimID");
	}
	qDebug() << "HStimRequestState::onEntry : emit playStim(" << m_nextStimID << ")";
	emit playStim(m_nextStimID);
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
	m_ptimer->start(m_ms);
};


HTrial::HTrial(QObject* pMediaManager, HLookDetector* pLD, int maxTrialLengthMS, bool bFixedLength, bool bUseAG, HState* parent=0) :
	HState("HTrial", parent), 
	m_pLD(pLD),
	m_maxTrialLengthMS(maxTrialLengthMS), 
	m_bFixedLength(bFixedLength), 
	m_bAG(bUseAG)
{
	// create timer for stim
	m_ptimer = new QTimer();
	m_ptimer->setSingleShot(true);
	
	
	// Create initial state. Do not define its transition yet. 
	HState* sInitial = new HState("sInitial", this);
	setInitialState(sInitial);

	// AG states
	HAGRequestState* sAGRequest = new HAGRequestState(this);
	HAGRunningState* sAGRunning = new HAGRunningState(this);

	// Stim request state and StimRunning states are similar to AGRequest and AGRunning. 
	m_sStimRequest = new HStimRequestState(this);
	HState* sStimRunning = new HStimRunningState(maxTrialLengthMS, m_ptimer, this);

	// Create final and bail state. Bail state is for timed-out trials, and leads to repeating the same trial.
	HState* sBail = new HState("stateBail", this);
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
	sAGRunning->addTransition(pLD, SIGNAL(attention()), m_sStimRequest);
		
	// StimRequest emits a playStim() signal, which is connected to the media manager. 
	// When media manager starts playing the stim it emits stimStarted(). That signal 
	// is used to exit to StimRunning.
 	QObject::connect(m_sStimRequest, SIGNAL(playStim(int)), pMediaManager, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()
	QObject::connect(sStimRunning, SIGNAL(entered()), this, SLOT(onStimRunningEntered()));
	QObject::connect(sStimRunning, SIGNAL(exited()), this, SLOT(onStimRunningExited()));
	m_sStimRequest->addTransition(pMediaManager, SIGNAL(stimStarted()), sStimRunning);		// on entry, emits playStim()

	// Transition from sStimRunning depends on what the trial type is.
	// Fixed length trials end on a timeout signal (and we don't bother with look signals).
	// Subject-controlled trials (bFixedLength==false) will end on a look signal, or they can be
	// aborted on a timeout signal. 
	if (bFixedLength)
	{			
		sStimRunning->addTransition(m_ptimer, SIGNAL(timeout()), sFinal);
	}
	else
	{
		sStimRunning->addTransition(m_ptimer, SIGNAL(timeout()), sBail);
		sStimRunning->addTransition(pLD, SIGNAL(look(HLook)), sFinal);
	}
	
	
	if (bUseAG)
	{
		sInitial->addTransition(sAGRequest);
	}
	else 
	{
		sInitial->addTransition(m_sStimRequest);
	}
	sBail->addTransition(sInitial);															// bailed trial automatically repeats
	
	
}

void HTrial::setNextStim(int i)	
{ 
	m_sStimRequest->setNextStim(i); 
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
