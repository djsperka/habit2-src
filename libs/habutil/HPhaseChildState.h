/*
 *  HPhaseChildState.h
 *  habutil
 *
 *  Created by Oakes Lab on 1/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HPHASECHILDSTATE_H_
#define _HPHASECHILDSTATE_H_

#include <QString>
#include <QStateMachine>
#include "HLogState.h"
#include "HQEvents.h"
#include "HPhase.h"
#include "HExperimentChildState.h"

class HExperiment;

class HPhaseChildState: public HLogState
{
	Q_OBJECT
	
public:
	HPhaseChildState(HPhase& phase, HEventLog& log, const QString& name)
	: HLogState(log, name, (QState*)&phase)
	, m_phase(phase)
	{};
	
	virtual ~HPhaseChildState() {};
	
	HPhase& phase() { return m_phase; };
	
private:
	HPhase& m_phase;
};


class HPhaseTrialStartingState: public HPhaseChildState
{
	Q_OBJECT
	
public:
	HPhaseTrialStartingState(HPhase& phase, HEventLog& log)
	: HPhaseChildState(phase, log, QString("HPhaseTrialStartingState"))
	{};
};


// See HPhase::onTrialCompleteEntered - test against criteria done there. 
class HPhaseTrialCompleteState: public HPhaseChildState
{
	Q_OBJECT
	
public:
	
	HPhaseTrialCompleteState(HPhase& phase, HEventLog& log) 
	: HPhaseChildState(phase, log, QString("HPhaseTrialCompleteState"))
	{};
	~HPhaseTrialCompleteState() {};
	
signals:
	
	
protected:
};



#endif