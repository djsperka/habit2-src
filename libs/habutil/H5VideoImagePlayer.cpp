/*
 * H5VideoImagePlayer.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: Oakeslab
 */


#include "H5VideoImagePlayer.h"

HVideoImagePlayer::HVideoImagePlayer(int id, QWidget *w, bool fullscreen, bool maintainAspectRatio) :
HPlayer(id, w), m_pendingClear(false), m_parent(w), m_pMediaPlayer(0), m_pVideoWidget(0), m_isFullScreen(fullscreen), m_maintainAspectRatio(maintainAspectRatio)
{
	// This combination needed to get the "close window when app exits"
	// right. Make sure to call with the parent as the thing that should
	// kill this window when closed.

	setWindowFlags(Qt::Window);
	showFullScreen();

	m_pMediaPlayer = new QMediaPlayer;
	/*
	player->setMedia(QUrl::fromLocalFile("/Users/me/Music/coolsong.mp3"));
	player->setVolume(50);
	player->play();
	*/

	m_pVideoWidget = new QVideoWidget(this);
	m_pMediaPlayer->setVideoOutput(m_pVideoWidget);

	connect(m_pMediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onStateChanged(QMediaPlayer::State)));

}


HVideoImagePlayer::~HVideoImagePlayer()
{
    delete m_pVideoWidget;
}


void HVideoImagePlayer::onStateChanged(QMediaPlayer::State newState)
{
	if (newState == QMediaPlayer::PlayingState)
	{
		qDebug() << "Playing...";
		emit started(m_id);
	}
	else if (newState == QMediaPlayer::StoppedState)
	{
		qDebug() << "Stopped.";
	}
	return;
}

void HVideoImagePlayer::stop()
{
	m_pMediaPlayer->stop();
}

void HVideoImagePlayer::clear()
{
	m_pendingClear = true;
	play(-1);
}


void HVideoImagePlayer::play(int number)
{
	HStimulusSource::HStimulusSourceType newType = getStimulusType(number);
	m_pMediaPlayer->setMedia(QUrl::fromLocalFile("/Users/Oakeslab/Desktop/stim/ball_fig8_bell.mov"));
	m_pMediaPlayer->play();
	m_iCurrentStim = number;
}



