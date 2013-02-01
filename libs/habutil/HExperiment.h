/*
 *  HExperiment.h
 *  habutil
 *
 *  Created by Oakes Lab on 1/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HEXPERIMENT_H_
#define _HEXPERIMENT_H_

#include "HLogState.h"
#include "HPhase.h"

class HExperiment: public HLogState
{
	Q_OBJECT
	
public:
	HExperiment(HEventLog& log, QState* parent = 0) : HLogState(log, "HExperiment", parent) {};
	~HExperiment() {};
	
	bool hasPreTestPhase() { return m_pPreTestPhase==(HPhase*)NULL; };
	bool hasHabituationPhase() { return m_pHabituationPhase==(HPhase*)NULL; };
	bool hasTestPhase() { return m_pTestPhase==(HPhase*)NULL; };
	HPhase* pretest() { return m_pPreTestPhase; };
	HPhase* habituation() { return m_pHabituationPhase; };
	HPhase* test() { return m_pTestPhase; };
	
	void onExit(QEvent* e) 
	{
		Q_UNUSED(e);
		// on exiting experiment, tell Media Manager to put up blank screen.
		emit playStim(-1);
	};
signals:
	void playStim(int);
	
private:
	HPhase* m_pPreTestPhase;
	HPhase* m_pHabituationPhase;
	HPhase* m_pTestPhase;
};

#endif
