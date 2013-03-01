/*
 *  HPhase.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 8/27/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhase.h"
#include "HTrial.h"
#include "HElapsedTimer.h"
#include "HMediaManager.h"
#include <QFinalState>


HPhase::HPhase(HExperiment& exp, HPhaseCriteria* pcriteria, HEventLog& log, const QList<QPair<int, Habit::StimulusSettings> >& stimuli, const HPhaseType& ptype, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG)
	: HExperimentChildState(exp, log, "Phase")
	, m_pcriteria(pcriteria)
	, m_stimuli(stimuli)
	, m_ptype(ptype)
	, m_itrial(0)
{
	QAbstractTransition* trans;
	m_sTrial = new HTrial(*this, log, maxTrialLengthMS, maxNoLookTimeMS, bFixedLength, bUseAG);
	setInitialState(m_sTrial);
	HPhaseTrialCompleteState* sTrialComplete = new HPhaseTrialCompleteState(*this, log);
	m_sTrial->addTransition(m_sTrial, SIGNAL(finished()), sTrialComplete);
	QFinalState* sFinal = new QFinalState(this);
	
	// There is a transition from the trial state to trial complete state when user wants
	// to skip/abort a trial
	trans = new HAbortTrialTransition();
	trans->setTargetState(sTrialComplete);
	m_sTrial->addTransition(trans);
	
	// There are two event transitions from the TrialComplete state. 
	trans = new HAllTrialsDoneTransition();
	trans->setTargetState(sFinal);
	sTrialComplete->addTransition(trans);
	trans = new HNewTrialTransition();
	trans->setTargetState(m_sTrial);
	sTrialComplete->addTransition(trans);
};


void HPhase::screenStarted(int playerid)
{
	eventLog().append(new HStimStartEvent(playerid, HElapsedTimer::elapsed()));
}




void HPhase::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	
	// Post to log
	HState::onEntry(e);

	// post 'phase start' event to event log.
	eventLog().append(new HPhaseStartEvent(ptype(), HElapsedTimer::elapsed()));
	
	// connect media manager signal screen(int) to slot screenStarted(int)
	// disconnect onExit() from this state.
	QObject::connect(&experiment().getMediaManager(), SIGNAL(screen(int)), this, SLOT(screenStarted(int)));
	
};

void HPhase::onExit(QEvent* e)
{
	Q_UNUSED(e);

	// Post to log
	HState::onExit(e);

	// post 'phase end' event to event log. 
	eventLog().append(new HPhaseEndEvent(HElapsedTimer::elapsed()));

	// connect media manager signal screen(int) to slot screenStarted(int)
	// disconnect onExit() from this state.
	QObject::disconnect(&experiment().getMediaManager(), SIGNAL(screen(int)), this, SLOT(screenStarted(int)));	
	
};


void HPhase::requestCurrentStim()
{
	if (m_itrial < m_stimuli.size())
	{
		experiment().getMediaManager().stim(m_stimuli.at(m_itrial).first);
		eventLog().append(new HStimRequestEvent(m_stimuli.at(m_itrial).first, HElapsedTimer::elapsed()));
	}
	else 
	{
		experiment().getMediaManager().stim(-1);
		eventLog().append(new HStimRequestEvent(-1, HElapsedTimer::elapsed()));
		qDebug() << "Bad trial number (" << m_itrial << ")for phase " << m_name << " max " << m_stimuli.size();
	}

}

void HPhase::requestAG()
{
	// TODO: createMediaManager should ensure that position 0 is always ag or a blank screen. 
	// config should ensure that trials with no ag configured will not request stim(0). 
	
	experiment().getMediaManager().ag();
	eventLog().append(new HStimRequestEvent(0, HElapsedTimer::elapsed()));
}

bool HPhase::advance()
{
	bool b = true;
	m_itrial++;
	if (m_itrial == m_stimuli.size())
	{
		b = false;
	}
	else 
	{
		m_sTrial->setTrialNumber(m_itrial);
		qDebug() << "HPhase::advance() - trial " << m_itrial << " stimindex " << m_stimuli.at(m_itrial).first;
	}
	return b;
};

void HPhase::onTrialCompleteEntered()
{
	// what to do here. 
	// PhaseCriteria/kTrialEnd - that was done in prev states
};

