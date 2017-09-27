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
#include "HGMM.h"

class HLookDetector;
class HPhase;

class HExperiment: public HLogState
{
	Q_OBJECT

public:
	HExperiment(HEventLog& log, HGMM& mm, HLookDetector& ld, QState* parent = 0);
	virtual ~HExperiment() {};
	
	bool hasPreTestPhase() { return m_pPreTestPhase==(HPhase*)NULL; };
	bool hasHabituationPhase() { return m_pHabituationPhase==(HPhase*)NULL; };
	bool hasTestPhase() { return m_pTestPhase==(HPhase*)NULL; };
	HPhase* pretest() { return m_pPreTestPhase; };
	HPhase* habituation() { return m_pHabituationPhase; };
	HPhase* test() { return m_pTestPhase; };
	HGMM& getMediaManager() { return m_mm; };
	const HGMM& getMediaManager() const { return m_mm; };
	HLookDetector& getLookDetector() { return m_ld; };
	const HLookDetector& getLookDetector() const { return m_ld; };
	void onExit(QEvent* e) 
	{
		Q_UNUSED(e);
		// on exiting experiment, tell Media Manager to put up blank screen.
		getMediaManager().stim(-1);
	};
	
signals:
	void phaseStarted(QString phaseName);
	void trialStarted(int trialnumber, int repeatnumber);

private slots:
	void onAttention();
	void onLook(HLook l);
	void onLookAborted(HLook l);

private:
	HGMM& m_mm;
	HLookDetector& m_ld;
	HPhase* m_pPreTestPhase;
	HPhase* m_pHabituationPhase;
	HPhase* m_pTestPhase;
};

#endif
