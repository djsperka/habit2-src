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
#include <climits>
#include <QApplication>
#include <QDesktopWidget>
#include <QMapIterator>


const unsigned int HMediaManager::backgroundKey=UINT_MAX;

HMediaManager::HMediaManager()
: QObject()
, m_pendingStartSignal(false)
, m_pendingAGStartSignal(false)
, m_pendingClearSignal(false)
, m_pendingStimNumber(-1)
{
	// There will always be at least 2 stim keys.
	Habit::StimulusSettings s("Background");
	m_mapStim.insert(backgroundKey, s);
	Habit::StimulusSettings ag("AG Placeholder");
	m_mapStim.insert(0, ag);
};


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

unsigned int HMediaManager::nextKey()
{
	return (unsigned int)(m_mapStim.size() - 1);
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


unsigned int HMediaManager::addAG(const Habit::StimulusSettings& ssAG)
{
	m_mapStim[0] = ssAG;
	if (m_players.contains(HPlayerPositionType::Left))
	{
		if (ssAG.isLeftEnabled())
		{
			Habit::StimulusInfo si = ssAG.getLeftStimulusInfo();
			m_players.value(HPlayerPositionType::Left)->addAG(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
		}
		else
		{
			qWarning("Left monitor is configured, and attention getter is used, but no left monitor attention getter stimulus is configured.");
		}
	}
	if (m_players.contains(HPlayerPositionType::Center))
	{
		if (ssAG.isCenterEnabled())
		{
			Habit::StimulusInfo si = ssAG.getCenterStimulusInfo();
			m_players.value(HPlayerPositionType::Center)->addAG(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
		}
		else
		{
			qWarning("Center monitor is configured, and attention getter is used, but no center monitor attention getter stimulus is configured.");
		}
	}
	if (m_players.contains(HPlayerPositionType::Right))
	{
		if (ssAG.isRightEnabled())
		{
			Habit::StimulusInfo si = ssAG.getRightStimulusInfo();
			m_players.value(HPlayerPositionType::Right)->addAG(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
		}
		else
		{
			qWarning("Right monitor is configured, and attention getter is used, but no right monitor attention getter stimulus is configured.");
		}
	}
	if (m_players.contains(HPlayerPositionType::Sound))
	{
		if (ssAG.isIndependentSoundEnabled())
		{
			Habit::StimulusInfo si = ssAG.getIndependentSoundInfo();
			m_players.value(HPlayerPositionType::Sound)->addAG(si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
		}
		else
		{
			qWarning("Control monitor is configured, and attention getter is used, but no independent sound attention getter stimulus is configured.");
		}
	}
	return 0;
}


void HMediaManager::addStimuli(const Habit::StimuliSettings& ss, QList<unsigned int>& idList)
{
	// it is assumed that we only add stimuli, not the AG, via this method.
	// That means the first stim added will never have key=0, instead it will start
	// at
	unsigned int key = 1;
	unsigned int test = 0;
	for (int i=0; i<ss.getStimuli().size(); i++)
	{
		key = nextKey();
		idList.append(addStimulus(key, ss.getStimuli().at(i)));
	}
	return;
}

unsigned int HMediaManager::addStimulus(unsigned int key, const Habit::StimulusSettings& settings)
{
	m_mapStim[key] = settings;
	if (m_players.contains(HPlayerPositionType::Left))
	{
		if (settings.isLeftEnabled())
		{
			Habit::StimulusInfo si = settings.getLeftStimulusInfo();
			m_players.value(HPlayerPositionType::Left)->addStimulus(key, si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
		}
		else
		{
			qWarning() << "Left monitor configured, but no left stimulus for stim named \"" << settings.getName() << "\"";
			m_players.value(HPlayerPositionType::Left)->addStimulus(key);
		}
	}

	if (m_players.contains(HPlayerPositionType::Center))
	{
		if (settings.isCenterEnabled())
		{
			Habit::StimulusInfo si = settings.getCenterStimulusInfo();
			m_players.value(HPlayerPositionType::Center)->addStimulus(key, si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
		}
		else
		{
			qWarning() << "Center monitor configured, but no center stimulus for stim named \"" << settings.getName() << "\"";
			m_players.value(HPlayerPositionType::Center)->addStimulus(key);
		}
	}

	if (m_players.contains(HPlayerPositionType::Right))
	{
		if (settings.isRightEnabled())
		{
			Habit::StimulusInfo si = settings.getRightStimulusInfo();
			m_players.value(HPlayerPositionType::Right)->addStimulus(key, si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
		}
		else
		{
			qWarning() << "Right monitor configured, but no right stimulus for stim named \"" << settings.getName() << "\"";
			m_players.value(HPlayerPositionType::Right)->addStimulus(key);
		}
	}

	if (m_players.contains(HPlayerPositionType::Sound))
	{
		if (settings.isIndependentSoundEnabled())
		{
			Habit::StimulusInfo si = settings.getIndependentSoundInfo();
			m_players.value(HPlayerPositionType::Sound)->addStimulus(key, si.getFileName(), si.getAudioBalance(), si.isLoopPlayBack());
		}
		else
		{
			qWarning() << "Independent sound configured, but no sound stimulus for stim named \"" << settings.getName() << "\"";
			m_players.value(HPlayerPositionType::Sound)->addStimulus(key);
		}
	}
	return key;
}



unsigned int HMediaManager::addStimulus(const Habit::StimulusSettings& ss)
{
	return addStimulus(nextKey(), ss);
}



Habit::StimulusSettings HMediaManager::getStimulusSettings(unsigned int key)
{
	if (m_mapStim.contains(key)) return m_mapStim.value(key);
	else return m_mapStim.value(backgroundKey);
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

