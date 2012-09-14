/*
 *  HabitPlayer.cpp
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 Center for Neuroscience. All rights reserved.
 *
 */

#include "HabitPlayer.h"
#include <QFocusEvent>

HabitPlayer::HabitPlayer(int ID, QWidget *w) : QLabel(w), m_id(ID), m_iCurrentStim(0)
{
	m_sources.append(StimulusSource("", 0));	// dummy placeholder for attention getter stim. 
	setObjectName("HabitPlayer");
}

int HabitPlayer::addStimulus(QString filename, int volume, bool isLooped)
{
	StimulusSource s(filename, volume, isLooped);
	m_sources.append(s);
	
	// note: The QList m_sources always has at least 1 element - the attention getter placeholder is 
	// added when this class is initialized. Thus the return value from this method is always at least 1.
	return m_sources.count()-1;
}

int HabitPlayer::addAG(QString filename, int volume, bool isLooped)
{
	StimulusSource s(filename, volume, isLooped);
	m_sources[0] = s;
	return 0;
}

StimulusSource::StimulusSourceType HabitPlayer::getStimulusType(int index)
{
	StimulusSource::StimulusSourceType type = StimulusSource::BACKGROUND;
	if (index > -1 && index < m_sources.count())
		type = (m_sources.at(index)).type();
	return type;
}

QTextStream& operator<<(QTextStream& out, const HabitPlayer& player)
{
	out << "Player has " << player.count() << " sources:" << endl;
	for (int i=0; i<player.count(); i++)
	{
		out << i << ". " << player.source(i) << endl;
	}
	return out;
}

