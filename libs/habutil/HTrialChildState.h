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
#include "HQEvents.h"
#include "HLogState.h"
#include "HPhaseSettings.h"

class HTrial;
class HLookDetector;

class HTrialChildState: public HLogState
{
	
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

signals:
	void trialStarted(int trialnumber, int repeatnumber);
	
protected:
	virtual void onEntry(QEvent* e);
};


class HAGRequestState: public HTrialChildState
{
	
public:
	
	HAGRequestState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HAGRequestState") {};
	~HAGRequestState() {};
	
protected:
	// on entry emit playAG() - media manager should act on that. 
	void onEntry(QEvent* e);
};

class HAGRunningState: public HTrialChildState
{
	
public:
	
	HAGRunningState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HAGRunningState") {};
	~HAGRunningState() {};
	
	void onEntry(QEvent* e);
	
};


class HStimRequestState: public HTrialChildState
{
	
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

public:
	HGotLookState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HGotLookState") {};
	virtual ~HGotLookState() {};
	
protected:
	
	// onEntry - append kTrialEnd event
	void onEntry(QEvent* e);
};

class HMaxAccumulatedLookTimeState: public HTrialChildState
{

public:
	HMaxAccumulatedLookTimeState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HMaxAccumulatedLookTimeState") {};
	virtual ~HMaxAccumulatedLookTimeState() {};

protected:

	// onEntry - append kTrialEnd event
	void onEntry(QEvent* e);
};

class HMaxLookAwayTimeState: public HTrialChildState
{

public:
	HMaxLookAwayTimeState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HMaxLookAwayTimeState") {};
	virtual ~HMaxLookAwayTimeState() {};

protected:

	// onEntry - append kTrialEnd event
	void onEntry(QEvent* e);
};

class HMaxStimulusTimeState: public HTrialChildState
{
	
public:
	HMaxStimulusTimeState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HMaxStimulusTimeState") {};
	virtual ~HMaxStimulusTimeState() {};
	
protected:
	
	// onEntry - append kTrialEnd event
	void onEntry(QEvent* e);
};

class HNoLookTimeoutState: public HTrialChildState
{
	
public:
	HNoLookTimeoutState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HNoLookTimeoutState") {};
	virtual ~HNoLookTimeoutState() {};
	
protected:
	
	// onEntry - append kTrialEnd event
	void onEntry(QEvent* e);
};




class HNoLookTransition: public QAbstractTransition
{
	
public:
	HNoLookTransition() {};
	~HNoLookTransition() {};
protected:
	bool eventTest(QEvent* e)
	{
		return (e->type() == QEvent::Type(HNoLookQEvent::NoLookType));
	};
	virtual void onTransition(QEvent* event)
	{
		Q_UNUSED(event);
	};
};

/**
 State entered when stim starts playing. Starts a timer on entry.
 */



class HStimRunningState: public HTrialChildState
{
	Q_OBJECT
public:
	HStimRunningState(HTrial& trial, HEventLog& log, const Habit::HPhaseSettings& phaseSettings);
	~HStimRunningState() {};

private slots:
	void noLookTimeout();
	void gotLookStarted();
	void gotLookPending();
	void gotLookAborted();

protected:
	// Start timer on entry to this state. Also generate kStimRunning event. 
	void onEntry(QEvent* e);
	void onExit(QEvent* e);
	
private:
	const Habit::HPhaseSettings& m_phaseSettings;
	QTimer* m_ptimerNoLook;
	bool m_bGotLookStarted;
	bool m_bGotLookPending;
	bool m_bNeedPendingBeforeAbort;
};



class HAGAbortedState: public HTrialChildState
{

public:
	HAGAbortedState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HAGAbortedState") {};
	virtual ~HAGAbortedState() {};

protected:

	// onEntry - do not end trial, simply repeat.
	void onEntry(QEvent* e);
};


class HStimAbortedState: public HTrialChildState
{

public:
	HStimAbortedState(HTrial& trial, HEventLog& log) : HTrialChildState(trial, log, "HStimAbortedState") {};
	virtual ~HStimAbortedState() {};

protected:

	// onEntry - append kTrialEnd event
	void onEntry(QEvent* e);
};


#endif
