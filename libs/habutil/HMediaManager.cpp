/*
 *  HMediaManager.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 7/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HMediaManager.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMapIterator>

HMediaManager::~HMediaManager()
{
	QMapIterator<HPlayerPositionType, HPlayer*> it(m_players);
	while (it.hasNext())
	{
		HPlayer* p = it.next().value();
		p->stop();
		delete p;	// error?
		//p->close();
	}
}

void HMediaManager::addPlayer(const HPlayerPositionType& ppt, HPlayer* player, int screenIndex)
{
	if (screenIndex >= 0)
	{
		QRect rect = QApplication::desktop()->screenGeometry(screenIndex);
		player->setGeometry(rect);
		player->move(rect.x(), rect.y());
	}
	else 
	{
		player->setGeometry(QRect(0, 0, 0, 0));
	}
	connect(player, SIGNAL(started(int, const QString&)), this, SLOT(playerStarted(int, const QString&)));
	m_players[ppt] = player;
}

void HMediaManager::stim(int i)
{
	m_pendingStartSignal = true;
	m_pendingStimNumber = i;
	QMapIterator<HPlayerPositionType, HPlayer *> it(m_players);
	while (it.hasNext())
	{
		it.next().value()->play(i);
	}
}

void HMediaManager::clear()
{
	m_pendingClearSignal = true;
	QMapIterator<HPlayerPositionType, HPlayer *> it(m_players);
	while (it.hasNext())
	{
		it.next().value()->clear();
	}
}

void HMediaManager::ag()
{
	m_pendingStartSignal = true;
	m_pendingAGStartSignal = true;
	stim(0);
}

void HMediaManager::playerStarted(int id, const QString& filename)
{
	emit screen(id, filename);
	if (m_pendingStartSignal)
	{
		if (m_pendingAGStartSignal)
		{
			emit agStarted(id);
		}
		else
		{
			emit stimStarted(m_pendingStimNumber);
		}
		m_pendingStartSignal = false;
		m_pendingAGStartSignal = false;
		m_pendingStimNumber = -1;
	}
}

void HMediaManager::playerCleared(int id)
{
	Q_UNUSED(id);
	if (m_pendingClearSignal)
	{
		m_pendingClearSignal = false;
		emit cleared();
	}
}

