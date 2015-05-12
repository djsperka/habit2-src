/*
 *  HNonPlayer.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 8/24/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HNonPlayer.h"

using namespace Habit;

void HNonPlayer::play(int number)
{
	const StimulusInfo& info = getStimulusInfo(number);
	qDebug() << "HNonPlayer::play(" << number << ")";
	qDebug() << info;
	emit started(number, info.label());
}

void HNonPlayer::stop()
{
}

void HNonPlayer::clear()
{
	emit cleared(m_id);
}

unsigned int HNonPlayer::addStimulusPrivate(const unsigned int id)
{
	return id;
}

