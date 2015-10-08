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
#include "HPhaseSettings.h"
#include "HLookSettings.h"
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
	HTrial(HPhase& phase, HEventLog& log, const Habit::HPhaseSettings& phaseSettings, const Habit::HLookSettings& lookSettings, bool bUseAG, bool bTestingInput = false);
	~HTrial() {};
	void setTrialNumber(int i);
	void incrementRepeatNumber();
	int getTrialNumber() { return m_trialNumber; };
	int getRepeatNumber() { return m_repeatNumber; };

protected:
	virtual void onEntry(QEvent* e);
//	virtual void onExit(QEvent* e);
	
private:
//	HLookDetector* m_pLD;
	const Habit::HPhaseSettings m_phaseSettings;
	const Habit::HLookSettings m_lookSettings;
//	int m_maxTrialLengthMS;
//	int m_maxNoLookTimeMS;
//	bool m_bFixedLength;
	bool m_bAG;
	int m_trialNumber;
	int m_repeatNumber;
	QTimer* m_ptimerMaxStimulusTime;
	
signals:
	void trialStarted(int trialnumber, int repeatnumber);

public slots:
	void onStimRunningEntered();
	void onAGRunningEntered();
	void onStimRunningExited();
	void onAGRunningExited();
	void onLookStarted();
};



#endif
