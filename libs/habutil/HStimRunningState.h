/*
 *  HStimRunningState.h
 *  habutil
 *
 *  Created by Oakes Lab on 7/12/12.
 *  Copyright 2012 UC Davis Infant Cognition Lab. All rights reserved.
 *
 */

#ifndef _HSTIMRUNNINGSTATE_H_
#define _HSTIMRUNNINGSTATE_H_

#include <QTimer>
#include <QState>
#include <QEvent>
#include <QSignalTransition>
#include "HState.h"
#include "HLook.h"

class HStimRunningState: public HState
{
	Q_OBJECT
	
private:
	bool m_bFixed;
	QTimer *m_ptimer;
	
public:
	
	HStimRunningState(int maxtimeMS, QObject *looker, QAbstractState *target, bool bFixed=false, QState* parent = 0) : HState("HStimRequestState", parent), m_bFixed(bFixed) 
	{
		m_ptimer = new QTimer();
		if (bFixed)
		{
			// timer timeout() signal will end
			addTransition(m_ptimer, SIGNAL(timeout()), target);
			
			// this transition only tallies looks in this case
			QSignalTransition *t = new QSignalTransition(looker, SIGNAL(look(HLook l)));
			addTransition(t);
		}
		else 
		{
			// this transition will end the trial when a look is received
			QSignalTransition *t = new QSignalTransition(looker, SIGNAL(look(HLook l)));
			addTransition(t);			
		}

			
		
	};
	~HStimRequestState() {};
	
signals:
	
	void playStim();
	
protected:
	// on entry to this state we request that the stim be played. 
	// Update: Make request by sending a signal (emit) to media controller. 
	
	void onEntry(QEvent* e) 
	{
		Q_UNUSED(e);
		HState::onEntry(e);
		emit playStim();
	};
};




#endif