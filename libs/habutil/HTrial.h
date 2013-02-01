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

#include "HTrialChildState.h"
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


class HInitialState: public HTrialChildState
{
	Q_OBJECT
	
	int m_newrepeat;
	
public:
	HInitialState(HTrial& trial, HEventLog& log, int new_repeat, QState* parent=0) : HTrialChildState(trial, log, "HInitialState", parent), m_newrepeat(new_repeat) {};
	~HInitialState() {};
	
protected:
	void onEntry(QEvent* e)
	{
		Q_UNUSED(e);
		HOutputGenerator::instance()->addLogItem(m_newrepeat, 0);
	};
};

class HAGRequestState: public HTrialChildState
{
	Q_OBJECT
	
public:
	
	HAGRequestState(HTrial& trial, HEventLog& log, QState* parent = 0) : HTrialChildState(trial, log, "HAGRequestState", parent) {};
	~HAGRequestState() {};
	
signals:
	
	void playAG();
	
protected:
	// on entry emit playAG() - media manager should act on that. 
	void onEntry(QEvent* e);
};

class HAGRunningState: public HTrialChildState
{
	Q_OBJECT
	
public:
	
	HAGRunningState(HTrial& trial, HEventLog& log, QState* parent = 0) : HTrialChildState(trial, log, "HAGRunningState", parent) {};
	~HAGRunningState() {};

	// on entry generate kAGStarted event 
	void onEntry(QEvent* e);

};


class HStimRequestState: public HTrialChildState
{
	Q_OBJECT
	
	int m_nextStimID;
	Habit::StimulusSettings m_nextStimulusSettings;
	
public:
	
	HStimRequestState(HTrial& trial, HEventLog& log, QState* parent = 0) : HTrialChildState(trial, log, "HStimRequestState", parent), m_nextStimID(-1) {};
	~HStimRequestState() {};	
	void setNextStim(int i, const Habit::StimulusSettings& ss);
signals:
	void playStim(int);	
protected:
	// on entry to this state we request that the stim be played. 
	// by emiting a signal to media controller. 	
	void onEntry(QEvent* e);
};

/**
 State entered when stim starts playing. Starts a timer on entry.
 */

class HNoLookTransition;

class HStimRunningState: public HTrialChildState
{
public:
	HStimRunningState(HTrial& trial, HEventLog& log, int msMax, HNoLookTransition* ptrans, QTimer* ptimerMax, int msNoLook, QTimer* ptimerNoLook, QState *parent=0) 
	: HTrialChildState(trial, log, "HStimRunning", parent)
	, m_ptransNoLook(ptrans)
	, m_msMax(msMax)
	, m_ptimerMax(ptimerMax)
	, m_msNoLook(msNoLook)
	, m_ptimerNoLook(ptimerNoLook)  
	{};
	~HStimRunningState() {};
	
protected:
	// Start timer on entry to this state. Also generate kStimRunning event. 
	void onEntry(QEvent* e);
	void onExit(QEvent* e);
	
private:
	HNoLookTransition* m_ptransNoLook;
	int m_msMax;
	QTimer* m_ptimerMax;
	int m_msNoLook;
	QTimer* m_ptimerNoLook;
};


class HBailState: public HTrialChildState
{
	Q_OBJECT
	
public:
	HBailState(HTrial& trial, HEventLog& log, QState* parent=0) 
	: HTrialChildState(trial, log, "BailState", parent) {};
	~HBailState() {};
	
protected:
	void onEntry(QEvent* e)
	{
		Q_UNUSED(e);
		HOutputGenerator::instance()->addLogItem(HTrialLogItem::REPEAT_TRIAL, 0);
	};
};

class HNoLookTransition: public QAbstractTransition
{
	Q_OBJECT
	
public:
	HNoLookTransition(int timerId) : m_timerId(timerId), m_bGotLook(false){};
	~HNoLookTransition() {};
	void gotLook() { m_bGotLook = true; };
	void reset() { m_bGotLook = false; };
protected:
	bool eventTest(QEvent* e);
	void onTransition(QEvent* e) { Q_UNUSED(e); };
private:
	int m_timerId;
	bool m_bGotLook;
};





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
	HStimRequestState* getSStimRequest() { return m_sStimRequest; };
	HStimRunningState* getSStimRunning() { return m_sStimRunning; };
	HAGRequestState* getSAGRequest() { return m_sAGRequest; };
	HAGRunningState* getSAGRunning() { return m_sAGRunning; };
	HState* getSBail() { return m_sBail; };
	HState* getSInitial() { return m_sInitial; };
	HState* getSBailInitial() { return m_sBailInitial; };
protected:
private:
	HLookDetector* m_pLD;
	int m_maxTrialLengthMS;
	int m_maxNoLookTimeMS;
	bool m_bFixedLength;
	bool m_bAG;
	QTimer* m_ptimerMaxTrialLength;	
	QTimer* m_ptimerMaxNoLookTime;
	HStimRequestState* m_sStimRequest;
	HStimRunningState* m_sStimRunning;
	HAGRequestState* m_sAGRequest;
	HAGRunningState* m_sAGRunning;
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