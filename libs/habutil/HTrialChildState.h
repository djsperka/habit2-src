/*
 *  HTrialChildState.h
 *  habutil
 *
 *  Created by Oakes Lab on 1/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _HTRIALCHILDSTATE_H_
#define _HTRIALCHILDSTATE_H_

#include <QString>
#include <QTimer>
#include <QAbstractTransition>
#include "HLogState.h"

class HTrial;

class HTrialChildState: public HLogState
{
	Q_OBJECT
	
public:
	HTrialChildState(HTrial& trial, HEventLog& log, const QString& name, QState* parent = 0)
	: HLogState(log, name, parent)
	, m_trial(trial)
	{};
	
	virtual ~HTrialChildState() {};
	
	HTrial& trial() { return m_trial; };
	
private:
	HTrial& m_trial;
};

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
//		HOutputGenerator::instance()->addLogItem(m_newrepeat, 0);
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
 State entered when stim starts playing. Starts a timer on entry.
 */



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
//		HOutputGenerator::instance()->addLogItem(HTrialLogItem::REPEAT_TRIAL, 0);
	};
};


#endif