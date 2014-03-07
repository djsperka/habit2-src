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




//HTrial::HTrial(HPhase& phase, HEventLog& log, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG)

					  
HTrial::HTrial(HPhase& phase, HEventLog& log, const Habit::HPhaseSettings& phaseSettings, const Habit::HLookSettings& lookSettings, bool bUseAG)
	: HPhaseChildState(phase, log, "HTrial")
	, m_phaseSettings(phaseSettings)
	, m_lookSettings(lookSettings)
	, m_bAG(bUseAG)
	, m_trialNumber(0)
	, m_repeatNumber(0)
{
	// Create initial state. Do not define its transition yet. 
	HTrialInitialState* sInitial = new HTrialInitialState(*this, log);
	setInitialState(sInitial);
	
	// AG states
	HAGRequestState* sAGRequest = new HAGRequestState(*this, log);
	HAGRunningState* sAGRunning = new HAGRunningState(*this, log);

	// Stim request state and StimRunning states are similar to AGRequest and AGRunning. 
	HStimRequestState* sStimRequest = new HStimRequestState(*this, log);
	HStimRunningState* sStimRunning = new HStimRunningState(*this, log, m_phaseSettings);

	// Create final state.
	QFinalState* sFinal = new QFinalState(this);
		
	// Set connection for AGRequest state, which emits playAG() in its 'onEntry' method. 
	// That goes to the media manager, which starts the playhing of the AG.

	// 2/14/2013 stim request and ag request will no longer emit playStim() - instead will call phase().requestCurentStim()
	//QObject::connect(sAGRequest, SIGNAL(playAG()), &phase().getMediaManager(), SLOT(ag()));

	// Once that happens the media manager starts playing the ag, it emits
	// agStarted(). We use that signal as a transition to the AGRunning state. 
	sAGRequest->addTransition(&phase.experiment().getMediaManager(), SIGNAL(agStarted(int)), sAGRunning);
	
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

	// connect look detector signals to sStimRunning state's appropriate slots.
	QObject::connect(&phase.experiment().getLookDetector(), SIGNAL(lookStarted()), sStimRunning, SLOT(gotLookStarted()));
	QObject::connect(&phase.experiment().getLookDetector(), SIGNAL(lookAwayStarted()), sStimRunning, SLOT(gotLookAwayStarted()));

	sStimRequest->addTransition(&phase.experiment().getMediaManager(), SIGNAL(stimStarted(int)), sStimRunning);		// on entry, emits playStim()

	// Set up state(s) that sStimRunning can transition to.
	// These are turned on/off by the settings in m_phaseSettings.
	// First, max no look time
	// The timer is controlled by HStimRunningState, and it will post an HNoLookQEvent()
	// to the state machine. That triggers the HNoLookTransition to the HNoLookTimeoutState.
	// That state automatically transitions back to sInitial to repeat the trial.

	if (m_phaseSettings.getIsMaxNoLookTime())
	{
		HNoLookTimeoutState* sNoLookTimeout = new HNoLookTimeoutState(*this, log);
		HNoLookTransition* pNoLookTrans = new HNoLookTransition();
		pNoLookTrans->setTargetState(sNoLookTimeout);
		sStimRunning->addTransition(pNoLookTrans);
		sNoLookTimeout->addTransition(sInitial);	// trial is repeated
	}


	// Now the looking criteria to end a trial.
	// The two possibilities should be mutually exclusive but I won't attempt to check that here.
	if (m_phaseSettings.getUseLookingCriteria())
	{
		if (m_phaseSettings.getIsSingleLook())
		{
			HGotLookState* sGotLook = new HGotLookState(*this, log);
			sStimRunning->addTransition(&phase.experiment().getLookDetector(), SIGNAL(look(HLook)), sGotLook);
			sGotLook->addTransition(sFinal);
		}
		if (m_phaseSettings.getIsMaxAccumulatedLookTime())
		{
			HMaxAccumulatedLookTimeState* sMaxAccum = new HMaxAccumulatedLookTimeState(*this, log);
			sStimRunning->addTransition(&phase.experiment().getLookDetector(), SIGNAL(maxAccumulatedLookTime()), sMaxAccum);
			sMaxAccum->addTransition(sFinal);
		}
	}

	// Max look away time - just not interested
	if (m_phaseSettings.getIsMaxLookAwayTime())
	{
		HMaxLookAwayTimeState* sMaxLookAway = new HMaxLookAwayTimeState(*this, log);
		sStimRunning->addTransition(&phase.experiment().getLookDetector(), SIGNAL(maxLookAwayTime()), sMaxLookAway);
		sMaxLookAway->addTransition(sFinal);
	}

	// Max stim time. The timer is controlled by this class, not one of the subclasses.
	// create timer for stim always, even if not used.
	m_ptimerMaxStimulusTime = new QTimer();
	m_ptimerMaxStimulusTime->setSingleShot(true);

	if (m_phaseSettings.getIsMaxStimulusTime())
	{
		// and state for transition
		HMaxStimulusTimeState* sMaxStimulus = new HMaxStimulusTimeState(*this, log);
		sStimRunning->addTransition(m_ptimerMaxStimulusTime, SIGNAL(timeout()), sMaxStimulus);
		sMaxStimulus->addTransition(sFinal);

		// when starting from onset, the onStimRunningEntered() slot can start the timer.
		// when starting from looking, the onLookStarted() slot starts the timer.
		if (m_phaseSettings.getMeasureStimulusTimeFromLooking())
		{
			QObject::connect(&phase.experiment().getLookDetector(), SIGNAL(lookStarted()), this, SLOT(onLookStarted()));
		}
	}


	// Initial state transition to AG request or directly to stim request...
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
	qDebug() << "onStimRunning entered max stim time? " << m_phaseSettings.getIsMaxStimulusTime() << " from onset " << m_phaseSettings.getMeasureStimulusTimeFromOnset();
	qDebug() << "time " << m_phaseSettings.getMaxStimulusTime();
	if (m_phaseSettings.getIsMaxStimulusTime() && m_phaseSettings.getMeasureStimulusTimeFromOnset())
	{
		qDebug() << "starting timer " << m_phaseSettings.getMaxStimulusTime();
		m_ptimerMaxStimulusTime->start(m_phaseSettings.getMaxStimulusTime());
	}
}

void HTrial::onLookStarted()
{
	qDebug() << "onStimRunning entered max stim time? " << m_phaseSettings.getIsMaxStimulusTime() << " from onset " << m_phaseSettings.getMeasureStimulusTimeFromLooking();
	qDebug() << "time " << m_phaseSettings.getMaxStimulusTime();
	if (m_phaseSettings.getIsMaxStimulusTime() && m_phaseSettings.getMeasureStimulusTimeFromLooking())
	{
		m_ptimerMaxStimulusTime->start(m_phaseSettings.getMaxStimulusTime());
	}
}

void HTrial::onStimRunningExited()
{
	if (m_ptimerMaxStimulusTime->isActive()) m_ptimerMaxStimulusTime->stop();
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
