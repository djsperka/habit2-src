/*
 *  HPhase.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 8/27/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhase.h"
#include <QFinalState>

HPhase::HPhase(HEventLog& log, const QList<QPair<int, Habit::StimulusSettings> >& stimuli, QObject* pMediaManager, HLookDetector* pLD, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG, HState* parent) 
	: HLogState(log, "Phase", parent)
	, m_stimuli(stimuli)
	, m_itrial(0)
{
	QAbstractTransition* trans;
	m_sTrial = new HTrial(*this, log, pMediaManager, pLD, maxTrialLengthMS, maxNoLookTimeMS, bFixedLength, bUseAG, this);
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
	
	// set next stim for first trial
	m_sTrial->setNextStim(m_stimuli.at(0).first, m_stimuli.at(0).second);
};

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
		qDebug() << "HPhase::advance() - trial " << m_itrial << " setNextStim " << m_stimuli.at(m_itrial).first;
		m_sTrial->setNextStim(m_stimuli.at(m_itrial).first, m_stimuli.at(m_itrial).second);
	}
	return b;
};
