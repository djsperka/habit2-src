/*
 *  HabitMediaManager.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 7/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HabitMediaManager.h"

void HabitMediaManager::addPlayer(HabitPlayer* player)
{
	connect(player, SIGNAL(started(int)), this, SLOT(playerStarted(int)));
	m_players.append(player);
}

void HabitMediaManager::stim(int i)
{
	m_pendingStartSignal = true;
	QListIterator<HabitPlayer *> it(m_players);
	while (it.hasNext())
	{
		it.next()->play(i);
	}
}

void HabitMediaManager::ag()
{
	stim(0);
}

void HabitMediaManager::playerStarted(int id)
{
	Q_UNUSED(id);
	if (m_pendingStartSignal)
	{
		emit started();
		m_pendingStartSignal = false;
	}
}

