/*
 * HAudioPlayer5.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#include "HAudioPlayer5.h"
#include <QtDebug>
#include <QUrl>
#include <exception>


HAudioPlayer5::HAudioPlayer5(int id, QWidget* w, const QDir& stimRootDir)
:HPlayer5(id, w, stimRootDir)
, m_mediaPlayer(this, QMediaPlayer::LowLatency)  // StreamPlayback) 	//QMediaPlayer::LowLatency)
{
	connect(&m_mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(stateChanged(QMediaPlayer::State)));
	connect(&m_mediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
}

void HAudioPlayer5::stateChanged(QMediaPlayer::State state)
{
	switch(state)
	{
	case QMediaPlayer::StoppedState:
		qInfo() << "HAudioPlayer5::stateChanged: stopped";
		break;
	case QMediaPlayer::PlayingState:
		qInfo() << "HAudioPlayer5::stateChanged: playing";
		break;
	case QMediaPlayer::PausedState:
		qInfo() << "HAudioPlayer5::stateChanged: paused";
		break;
	default:
		qInfo() << "HAudioPlayer5::stateChanged: ???";
		break;
	}
}


#if 0
player = new QMediaPlayer;

 playlist = new QMediaPlaylist(player);
 playlist->addMedia(QUrl("http://example.com/myfile1.mp3"));
 playlist->addMedia(QUrl("http://example.com/myfile2.mp3"));
 // ...
 playlist->setCurrentIndex(1);
 player->play();
#endif

HAudioPlayer5::~HAudioPlayer5()
{
}


/// Play the stim at index 'number'. Out of range index defaults to background.
void HAudioPlayer5::play(unsigned int number)
{
	qInfo() << "play(" << number << ")";
	if (media().contains(number))
	{
		m_mediaPlayer.setMedia(media()[number]->content(), media()[number]->iodevice());
// works		m_pMediaPlayer->setMedia(QUrl::fromLocalFile("/Users/dan/Desktop/habit22/stim/examples/wav/jaw_harp.wav"));
		m_mediaPlayer.setVolume(getStimulusInfo(number).getVolume());
		m_mediaPlayer.play();
	}
	else
		qInfo() << "play(" << number << ") - not found";

}

/// Stop playing the current stim. Has no effect on a video that has stopped, or
/// on an image or background.
void HAudioPlayer5::stop()
{

}

/// Clear screen or equivalent. Emit cleared() when operation complete.
void HAudioPlayer5::clear()
{

}

void HAudioPlayer5::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
	switch(status)
	{
	case QMediaPlayer::UnknownMediaStatus:
		qInfo() << "HAudioPlayer5::mediaStatusChanged - QMediaPlayer::UnknownMediaStatus";
		break;
	case QMediaPlayer::NoMedia:
		qInfo() << "HAudioPlayer5::mediaStatusChanged -  QMediaPlayer::NoMedia";
		break;
	case QMediaPlayer::LoadingMedia:
		qInfo() << "HAudioPlayer5::mediaStatusChanged -  QMediaPlayer::LoadingMedia";
		break;
	case QMediaPlayer::LoadedMedia:
		qInfo() << "HAudioPlayer5::mediaStatusChanged -  QMediaPlayer::LoadedMedia";
		break;
	case QMediaPlayer::StalledMedia:
		qInfo() << "HAudioPlayer5::mediaStatusChanged -  QMediaPlayer::StalledMedia";
		break;
	case QMediaPlayer::BufferingMedia:
		qInfo() << "HAudioPlayer5::mediaStatusChanged -  QMediaPlayer::BufferingMedia";
		break;
	case QMediaPlayer::BufferedMedia:
		qInfo() << "HAudioPlayer5::mediaStatusChanged -  QMediaPlayer::BufferedMedia";
		break;
	case QMediaPlayer::EndOfMedia:
		qInfo() << "HAudioPlayer5::mediaStatusChanged -  QMediaPlayer::EndOfMedia";
		break;
	case QMediaPlayer::InvalidMedia:
		qInfo() << "HAudioPlayer5::mediaStatusChanged -  QMediaPlayer::InvalidMedia";
		break;
	default:
		qInfo() << "HAudioPlayer5::mediaStatusChanged -  QMediaPlayer::?????";
		break;
	}
}
