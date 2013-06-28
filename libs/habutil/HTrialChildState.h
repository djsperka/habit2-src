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
#include <QSignalTransition>
#include "HLogState.h"

class HTrial;
class HLookDetector;

class HTrialChildState: public HLogState
{
	Q_OBJECT
	
public:
	HTrialChildState(HTrial& trial, HEventLog& log, const QString& name)
	: HLogState(log, name, (QState *)&trial)
	, m_trial(trial)
	{};
	
	virtual ~HTrialChildState() {};
	
	HTrial& trial() { return m_trial; };
	
private:
	HTrial& m_trial;
};

class HTrialInitialState: public HTrialChildState
{
	Q_OBJECT
	
public:
	HTrialInitialState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HInitialState") {};
	~HTrialInitialState() {};
	
protected:
	virtual void onEntry(QEvent* e);
};


class HAGRequestState: public HTrialChildState
{
	Q_OBJECT
	
public:
	
	HAGRequestState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HAGRequestState") {};
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
	
	HAGRunningState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HAGRunningState") {};
	~HAGRunningState() {};
	
	void onEntry(QEvent* e);
	
};


class HStimRequestState: public HTrialChildState
{
	Q_OBJECT
	
public:
	
	HStimRequestState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HStimRequestState") {};
	~HStimRequestState() {};	

protected:
	// on entry to this state we request that the stim be played. 
	// by emiting a signal to media controller. 	
	void onEntry(QEvent* e);
};


class HGotLookState: public HTrialChildState
{
	Q_OBJECT

public:
	HGotLookState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HGotLookState") {};
	virtual ~HGotLookState() {};
	
protected:
	
	// onEntry - append kTrialEnd event
	void onEntry(QEvent* e);
};

class HFixedTimeoutState: public HTrialChildState
{
	Q_OBJECT
	
public:
	HFixedTimeoutState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HFixedTimeoutState") {};
	virtual ~HFixedTimeoutState() {};
	
protected:
	
	// onEntry - append kTrialEnd event
	void onEntry(QEvent* e);
};

class HNoLookTimeoutState: public HTrialChildState
{
	Q_OBJECT
	
public:
	HNoLookTimeoutState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HNoLookTimeoutState") {};
	virtual ~HNoLookTimeoutState() {};
	
protected:
	
	// onEntry - append kTrialEnd event
	void onEntry(QEvent* e);
};

	
	
class HNoLookTransition: public QSignalTransition
{
	Q_OBJECT
	
public:
	HNoLookTransition(HLookDetector& ld, QTimer *ptimer) : QSignalTransition(ptimer, SIGNAL(timeout())), m_ld(ld), m_ptimer(ptimer) {};
	~HNoLookTransition() {};
protected:
	bool eventTest(QEvent* e);
	void onTransition(QEvent* e) { Q_UNUSED(e); };
private:
	HLookDetector& m_ld;
	QTimer* m_ptimer;
};




/**
 State entered when stim starts playing. Starts a timer on entry.
 */



class HStimRunningState: public HTrialChildState
{
public:
	HStimRunningState(HTrial& trial, HEventLog& log, int msMax, QTimer* ptimerMax, int msNoLook, QTimer* ptimerNoLook)
	: HTrialChildState(trial, log, "HStimRunning")
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
	int m_msMax;
	QTimer* m_ptimerMax;
	int m_msNoLook;
	QTimer* m_ptimerNoLook;
};


#endif
