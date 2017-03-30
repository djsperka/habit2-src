/*
 * HVIPlayer5.cpp
 *
 *  Created on: Feb 28, 2017
 *      Author: dan
 */

#include "HVIPlayer5.h"

#include <QtDebug>
#include <QUrl>
#include <exception>


HVIPlayer5::HVIPlayer5(int id, QWidget* w, const QDir& stimRootDir)
:HPlayer5(id, w, stimRootDir)
, m_mediaPlayer(this, QMediaPlayer::LowLatency)  // StreamPlayback) 	//QMediaPlayer::LowLatency)
{
	connect(&m_mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(stateChanged(QMediaPlayer::State)));
	connect(&m_mediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
}

void HVIPlayer5::stateChanged(QMediaPlayer::State state)
{
	switch(state)
	{
	case QMediaPlayer::StoppedState:
		qInfo() << "HVIPlayer5::stateChanged: stopped";
		break;
	case QMediaPlayer::PlayingState:
		qInfo() << "HVIPlayer5::stateChanged: playing";
		break;
	case QMediaPlayer::PausedState:
		qInfo() << "HVIPlayer5::stateChanged: paused";
		break;
	default:
		qInfo() << "HVIPlayer5::stateChanged: ???";
		break;
	}
}


/// Play the stim at index 'number'. Out of range index defaults to background.
void HVIPlayer5::play(unsigned int number)
{
	qInfo() << "HVIPlayer5::play(" << number << ")";
	if (media().contains(number))
	{
		QIODevice *iodevice = media()[number]->iodevice();
		iodevice->open(QIODevice::ReadOnly);
		if (iodevice->isOpen()) qInfo() << "open at " << iodevice->pos();
		m_mediaPlayer.setMedia(media()[number]->content(), iodevice);
		m_mediaPlayer.setPosition(0);

// works		m_pMediaPlayer->setMedia(QUrl::fromLocalFile("/Users/dan/Desktop/habit22/stim/examples/wav/jaw_harp.wav"));
		m_mediaPlayer.setVolume(getStimulusInfo(number).getVolume());
		m_mediaPlayer.play();
	}
	else
		qInfo() << "HVIPlayer5::play(" << number << ") - not found";

}

/// Stop playing the current stim. Has no effect on a video that has stopped, or
/// on an image or background.
void HVIPlayer5::stop()
{

}

/// Clear screen or equivalent. Emit cleared() when operation complete.
void HVIPlayer5::clear()
{

}

void HVIPlayer5::setVideoOutput(QVideoWidget *output)
{
	m_mediaPlayer.setVideoOutput(output);
}

void HVIPlayer5::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
	switch(status)
	{
	case QMediaPlayer::UnknownMediaStatus:
		qInfo() << "HVIPlayer5::mediaStatusChanged - QMediaPlayer::UnknownMediaStatus";
		break;
	case QMediaPlayer::NoMedia:
		qInfo() << "HVIPlayer5::mediaStatusChanged -  QMediaPlayer::NoMedia";
		break;
	case QMediaPlayer::LoadingMedia:
		qInfo() << "HVIPlayer5::mediaStatusChanged -  QMediaPlayer::LoadingMedia";
		break;
	case QMediaPlayer::LoadedMedia:
		qInfo() << "HVIPlayer5::mediaStatusChanged -  QMediaPlayer::LoadedMedia";
		break;
	case QMediaPlayer::StalledMedia:
		qInfo() << "HVIPlayer5::mediaStatusChanged -  QMediaPlayer::StalledMedia";
		break;
	case QMediaPlayer::BufferingMedia:
		qInfo() << "HVIPlayer5::mediaStatusChanged -  QMediaPlayer::BufferingMedia";
		break;
	case QMediaPlayer::BufferedMedia:
		qInfo() << "HVIPlayer5::mediaStatusChanged -  QMediaPlayer::BufferedMedia";
		break;
	case QMediaPlayer::EndOfMedia:
		qInfo() << "HVIPlayer5::mediaStatusChanged -  QMediaPlayer::EndOfMedia";
		break;
	case QMediaPlayer::InvalidMedia:
		qInfo() << "HVIPlayer5::mediaStatusChanged -  QMediaPlayer::InvalidMedia";
		break;
	default:
		qInfo() << "HVIPlayer5::mediaStatusChanged -  QMediaPlayer::?????";
		break;
	}
}
