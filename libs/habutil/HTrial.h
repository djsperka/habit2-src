/*
 *  HTrial.h
 *  habutil
 *
 *  Created by Oakes Lab on 6/22/12.
 *  Copyright 2012 UC Davis Infant Cognition Lab. All rights reserved.
 *
 */

#ifndef _HTRIAL_H_
#define _HTRIAL_H_

#include "HPhaseChildState.h"
#include "HLookDetector.h"
#include "stimulussettings.h"
#include "HOutputGenerator.h"
#include <QObject>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QEventTransition>
#include <QDebug>
#include <QStateMachine>



/**
 Superstate class that contains all states required for a single trial. 
 The HTrial is a single state which will conduct a single trial. 
*/

class HTrial: public HPhaseChildState
{
	Q_OBJECT
	
public:
	HTrial(HPhase& phase, HEventLog& log, QObject* pMediaPlayer, HLookDetector* pLD, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG);
	~HTrial() {};
	void setNextStim(int i, const Habit::StimulusSettings& ss);
#ifdef NO_LONGER_USED
	HStimRequestState* getSStimRequest() { return m_sStimRequest; };
	HStimRunningState* getSStimRunning() { return m_sStimRunning; };
	HAGRequestState* getSAGRequest() { return m_sAGRequest; };
	HAGRunningState* getSAGRunning() { return m_sAGRunning; };
	HState* getSBail() { return m_sBail; };
	HState* getSInitial() { return m_sInitial; };
	HState* getSBailInitial() { return m_sBailInitial; };
#endif

protected:
private:
	HLookDetector* m_pLD;
	int m_maxTrialLengthMS;
	int m_maxNoLookTimeMS;
	bool m_bFixedLength;
	bool m_bAG;
	QTimer* m_ptimerMaxTrialLength;	
	QTimer* m_ptimerMaxNoLookTime;
#ifdef NO_LONGER_USED
	HStimRequestState* m_sStimRequest;
	HStimRunningState* m_sStimRunning;
	HAGRequestState* m_sAGRequest;
	HAGRunningState* m_sAGRunning;
#endif
	HState* m_sBail;
	HState* m_sInitial;
	HState* m_sBailInitial;
	
public slots:
	void onStimRunningEntered();
	void onAGRunningEntered();
	void onStimRunningExited();
	void onAGRunningExited();	
};



#endif