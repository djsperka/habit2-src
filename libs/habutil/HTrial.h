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
#include <QEvent>
#include <QKeyEvent>
#include <QEventTransition>
#include <QDebug>
#include <QStateMachine>



/**
 Superstate class that contains all states required for a single trial. 
 The HTrial is a single state which will conduct a single trial. 
*/

class HTrial: public HState
{
public:
	HTrial(QObject* pDialog, QObject* pMediaPlayer, QObject*pLooker, int maxTrialLengthMS, bool bFixedLength, bool bUseAG, bool bUseLeft, bool bUseCenter, bool bUseRight);
	~HTrial() {};
protected:
private:
	QObject* m_pdialog;
	int m_maxTrialLengthMS;
	bool m_bFixedLength;
	bool m_bAG;
	bool m_bUseLeft;
	bool m_bUseCenter;
	bool m_bUseRight;
	QTimer* m_ptimer;	
};


class HAGState: public HState
{
	Q_OBJECT
	
public:
	
	HAGState(QState* parent = 0) : HState("HAGState", parent) {};
	~HAGState() {};
	
signals:
	
	void playStim(int);
	
protected:
	// on entry to this state we request that the attention getter be played. 
	// TODO: will need to have access to the media controller. 
	// TODO: exit transition should be triggered by the 'started' signal from the player.
	// Update: Make request by sending a signal (emit) to media controller. 
	
	void onEntry(QEvent* e) 
	{
		Q_UNUSED(e);
		HState::onEntry(e);
		emit playStim(0);
	};
};

class HReadyTransition : public QEventTransition
{
	Q_OBJECT
	
public:
	
	HReadyTransition(QObject* obj, QAbstractState* target) : QEventTransition(obj, QEvent::KeyPress) { setTargetState(target); };
	~HReadyTransition() {};
	
protected:
	
	// this is called for all keypress events, regardless of key. 
	// We test whether its the Enter key
	
	bool eventTest(QEvent *event) 
	{
		qDebug() << "eventTest";
		if (event->type() == QEvent::StateMachineWrapped &&
			static_cast<QStateMachine::WrappedEvent *>(event)->event()->type() == QEvent::KeyPress) {
			QEvent *wrappedEvent = static_cast<QStateMachine::WrappedEvent *>(event)->event();
			
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(wrappedEvent);
			int key = keyEvent->key();
			
			return key == Qt::Key_Enter;
		}
		return false;
	}
	
	
	void onTransition(QEvent* e) 
	{
		Q_UNUSED(e);
	};
	
};

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