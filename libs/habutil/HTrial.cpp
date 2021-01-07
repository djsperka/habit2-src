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
#include "HGMM.h"
#include <QFinalState>
#include <QTimerEvent>
#include <QtDebug>




//HTrial::HTrial(HPhase& phase, HEventLog& log, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG)

					  
HTrial::HTrial(HPhase& phase, HEventLog& log, const Habit::HPhaseSettings& phaseSettings, const Habit::HLookSettings& lookSettings, const Habit::AttentionGetterSettings& agSettings, bool bTestingInput)
	: HPhaseChildState(phase, log, "HTrial")
	, m_phaseSettings(phaseSettings)
	, m_lookSettings(lookSettings)
	, m_agSettings(agSettings)
	, m_trialNumber(0)
	, m_repeatNumber(0)
{
	// Create initial state. Do not define its transition yet. 
	HTrialInitialState* sInitial = new HTrialInitialState(*this, log);
	setInitialState(sInitial);
	QObject::connect(sInitial, SIGNAL(trialStarted(int, unsigned int, unsigned int)), &this->phase().experiment(), SIGNAL(trialStarted(int, unsigned int, unsigned int)));
	QObject::connect(sInitial, SIGNAL(trialStarted(int, unsigned int, unsigned int)), &this->phase(), SLOT(trialStarted(int, unsigned int, unsigned int)));

	// AG states
	HAGRequestState* sAGRequest = new HAGRequestState(*this, log);
	HAGRunningState* sAGRunning = new HAGRunningState(*this, log);

	// Stim request state and StimRunning states are similar to AGRequest and AGRunning. 
	HStimRequestState* sStimRequest = new HStimRequestState(*this, log);
	HStimRunningState* sStimRunning = new HStimRunningState(*this, log, m_phaseSettings);

	// Create final state.
	QFinalState* sFinal = new QFinalState(this);




	// Once that happens the media manager starts playing the ag, it emits
	// agStarted(). We use that signal as a transition to the AGRunning state. 
	sAGRequest->addTransition(&(this->phase().experiment().getHGMM()), SIGNAL(agStarted(int)), sAGRunning);
	
	// When AGRunning state is entered, it tells the look detector to start looking for 
	// attention. It will emit an attention() signal when that happens (see below). 
	QObject::connect(sAGRunning, SIGNAL(entered()), this, SLOT(onAGRunningEntered()));
	QObject::connect(sAGRunning, SIGNAL(exited()), this, SLOT(onAGRunningExited()));

	// This timer is only used when fixed isi is specified. Create the timer always to guard against null ptr errs.
	m_ptimerFixedISI = new QTimer();
	m_ptimerFixedISI->setTimerType(Qt::PreciseTimer);
	m_ptimerFixedISI->setSingleShot(true);

	if (m_agSettings.isAttentionGetterUsed() || m_agSettings.isSoundOnly())
	{
		// When the look detector emits the attention() signal enter StimRequest state.
		sAGRunning->addTransition(&phase.experiment().getLookDetector(), SIGNAL(attention()), sStimRequest);
	}
	else if (m_agSettings.isFixedISI())
	{
		sAGRunning->addTransition(m_ptimerFixedISI, SIGNAL(timeout()), sStimRequest);
	}

	// StimRequest emits a playStim() signal, which is connected to the media manager. 
	// When media manager starts playing the stim it emits stimStarted(). That signal 
	// is used to exit to StimRunning.
	
	// 2/14/2013 stim request and ag request will no longer emit playStim() - instead will call phase().requestCurentStim()
 	//QObject::connect(sStimRequest, SIGNAL(playStim(int)), &phase().getMediaManager(), SLOT(stim(int)));// media player will receive this signal and emit stimStarted()
	QObject::connect(sStimRunning, SIGNAL(entered()), this, SLOT(onStimRunningEntered()));
	QObject::connect(sStimRunning, SIGNAL(exited()), this, SLOT(onStimRunningExited()));

	// connect look detector signals to sStimRunning state's appropriate slots.
	QObject::connect(&phase.experiment().getLookDetector(), SIGNAL(lookStarted()), sStimRunning, SLOT(gotLookStarted()));
	QObject::connect(&phase.experiment().getLookDetector(), SIGNAL(lookPending()), sStimRunning, SLOT(gotLookPending()));
	QObject::connect(&phase.experiment().getLookDetector(), SIGNAL(lookAborted(HLook)), sStimRunning, SLOT(gotLookAborted(HLook)));

	sStimRequest->addTransition(&(this->phase().experiment().getHGMM()), SIGNAL(stimStarted(int)), sStimRunning);		// on entry, emits playStim()

	// Set up state(s) that sStimRunning can transition to.
	// These are turned on/off by the settings in m_phaseSettings.
	// First, max no look time
	// The timer is controlled by HStimRunningState, and it will post an HNoLookQEvent()
	// to the state machine. That triggers the HNoLookTransition to the HNoLookTimeoutState.
	// That state automatically transitions back to sInitial to repeat the trial.
	// TODO CAn this get triggered but have another transition end the trial before the event is received?

	if (m_phaseSettings.getIsMaxNoLookTime())
	{
		HNoLookTimeoutState* sNoLookTimeout = new HNoLookTimeoutState(*this, log); // onEntry: HTrialEndEvent(HTrialEndType::HTrialEndNoLookTimeout, elapsed())
		HNoLookTransition* pNoLookTrans = new HNoLookTransition(); // fires on HNoLookQEvent::NoLookType - posted by HStimRunningState::onEntry
		pNoLookTrans->setTargetState(sNoLookTimeout);
		sStimRunning->addTransition(pNoLookTrans);
		sNoLookTimeout->addTransition(sInitial);	// trial is repeated
	}

	// Max look away time - just not interested.
	// The transition is triggered by SIGNAL(maxLookAwayTime()) from the look detector.
	if (m_phaseSettings.getIsMaxLookAwayTime())
	{
		HMaxLookAwayTimeState* sMaxLookAway = new HMaxLookAwayTimeState(*this, log, m_phaseSettings.getRepeatTrialOnMaxLookAwayTime());
		sStimRunning->addTransition(&phase.experiment().getLookDetector(), SIGNAL(maxLookAwayTime()), sMaxLookAway);
		if (m_phaseSettings.getRepeatTrialOnMaxLookAwayTime())
			sMaxLookAway->addTransition(sInitial);	// trial is repeated -- this is a failed trial!
		else
			sMaxLookAway->addTransition(sFinal);	// trial is NOT repeated -- this is successful
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

	// If this phase uses total looking time for habituation, then a trial may end when that habituation time is reached,
	// without any other trial-ending criteria. This is its own trial end type, used only in this special situation.
	if (m_phaseSettings.habituationSettings().getHabituationType() == HHabituationType::HHabituationTypeTotalLookingTime)
	{
		HPhaseAccumulatedLookTimeState *sPhaseAccum = new HPhaseAccumulatedLookTimeState(*this, log);
		sStimRunning->addTransition(&phase.experiment().getLookDetector(), SIGNAL(phaseAccumulatedLookTime()), sPhaseAccum);
		sPhaseAccum->addTransition(sFinal);
	}

	// Max stim time. The timer is controlled by this class, not one of the subclasses.
	// create timer for stim always, even if not used.
	m_ptimerMaxStimulusTime = new QTimer();
	m_ptimerMaxStimulusTime->setTimerType(Qt::PreciseTimer);
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

	// If the user aborts the trial during the AG this transition will pick it up.
	QAbstractTransition* trans;
	HAGAbortedState* sAGAborted = new HAGAbortedState(*this, log);
	trans = new HAbortTrialTransition();
	trans->setTargetState(sAGAborted);
	sAGRunning->addTransition(trans);
	sAGAborted->addTransition(sInitial);	// trial is repeated

	// If user aborts the trial during the stimulus this transition will pick it up.
	HStimAbortedState* sStimAborted = new HStimAbortedState(*this, log);
	trans = new HAbortTrialTransition();
	trans->setTargetState(sStimAborted);
	sStimRunning->addTransition(trans);
	sStimAborted->addTransition(sFinal);


	// Initial state transition to AG request or stim request, depending on whether an ISI is used.
	// When doing testing input, skip directly to stim running state: stimuli are not played, NO ISI even if one is configured!
	if (!bTestingInput)
	{
		if (m_agSettings.isAttentionGetterUsed())
		{
			sInitial->addTransition(sAGRequest);
		}
		else if (m_agSettings.isFixedISI())
		{
			sInitial->addTransition(sAGRequest);
		}
		else if (m_agSettings.isSoundOnly())
		{
			sInitial->addTransition(sAGRequest);
		}
		else
		{
			sInitial->addTransition(sStimRequest);
		}
	}
	else
	{
		sInitial->addTransition(sStimRunning);
	}
}


void HTrial::setTrialNumber(unsigned int i)
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

void HTrial::onExit(QEvent *)
{
	qDebug() << "HTrial::onExit phase " << phase().name() << "  trial " << m_trialNumber << " repeat " << m_repeatNumber;
}

void HTrial::onStimRunningEntered()
{
	qDebug() << "HTrial::onStimRunningEntered trial " << m_trialNumber << " repeat " << m_repeatNumber;
	phase().experiment().getLookDetector().enableLook();
//	eventLog().append(new HLookEnabledEvent(HElapsedTimer::elapsed()));
	if (m_phaseSettings.getIsMaxStimulusTime() && m_phaseSettings.getMeasureStimulusTimeFromOnset())
	{
		m_ptimerMaxStimulusTime->start(m_phaseSettings.getMaxStimulusTime());
	}
}

void HTrial::onLookStarted()
{
	if (m_phaseSettings.getIsMaxStimulusTime() && m_phaseSettings.getMeasureStimulusTimeFromLooking())
	{
		m_ptimerMaxStimulusTime->start(m_phaseSettings.getMaxStimulusTime());
	}
}

void HTrial::onStimRunningExited()
{
	if (m_ptimerMaxStimulusTime->isActive()) m_ptimerMaxStimulusTime->stop();
	phase().experiment().getLookDetector().disable();
//	eventLog().append(new HLookDisabledEvent(HElapsedTimer::elapsed()));
}

void HTrial::onAGRunningEntered()
{
	if (m_agSettings.isAttentionGetterUsed() || m_agSettings.isSoundOnly())
	{
		phase().experiment().getLookDetector().enableAGLook();
	}
	else if (m_agSettings.isFixedISI())
	{
		qDebug() << "onAGRunningEntered: start timer " << m_agSettings.getFixedISIMS();
		m_ptimerFixedISI->start(m_agSettings.getFixedISIMS() > 0 ? m_agSettings.getFixedISIMS() : 1);
	}
}

void HTrial::onAGRunningExited()
{
	if (m_agSettings.isAttentionGetterUsed() || m_agSettings.isSoundOnly())
	{
		phase().experiment().getLookDetector().disable();
	}
	else if (m_agSettings.isFixedISI())
	{
		if (m_ptimerFixedISI->isActive()) m_ptimerFixedISI->stop();
	}
}
