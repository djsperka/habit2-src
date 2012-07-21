/*
 *  HTrial.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 6/22/12.
 *  Copyright 2012 UC Davis Infant Cognition Lab. All rights reserved.
 *
 */

#include "HTrial.h"
#include "HStartTrialState.h"
#include "HAGState.h"
#include "HStimRequestState.h"
#include <QFinalState>
#include "HReadyTransition.h"


HTrial::HTrial(QObject* pDialog, QObject* pMediaPlayer, QObject*pLooker, int maxTrialLengthMS, bool bFixedLength, bool bUseAG) :
	HState("HTrial"), 
	m_maxTrialLengthMS(maxTrialLengthMS), 
	m_bFixedLength(bFixedLength), 
	m_bAG(bUseAG)
{
	// create timer for stim
	m_ptimer = new QTimer();
	m_ptimer->setSingleShot(true);
	
	// Create all stim states and transitions
	HState* sStimRequest = new HStimRequestState(this);
	HState* sStimRunning = new HStimRunningState(maxTrialLengthMS, m_ptimer, this);
	HState* sBail = new HState("stateBail", this);
	QFinalState* sFinal = new QFinalState(this);

	sStimRequest->addTransition(pMediaPlayer, SIGNAL(started()), sStimRunning);		// on entry, emits playStim()
 	QObject::connect(sStimRequest, SIGNAL(playStim(int)), pMediaPlayer, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()
	
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
		sStimRunning->addTransition(pLooker, SIGNAL(look(HLook)), sFinal);
	}
	
	
	// initial state will transition to sAG when attention getter is used, otherwise
	// to sStimRequest.
	HState* sInitial = new HState("sInitial", this);
	setInitialState(sInitial);
	if (bUseAG)
	{
		HAGState* sAG = new HAGState(this);
		HState* sAGRunning = new HState("stateAGRunning", this);
		sAG->addTransition(pMediaPlayer, SIGNAL(started()), sAGRunning);					// exit this state when ag has started signal that stim has started
		sInitial->addTransition(sAG);
		sAGRunning->addTransition(new HReadyTransition(pDialog, sStimRequest));				// ready transition looks for <Enter> key
		QObject::connect(sAG, SIGNAL(playStim(int)), pMediaPlayer, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()
	}
	else 
	{
		sInitial->addTransition(sStimRequest);
	}
	sBail->addTransition(sInitial);															// bailed trial automatically repeats
	
	
}

