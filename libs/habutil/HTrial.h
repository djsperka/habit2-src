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

#include "HState.h"
#include <QObject>
#include <QTimer>


/**
 Superstate class that contains all states required for a single trial. 
 The HTrial is a single state which will conduct a single trial. 
*/

class HTrial: public HState
{
public:
	HTrial(QObject* pDialog, QObject* pMediaPlayer, QObject*pLooker, int maxTrialLengthMS, bool bFixedLength, bool bUseAG);
	~HTrial() {};
	
private:
	int m_maxTrialLengthMS;
	bool m_bFixedLength;
	bool m_bAG;
	QTimer* m_ptimer;
	
};


/**
 State entered when stim starts playing. Starts a timer on entry.
 */

class HStimRunningState: public HState
{
public:
	HStimRunningState(int ms, QTimer* ptimer, QState *parent=0) : HState("HStimRunning", parent), m_ms(ms), m_ptimer(ptimer) {};
	~HStimRunningState() {};
	
protected:
	// Start timer on entry to this state
	void onEntry(QEvent* e) 
	{
		Q_UNUSED(e);
		HState::onEntry(e);
		m_ptimer->start(m_ms);
	};
	
private:
	int m_ms;
	QTimer *m_ptimer;
};

#endif