/*
 *  HAudioPlayer.cpp
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HAudioPlayer.h"

using namespace Habit;

HAudioPlayer::HAudioPlayer(int id, QWidget *w, const QDir& stimRootDir)
: HPlayer(id, w, stimRootDir)
, m_pendingStop(false)
, m_pMediaObject(0)
, m_pAudioOutput(0)
, m_nowPlayingFilename("NONE")
{
	// Generate image widget, media object, video widget, audio output
	// Special case is when this player is for audio only stimuli (as for control player and ISS stim)
	
	m_pMediaObject = new Phonon::MediaObject(this);
	m_pMediaObject->setObjectName("MediaObject");
	m_pAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	setFocusPolicy(Qt::NoFocus);
	
	// connect media object slot to handle looped video when needed
	
	connect(m_pMediaObject, SIGNAL(prefinishMarkReached(qint32)), this, SLOT(onPrefinishMarkReached(qint32)));

	// connect media object's stateChanged signal so we can emit started() and stopped() signals.

	connect(m_pMediaObject, SIGNAL(stateChanged (Phonon::State, Phonon::State)),
			this, SLOT(onStateChanged(Phonon::State, Phonon::State)));

	// Create paths
	Phonon::createPath(m_pMediaObject, m_pAudioOutput);

}

unsigned int HAudioPlayer::addStimulusPrivate(unsigned int id)
{
	// TODO: Store buffer of audio file?
	//qDebug() << "HAudioPlayer::addStimulusPrivate(" << id << ")";
	return id;
}

HAudioPlayer::~HAudioPlayer()
{
}

void HAudioPlayer::stop()
{
	m_pMediaObject->stop();
}

void HAudioPlayer::play(unsigned int number)
{
	const StimulusInfo& info = getStimulusInfo(number);
	m_pMediaObject->setCurrentSource(info.getAbsoluteFileName(getStimulusRoot()));
	m_pAudioOutput->setVolume((double)info.getVolume()/100.0);
	m_pMediaObject->play();
	m_iCurrentStim = number;
	m_nowPlayingFilename = info.getFileName();
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

	if (getCurrentStimulusInfo().isLoopPlayBack())
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
			emit started(m_id, m_nowPlayingFilename);
		}		
	}
	return;
}
