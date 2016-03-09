/*
 * HExperiment.cpp
 *
 *  Created on: Oct 6, 2015
 *      Author: Oakeslab
 */

#include "HExperiment.h"
#include "HElapsedTimer.h"
#include "HLookDetector.h"
#include <QObject>

HExperiment::HExperiment(HEventLog& log, HMediaManager& mm, HLookDetector& ld, QState* parent)
: HLogState(log, "HExperiment", parent)
, m_mm(mm)
, m_ld(ld)
, m_pPreTestPhase(NULL)
, m_pHabituationPhase(NULL)
, m_pTestPhase(NULL)
{
	connect(&m_ld, SIGNAL(attention()), this, SLOT(onAttention()));
	connect(&m_ld, SIGNAL(look(HLook)), this, SLOT(onLook(HLook)));
	connect(&m_ld, SIGNAL(lookAborted(HLook)), this, SLOT(onLookAborted(HLook)));
};

void HExperiment::onAttention()
{
	eventLog().append(new HAttentionEvent(HElapsedTimer::elapsed()));
}

void HExperiment::onLook(HLook l)
{
	eventLog().append(new HLookEvent(l, HElapsedTimer::elapsed()));
}

void HExperiment::onLookAborted(HLook l)
{
	eventLog().append(new HIncompleteLookEvent(l, HElapsedTimer::elapsed()));
}


