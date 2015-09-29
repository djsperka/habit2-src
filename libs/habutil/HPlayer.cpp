/*
 *  HPlayer.cpp
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 Center for Neuroscience. All rights reserved.
 *
 */

#include "HPlayer.h"
#include <QFocusEvent>

const Habit::StimulusInfo HPlayer::dummyStimulusInfo(QString("dummy"), QString(), false, 0, true);

HPlayer::HPlayer(int ID, QWidget *w, const QDir& stimRootDir)
: QWidget(w)
, m_id(ID)
, m_iCurrentStim(0)
, m_dirStimulusRoot(stimRootDir)
, m_preferBufferedStimulus(true)
{
}

HPlayer::~HPlayer()
{
	//qDebug() << "HPlayer::~HPlayer()";
}

unsigned int HPlayer::addStimulus(const unsigned int id, const Habit::StimulusInfo& info)
{
	m_mapPStimulusInfo.insert(id, &info);
	return addStimulusPrivate(id);
}

const Habit::StimulusInfo& HPlayer::getStimulusInfo(unsigned int key)
{
	if (m_mapPStimulusInfo.contains(key))
		return *m_mapPStimulusInfo.value(key);
	else
		return HPlayer::dummyStimulusInfo;
}
