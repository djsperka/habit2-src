/*
 *  HabitNonPlayer.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 8/24/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HabitNonPlayer.h"

void HabitNonPlayer::play(int number)
{
	qDebug() << "HabitNonPlayer::play(" << number << ")";
	emit started(number);
}

void HabitNonPlayer::stop()
{
}

