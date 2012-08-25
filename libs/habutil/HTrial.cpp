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


HTrial::HTrial(QObject* pDialog, QObject* pMediaPlayer, HLookDetector* pLD, int maxTrialLengthMS, bool bFixedLength, bool bUseAG) :
	HState("HTrial"), 
	m_pdialog(pDialog),
	m_pLD(pLD),
	m_maxTrialLengthMS(maxTrialLengthMS), 
	m_bFixedLength(bFixedLength), 
	m_bAG(bUseAG)
{
	// create timer for stim
	m_ptimer = new QTimer();
	m_ptimer->setSingleShot(true);
	
	// Create all stim states and transitions
	m_sStimRequest = new HStimRequestState(this);
	HState* sStimRunning = new HStimRunningState(maxTrialLengthMS, m_ptimer, this);
	HState* sBail = new HState("stateBail", this);
	QFinalState* sFinal = new QFinalState(this);

	m_sStimRequest->addTransition(pMediaPlayer, SIGNAL(stimStarted()), sStimRunning);		// on entry, emits playStim()
 	QObject::connect(m_sStimRequest, SIGNAL(playStim(int)), pMediaPlayer, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()
	QObject::connect(m_sStimRequest, SIGNAL(entered()), this, SLOT(onStimRunningEntered()));
	
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
	
	
	// initial state will transition to sAG when attention getter is used, otherwise
	// to sStimRequest.
	HState* sInitial = new HState("sInitial", this);
	setInitialState(sInitial);
	if (bUseAG)
	{
		HAGState* sAG = new HAGState(this);
		HAGRunningState* sAGRunning = new HAGRunningState(this);
		//HState* sAGRunning = new HState("stateAGRunning", this);
		sInitial->addTransition(sAG);
		sAG->addTransition(pMediaPlayer, SIGNAL(agStarted()), sAGRunning);					// exit this state when ag has started signal that stim has started
		sAGRunning->addTransition(pLD, SIGNAL(attention()), m_sStimRequest);				// ready transition looks for <Enter> key
		QObject::connect(sAG, SIGNAL(playAG()), pMediaPlayer, SLOT(ag()));					// media player will receive this signal and emit stimStarted()
		QObject::connect(sAG, SIGNAL(entered()), this, SLOT(onAGRunningEntered()));
	}
	else 
	{
		sInitial->addTransition(m_sStimRequest);
	}
	sBail->addTransition(sInitial);															// bailed trial automatically repeats
	
	
}


void HTrial::onStimRunningEntered()
{
	m_pLD->enableLook();
}

void HTrial::onAGRunningEntered()
{
	m_pLD->enableAGLook();
}
