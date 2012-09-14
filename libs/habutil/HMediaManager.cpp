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
	m_pendingAGStartSignal = true;
	stim(0);
}

void HabitMediaManager::playerStarted(int id)
{
	Q_UNUSED(id);
	if (m_pendingStartSignal)
	{
		if (m_pendingAGStartSignal)
		{
			qDebug() << "HabitMediaManager::playerStarted(" << id << "): emit agStarted()";
			emit agStarted();
		}
		else
		{
			qDebug() << "HabitMediaManager::playerStarted(" << id << "): emit stimStarted()";
			emit stimStarted();
		}
		m_pendingStartSignal = false;
		m_pendingAGStartSignal = false;
	}
}

