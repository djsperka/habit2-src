/*
 * H5AudioPlayer.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: Oakeslab
 */

#include "H5AudioPlayer.h"



#if 0
player = new QMediaPlayer;
connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
player->setMedia(QUrl::fromLocalFile("/Users/me/Music/coolsong.mp3"));
player->setVolume(50);
player->play();
#endif

HAudioPlayer::HAudioPlayer(int id, QWidget *w) :
HPlayer(id, w), m_pendingStop(false), m_pMediaPlayer(0)
{
	// Generate image widget, media object, video widget, audio output
	// Special case is when this player is for audio only stimuli (as for control player and ISS stim)

	m_pMediaPlayer = new QMediaPlayer;

	// connect media object slot to handle looped video when needed

	//connect(m_pMediaObject, SIGNAL(prefinishMarkReached(qint32)), this, SLOT(onPrefinishMarkReached(qint32)));

}

void HAudioPlayer::stop()
{
	m_pMediaPlayer->stop();
}

void HAudioPlayer::play(int number)
{
	if (number >= 0 && number < m_sources.count())
	{
		m_pMediaPlayer->setMedia(QUrl::fromLocalFile(m_sources[number].filename()));
		m_pMediaPlayer->setVolume(m_sources[number].getAudioBalance());
		m_pMediaPlayer->play();
		m_iCurrentStim = number;
	}
}


void HAudioPlayer::clear()
{
	if (m_pMediaPlayer->state() == QMediaPlayer::StoppedState)
	{
		emit cleared(m_id);
	}
	else
	{
		m_pendingStop = true;
		m_pMediaPlayer->stop();
	}
}

void HAudioPlayer::onPrefinishMarkReached(qint32 msec)
{
	Q_UNUSED(msec);
	if (m_iCurrentStim > -1 && m_iCurrentStim < m_sources.count() && (m_sources.at(m_iCurrentStim)).type() == HStimulusSource::AUDIO && (m_sources.at(m_iCurrentStim)).isLooped())
	{
//		m_pMediaPlayer->pause();
//		m_pMediaPlayer->seek(0);
//		m_pMediaPlayer->play();
	}
}

void HAudioPlayer::onStateChanged(QMediaPlayer::State newState)
{
	if (m_pendingStop)
	{
		if (newState == QMediaPlayer::StoppedState)
		{
			m_pendingStop = false;
			emit cleared(m_id);
		}
	}
	else
	{
		if (newState == QMediaPlayer::PlayingState)
		{
			emit started(m_id);
		}
	}
	return;
}


