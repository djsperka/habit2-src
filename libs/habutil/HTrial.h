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
	
	void setNextStim(int i) { m_nextStimID = i; };
signals:
	
	void playStim(int);
	
protected:
	// on entry to this state we request that the stim be played. 
	// Update: Make request by sending a signal (emit) to media controller. 
	
	void onEntry(QEvent* e) 
	{
		Q_UNUSED(e);
		HState::onEntry(e);
		emit playStim(m_nextStimID);
	};
};


/**
 Superstate class that contains all states required for a single trial. 
 The HTrial is a single state which will conduct a single trial. 
*/

class HTrial: public HState
{
	Q_OBJECT
	
public:
	HTrial(QObject* pDialog, QObject* pMediaPlayer, HLookDetector* pLD, int maxTrialLengthMS, bool bFixedLength, bool bUseAG);
	~HTrial() {};
	void setNextStim(int i)	{ m_sStimRequest->setNextStim(i); };
protected:
private:
	QObject* m_pdialog;
	HLookDetector* m_pLD;
	int m_maxTrialLengthMS;
	bool m_bFixedLength;
	bool m_bAG;
	QTimer* m_ptimer;	
	HStimRequestState* m_sStimRequest;
	
public slots:
	void onStimRunningEntered();
	void onAGRunningEntered();
	
	
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
	// on entry to this state we request that the attention getter be played. 
	// TODO: will need to have access to the media controller. 
	// TODO: exit transition should be triggered by the 'started' signal from the player.
	// Update: Make request by sending a signal (emit) to media controller. 
	
	void onEntry(QEvent* e) 
	{
		Q_UNUSED(e);
		qDebug() << "HAGRequestState::onEntry";
		HState::onEntry(e);
		emit playAG();
	};
};

class HAGRunningState: public HState
{
	Q_OBJECT
	
public:
	
	HAGRunningState(QState* parent = 0) : HState("HAGRunningState", parent) {};
	~HAGRunningState() {};
	
protected:
	void onEntry(QEvent* e) 
	{
		Q_UNUSED(e);
		qDebug() << "HAGRunningState::onEntry";
		HState::onEntry(e);
	};
};


/**
 State entered when stim starts playing. Starts a timer on entry.
 */

class HStimRunningState: public HState
{
public:
	HStimRunningState(int ms, QTimer* ptimer, QObject* pdialog, QState *parent=0) : HState("HStimRunning", parent), m_ms(ms), m_ptimer(ptimer), m_pdialog(pdialog) {};
	~HStimRunningState() {};
	
protected:
	// Start timer on entry to this state
	void onEntry(QEvent* e) 
	{
		Q_UNUSED(e);
		HState::onEntry(e);
		m_ptimer->start(m_ms);
		m_pdialog->installEventFilter(this);
	};
	
	void onExit(QEvent* e)
	{
		Q_UNUSED(e);
		m_pdialog->removeEventFilter(this);
	};
	
	
private:
	int m_ms;
	QTimer* m_ptimer;
	QObject* m_pdialog;
};

#endif