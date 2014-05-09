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
	m_sources[0] = new HStimulusSource("", 0);	// dummy placeholder for attention getter stim.
}

HPlayer::~HPlayer()
{
	QMapIterator<unsigned int, HStimulusSource*> it(m_sources);
	while (it.hasNext())
	{
		it.next();
		delete it.value();
	}
	m_sources.clear();
}

#if 0
unsigned int HPlayer::nextKey()
{
	unsigned int key = 1;
	if (!m_sources.keys().isEmpty()) key = m_sources.keys().last()+1;
	return key;
}
#endif

unsigned int HPlayer::addStimulus(unsigned int stimid, QString filename, int volume, bool isLooped)
{
	HStimulusSource *s = new HStimulusSource(filename, volume, isLooped);
	if (m_sources.contains(stimid)) delete m_sources[stimid];
	m_sources[stimid] = s;
	return stimid;
}

#if 0
unsigned int HPlayer::addStimulus(QString filename, int volume, bool isLooped)
{
	HStimulusSource *s = new HStimulusSource(filename, volume, isLooped);
	unsigned int key = nextKey();
	m_sources[key] = s;
	// return value should be the index to use m_sources[index] = the source being added.
	return m_sources.count()-1;
}
#endif

unsigned int HPlayer::addStimulus(unsigned int stimid)
{
	HStimulusSource *s = new HStimulusSource("", 0);
	if (m_sources.contains(stimid)) delete m_sources[stimid];
	m_sources[stimid] = s;
	return stimid;
}

#if 0
unsigned int HPlayer::addStimulus()
{
	return addStimulus(nextKey());
}
#endif

unsigned int HPlayer::addAG(QString filename, int volume, bool isLooped)
{
	HStimulusSource *s = new HStimulusSource(filename, volume, isLooped);
	// There is always an AG at [0] - see constructor.
	delete m_sources[0];
	m_sources[0] = s;
	return 0;
}

HStimulusSource::HStimulusSourceType HPlayer::getStimulusType(unsigned int index)
{
	HStimulusSource::HStimulusSourceType type = HStimulusSource::BACKGROUND;
	if (m_sources.contains(index))
		type = m_sources[index]->type();
	return type;
}

QTextStream& HPlayer::operator<<(QTextStream& out)
{
	out << "Player has " << m_sources.count() << " sources:" << endl;
	QMapIterator<unsigned int, HStimulusSource*> it(m_sources);
	while (it.hasNext())
	{
		it.next();
		out << it.key() << ". " << *(it.value()) << endl;
	}
	return out;
}

