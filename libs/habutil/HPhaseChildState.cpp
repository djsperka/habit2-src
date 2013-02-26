/*
 *  HPhaseChildState.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 2/21/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhaseChildState.h"

void HPhaseTrialCompleteState::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	HPhaseChildState::onEntry(e);

	// Get phase log, check phase criteria
	// If phase complete, post AllTrialsDoneEvent
	// If not, generate new trial, set up HTrial to prepare, and post NewTrialEvent

#if 0
	if (phase().advance())
	{
		qDebug() << "HPhaseTrialCompleteState::onEntry - advance OK, post NewTrialEvent";
		machine()->postEvent(new HNewTrialEvent());
	}
	else 
	{
		qDebug() << "HPhaseTrialCompleteState::onEntry - all trials done, post AllTrialsDoneEvent";
		machine()->postEvent(new HAllTrialsDoneEvent());
	}
};
#endif

	return;
}