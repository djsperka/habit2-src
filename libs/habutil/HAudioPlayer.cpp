/*
 *  HAudioPlayer.cpp
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HAudioPlayer.h"

HAudioPlayer::HAudioPlayer(int id, QWidget *w) : 
HPlayer(id, w), m_pendingStop(false), m_pMediaObject(0), m_pAudioOutput(0)
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

void HAudioPlayer::stop()
{
	m_pMediaObject->stop();
}

void HAudioPlayer::play(int number)
{
	if (number >= 0 && number < m_sources.count())
	{
		m_pMediaObject->setCurrentSource((m_sources[number]->filename()));
		m_pAudioOutput->setVolume(m_sources[number]->getAudioBalance());
		m_pMediaObject->play();
		m_iCurrentStim = number;
	}
}


void HAudioPlayer::clear()
{
	if (m_pMediaObject->state() == Phonon::StoppedState)
	{
		emit cleared(m_id);
	}
	else 
	{
		m_pendingStop = true;
		m_pMediaObject->stop();
	}
}

void HAudioPlayer::onPrefinishMarkReached(qint32 msec)
{
	Q_UNUSED(msec);
	if (m_iCurrentStim > -1 && m_iCurrentStim < m_sources.count() && (m_sources.at(m_iCurrentStim))->type() == HStimulusSource::AUDIO && (m_sources.at(m_iCurrentStim))->isLooped())
	{
		m_pMediaObject->pause();
		m_pMediaObject->seek(0);
		m_pMediaObject->play();
	}
}

void HAudioPlayer::onStateChanged(Phonon::State newState, Phonon::State oldState)
{
	Q_UNUSED(oldState);
	if (m_pendingStop)
	{
		if (newState == Phonon::StoppedState)
		{
			m_pendingStop = false;
			emit cleared(m_id);
		}
	}
	else
	{
		if (newState == Phonon::PlayingState)
		{
			emit started(m_id);
		}		
	}
	return;
}
