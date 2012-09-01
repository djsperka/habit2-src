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
#include "HLookDetector.h"
#include <QObject>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QEventTransition>
#include <QDebug>
#include <QStateMachine>


class HStimRequestState: public HState
{
	Q_OBJECT
	
	int m_nextStimID;
	
public:
	
	HStimRequestState(QState* parent = 0) : HState("HStimRequestState", parent), m_nextStimID(-1) {};
	~HStimRequestState() {};	
	void setNextStim(int i);
signals:
	void playStim(int);	
protected:
	// on entry to this state we request that the stim be played. 
	// by emiting a signal to media controller. 	
	void onEntry(QEvent* e);
};


/**
 Superstate class that contains all states required for a single trial. 
 The HTrial is a single state which will conduct a single trial. 
*/

class HTrial: public HState
{
	Q_OBJECT
	
public:
	HTrial(QObject* pMediaPlayer, HLookDetector* pLD, int maxTrialLengthMS, bool bFixedLength, bool bUseAG, HState* parent);
	~HTrial() {};
	void setNextStim(int i);
protected:
private:
	HLookDetector* m_pLD;
	int m_maxTrialLengthMS;
	bool m_bFixedLength;
	bool m_bAG;
	QTimer* m_ptimer;	
	HStimRequestState* m_sStimRequest;
	
public slots:
	void onStimRunningEntered();
	void onAGRunningEntered();
	void onStimRunningExited();
	void onAGRunningExited();	
};


class HAGRequestState: public HState
{
	Q_OBJECT
	
public:
	
	HAGRequestState(QState* parent = 0) : HState("HAGRequestState", parent) {};
	~HAGRequestState() {};
	
signals:
	
	void playAG();
	
protected:
	// on entry emit playAG() - media manager should act on that. 
	void onEntry(QEvent* e);
};

class HAGRunningState: public HState
{
	Q_OBJECT
	
public:
	
	HAGRunningState(QState* parent = 0) : HState("HAGRunningState", parent) {};
	~HAGRunningState() {};
	
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
	void onEntry(QEvent* e);

private:
	int m_ms;
	QTimer* m_ptimer;
};

#endif