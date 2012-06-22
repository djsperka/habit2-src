/*
 *  HStartTrialState.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <QState>
#include <QEvent>
#include <QStateMachine>
#include "HEvents.h"
#include "HState.h"

class HStartTrialState: public HState
{
	Q_OBJECT

public:

	HStartTrialState(): HState("HStartTrialState") {}
	~HStartTrialState() {}

protected:

	// on entry to this state, we will post an event to the state machine
	// that will trigger a transition to one of two states - either the AG
	// state or the Stim state. 
	
	virtual void onEntry(QEvent *e)
	{
		Q_UNUSED(e);
		HState::onEntry(e);
		machine()->postEvent(new HAGTrialEvent());
	}
};
