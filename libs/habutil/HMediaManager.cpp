/*
 *  HMediaManager.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 7/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HMediaManager.h"
#include "attentiongettersettings.h"
#include "stimulisettings.h"
#include "HTypes.h"
#include "HWorkspaceUtil.h"
#include <climits>
#include <QApplication>
#include <QDesktopWidget>
#include <QMapIterator>


const unsigned int HMediaManager::backgroundKey = UINT_MAX;
const unsigned int HMediaManager::agKey = 0;
const Habit::StimulusSettings HMediaManager::dummyStimulusSettings(QString("dummySS"));

HMediaManager::HMediaManager(bool bPlayersAreFullScreen)
: QObject()
, m_pendingStartSignal(false)
, m_pendingAGStartSignal(false)
, m_pendingClearSignal(false)
, m_pendingStimNumber(-1)
, m_bPlayersAreFullScreen(bPlayersAreFullScreen)
{
	// There will always be at least 2 stim keys.
	m_mapPStimulusSettings.insert(backgroundKey, &dummyStimulusSettings);
	m_mapPStimulusSettings.insert(agKey, &dummyStimulusSettings);
};


HMediaManager::~HMediaManager()
{
	qDebug() << "HMediaManager::~HMediaManager()";
	if (m_bPlayersAreFullScreen)
	{
		QMapIterator<HPlayerPositionType, HPlayer*> it(m_players);
		while (it.hasNext())
		{
			HPlayer* p = it.next().value();
			//p->stop();
			delete p;
		}
	}
}

unsigned int HMediaManager::nextKey()
{
	// Assumes that there are two pre-loaded keys -- see constructor.
	// All stimuli added via addStimulus (and by extension, addStimuli) are numbered 1, 2, 3, ...
	return (unsigned int)(m_mapPStimulusSettings.size() - 1);
}

void HMediaManager::addPlayer(const HPlayerPositionType& ppt, HPlayer* player, int screenIndex)
{
	// TODO: This should be governed by fullscreen setting!
	if (m_bPlayersAreFullScreen)
	{
		if (ppt != HPlayerPositionType::Sound)
		{
			QRect rect = QApplication::desktop()->screenGeometry(screenIndex);
			player->setGeometry(rect);
			player->move(rect.x(), rect.y());
			player->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
			player->show();
			qDebug() << "Player index " << screenIndex << " moved to rect " << rect;
		}
		else
		{
			player->setGeometry(QRect(0, 0, 0, 0));
			player->show();
			//player->hide();
			qDebug() << "Player index " << screenIndex << " moved to rect (0,0,0,0)";
		}
	}
	connect(player, SIGNAL(started(int, const QString&)), this, SLOT(playerStarted(int, const QString&)));
	m_players[ppt] = player;
	//qDebug() << "HMediaManager::addPlayer: player parent is " << (player->parent() ? player->parent()->objectName() : "NONE") << " hidden? " << player->isHidden() << " rect " << player->rect();
	//if (player->windowModality() == Qt::WindowModal) qDebug() << "WindowModal";
	//else if (player->windowModality() == Qt::ApplicationModal) qDebug() << "ApplicationModal";
	//else if (player->windowModality() == Qt::NonModal) qDebug() << "NonModal";
	//else qDebug() << "Modality???";
}


unsigned int HMediaManager::addAG(const Habit::StimulusSettings& ssAG)
{
	m_mapPStimulusSettings.insert(agKey, &ssAG);

	// Assume that if the player is here, then there is a stimulus configured for it.

	if (m_players.contains(HPlayerPositionType::Left))
	{
		m_players.value(HPlayerPositionType::Left)->addStimulus(agKey, ssAG.getLeftStimulusInfo());
	}
	if (m_players.contains(HPlayerPositionType::Center))
	{
		m_players.value(HPlayerPositionType::Center)->addStimulus(agKey, ssAG.getCenterStimulusInfo());
	}
	if (m_players.contains(HPlayerPositionType::Right))
	{
		m_players.value(HPlayerPositionType::Right)->addStimulus(agKey, ssAG.getRightStimulusInfo());
	}
	if (m_players.contains(HPlayerPositionType::Sound))
	{
		m_players.value(HPlayerPositionType::Sound)->addStimulus(agKey, ssAG.getIndependentSoundInfo());
	}

	// Append to context list.
	// This code allows more than one ag....nothing here prevents multiple ags to be added to the list.
	QList<unsigned int> list;
	list.append(agKey);
	addOrAppendList(HStimContext::AttentionGetter, list);

	return agKey;
}


void HMediaManager::addOrAppendList(const HStimContext& c, const QList<unsigned int>& list)
{
	if (m_mapContext.contains(c))
	{
		m_mapContext[c].append(list);
	}
	else
	{
		m_mapContext.insert(c, list);
	}
}

unsigned int HMediaManager::getContextStimList(const HStimContext& c, QList<unsigned int>& list)
{
	unsigned int n=0;
	if (m_mapContext.contains(c))
	{
		n = m_mapContext.value(c).size();
		list.append(m_mapContext.value(c));
	}
	return list.size();
}


//void HMediaManager::addStimuli(const Habit::StimuliSettings& ss, QList<unsigned int>& idList)
void HMediaManager::addStimuli(const Habit::StimuliSettings& ss)
{
	QList<unsigned int> idList;
	QListIterator<Habit::StimulusSettings> it(ss.stimuli());	// note: getting const HStimulusSettingsList&, it.next() will be const StimulusSettings&
	while (it.hasNext())
	{
		idList.append(addStimulus(it.next()));
	}
	addOrAppendList(ss.getStimContext(), idList);
	return;
}

unsigned int HMediaManager::addStimulus(unsigned int key, const Habit::StimulusSettings& settings)
{
	m_mapPStimulusSettings.insert(key, &settings);

	if (m_players.contains(HPlayerPositionType::Left))
	{
		m_players.value(HPlayerPositionType::Left)->addStimulus(key, settings.getLeftStimulusInfo());
	}

	if (m_players.contains(HPlayerPositionType::Center))
	{
		m_players.value(HPlayerPositionType::Center)->addStimulus(key, settings.getCenterStimulusInfo());
	}

	if (m_players.contains(HPlayerPositionType::Right))
	{
		m_players.value(HPlayerPositionType::Right)->addStimulus(key, settings.getRightStimulusInfo());
	}

	if (m_players.contains(HPlayerPositionType::Sound))
	{
		m_players.value(HPlayerPositionType::Sound)->addStimulus(key, settings.getIndependentSoundInfo());
	}

	return key;
}



unsigned int HMediaManager::addStimulus(const Habit::StimulusSettings& ss)
{
	return addStimulus(nextKey(), ss);
}



const Habit::StimulusSettings& HMediaManager::getStimulusSettings(unsigned int key) const
{
	if (m_mapPStimulusSettings.contains(key)) return *m_mapPStimulusSettings.value(key);
	else return dummyStimulusSettings;
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
	stim(agKey);
}

void HMediaManager::playerStarted(int id, const QString& filename)
{
	emit screen(id, filename);
	if (m_pendingStartSignal)
	{
		if (m_pendingAGStartSignal)
		{
			emit agStarted(agKey);
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

