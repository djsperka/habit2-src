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

HMediaManager::~HMediaManager()
{
	qDebug("HMediaManager::~HMediaManager");
	QListIterator<HPlayer*> it(m_players);
	while (it.hasNext())
	{
		HPlayer* p = it.next();
		p->stop();
		delete p;	// error?
		//p->close();
	}
	qDebug("HMediaManager::~HMediaManager - done");
}

void HMediaManager::addPlayer(HPlayer* player, int screenIndex)
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
	m_players.append(player);
}

void HMediaManager::stim(int i)
{
	m_pendingStartSignal = true;
	m_pendingStimNumber = i;
	QListIterator<HPlayer *> it(m_players);
	while (it.hasNext())
	{
		it.next()->play(i);
	}
}

void HMediaManager::clear()
{
	m_pendingClearSignal = true;
	QListIterator<HPlayer *> it(m_players);
	while (it.hasNext())
	{
		it.next()->clear();
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
	qDebug() << "HMediaManager::playerStarted(int id = " << id << ") file " << filename << " pendingStart " << m_pendingStartSignal << " pendingAGStart " << m_pendingAGStartSignal;
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

