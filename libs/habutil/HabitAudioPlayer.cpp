/*
 *  HabitAudioPlayer.cpp
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HabitAudioPlayer.h"

HabitAudioPlayer::HabitAudioPlayer(int id, QWidget *w) : 
HabitPlayer(id, w), m_pMediaObject(0), m_pAudioOutput(0)
{
	// Generate image widget, media object, video widget, audio output
	// Special case is when this player is for audio only stimuli (as for control player and ISS stim)
	
	m_pMediaObject = new Phonon::MediaObject(this);
	m_pMediaObject->setObjectName("MediaObject");
	m_pAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	
	// connect media object slot to handle looped video when needed
	
	connect(m_pMediaObject, SIGNAL(prefinishMarkReached(qint32)), this, SLOT(onPrefinishMarkReached(qint32)));
	
	// Create paths
	Phonon::createPath(m_pMediaObject, m_pAudioOutput);
	
}

void HabitAudioPlayer::stop()
{
	m_pMediaObject->stop();
}

void HabitAudioPlayer::play(int number)
{
	if (number >= 0 && number < m_sources.count())
	{
		m_pMediaObject->setCurrentSource((m_sources[number].filename()));
		m_pMediaObject->play();
		m_iCurrentStim = number;
	}
}

void HabitAudioPlayer::onPrefinishMarkReached(qint32 msec)
{
	Q_UNUSED(msec);
	if (m_iCurrentStim > -1 && m_iCurrentStim < m_sources.count() && (m_sources.at(m_iCurrentStim)).type() == StimulusSource::AUDIO && (m_sources.at(m_iCurrentStim)).isLooped())
	{
		m_pMediaObject->pause();
		m_pMediaObject->seek(0);
		m_pMediaObject->play();
	}
}

void HabitAudioPlayer::onStateChanged(Phonon::State newState, Phonon::State oldState)
{
	Q_UNUSED(oldState);
	if (newState == Phonon::PlayingState)
	{
		emit started(m_id);
	}		
	return;
}
