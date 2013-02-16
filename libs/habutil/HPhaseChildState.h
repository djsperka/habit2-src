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
	// on entry to this state we request that the stim be played. 
	// Update: Make request by sending a signal (emit) to media controller. 
	
	// TODO: this is where we apply the HPhaseCriteria to see if the phase has ended. 
	
	void onEntry(QEvent* e);
#if 0
	{
		Q_UNUSED(e);
		HState::onEntry(e);
		
		if (phase().advance())
		{
			qDebug() << "HPhaseTrialCompleteState::onEntry - advance OK, post NewTrialEvent";
			machine()->postEvent(new HNewTrialEvent());
		}
		else 
		{
			qDebug() << "HPhaseTrialCompleteState::onEntry - all trials done, post AllTrialsDoneEvent";
			machine()->postEvent(new HAllTrialsDoneEvent());
		}
	};
#endif
};



#endif