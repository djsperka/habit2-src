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

HPhase::HPhase(const QList<int>& stimuli, QObject* pMediaManager, HLookDetector* pLD, int maxTrialLengthMS, bool bFixedLength, bool bUseAG, HState* parent) 
	: HState("Phase", parent), m_stimuli(stimuli), m_itrial(0)
{
	m_sTrial = new HTrial(pMediaManager, pLD, maxTrialLengthMS, bFixedLength, bUseAG, this);
	setInitialState(m_sTrial);
	HPhaseTrialCompleteState* sTrialComplete = new HPhaseTrialCompleteState(this);
	m_sTrial->addTransition(m_sTrial, SIGNAL(finished()), sTrialComplete);
	QFinalState* sFinal = new QFinalState(this);
	
	// There are two event transitions from the TrialComplete state. 
	QAbstractTransition* trans = new HAllTrialsDoneTransition();
	trans->setTargetState(sFinal);
	sTrialComplete->addTransition(trans);
	trans = new HNewTrialTransition();
	trans->setTargetState(m_sTrial);
	sTrialComplete->addTransition(trans);
	
	// set next stim for first trial
	m_sTrial->setNextStim(m_stimuli.at(0));
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
		qDebug() << "HPhase::advance() - trial " << m_itrial << " setNextStim " << m_stimuli.at(m_itrial);
		m_sTrial->setNextStim(m_stimuli.at(m_itrial));
	}
	return b;
};
