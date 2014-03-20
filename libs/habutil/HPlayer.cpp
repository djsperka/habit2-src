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

HPlayer::HPlayer(int ID, QWidget *w) : QWidget(w), m_id(ID), m_iCurrentStim(0)
{
	m_sources.append(new HStimulusSource("", 0));	// dummy placeholder for attention getter stim.
}

HPlayer::~HPlayer()
{
	while (!m_sources.isEmpty())
		delete m_sources.takeFirst();
}

int HPlayer::addStimulus(QString filename, int volume, bool isLooped)
{
	HStimulusSource *s = new HStimulusSource(filename, volume, isLooped);
	m_sources.append(s);
	
	// note: The QList m_sources always has at least 1 element - the attention getter placeholder is 
	// added when this class is initialized. Thus the return value from this method is always at least 1.
	return m_sources.count()-1;
}

int HPlayer::addStimulus()
{
	HStimulusSource *s = new HStimulusSource("", 0);
	m_sources.append(s);
	return m_sources.count()-1;
}


int HPlayer::addAG(QString filename, int volume, bool isLooped)
{
	HStimulusSource *s = new HStimulusSource(filename, volume, isLooped);
	m_sources[0] = s;
	return 0;
}

HStimulusSource::HStimulusSourceType HPlayer::getStimulusType(int index)
{
	HStimulusSource::HStimulusSourceType type = HStimulusSource::BACKGROUND;
	if (index > -1 && index < m_sources.count())
		type = (*m_sources.at(index)).type();
	return type;
}

QTextStream& operator<<(QTextStream& out, const HPlayer& player)
{
	out << "Player has " << player.count() << " sources:" << endl;
	for (int i=0; i<player.count(); i++)
	{
		out << i << ". " << player.source(i) << endl;
	}
	return out;
}

