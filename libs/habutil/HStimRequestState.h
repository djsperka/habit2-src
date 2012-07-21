/*
 *  HStimState.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/8/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HSTIMLOOKSTATE_H_
#define _HSTIMLOOKSTATE_H_

#include <QState>
#include <QEvent>
#include "HState.h"

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

#endif