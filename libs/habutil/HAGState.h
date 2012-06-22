/*
 *  HAGState.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <QState>
#include <QEvent>
#include "HState.h"

class HAGState: public HState
{
	Q_OBJECT

public:

	HAGState() : HState("HAGState") {};
	~HAGState() {};

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
		HState::onEntry(e);
		emit playAG();
	};
};
