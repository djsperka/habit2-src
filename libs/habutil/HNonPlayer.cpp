/*
 *  HNonPlayer.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 8/24/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HNonPlayer.h"

void HNonPlayer::play(int number)
{
	qDebug() << "HNonPlayer::play(" << number << ")";
	emit started(number);
}

void HNonPlayer::stop()
{
}

