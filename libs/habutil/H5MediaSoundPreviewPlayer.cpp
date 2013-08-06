/*
 * H5MediaSoundPreviewPlayer.cpp
 *
 *  Created on: Jul 26, 2013
 *      Author: Oakeslab
 */


#include "H5MediaSoundPreviewPlayer.h"
#include <QFileInfo>

MediaSoundPreviewPlayer::MediaSoundPreviewPlayer(QWidget *parent)
	: QWidget(parent)
{
	_pMediaPlayer1 = new QMediaPlayer(this);
	_pMediaPlayer2 = new QMediaPlayer(this);
	_pMediaPlayer3 = new QMediaPlayer(this);
	_pMediaPlayer4 = new QMediaPlayer(this);
	_mediaWait1 = _mediaWait2 = _mediaWait3 = _mediaWait4 = false;
	_mediaLoaded1 = _mediaLoaded2 = _mediaLoaded3 = _mediaLoaded4 = false;
	_playRequest = false;
	connect(_pMediaPlayer1, SIGNAL(stateChanged (QMediaPlayer::State)), this, SLOT(onMedia1StateChanged(QMediaPlayer::State)));
	connect(_pMediaPlayer2, SIGNAL(stateChanged (QMediaPlayer::State)), this, SLOT(onMedia2StateChanged(QMediaPlayer::State)));
	connect(_pMediaPlayer3, SIGNAL(stateChanged (QMediaPlayer::State)), this, SLOT(onMedia3StateChanged(QMediaPlayer::State)));
	connect(_pMediaPlayer4, SIGNAL(stateChanged (QMediaPlayer::State)), this, SLOT(onMedia4StateChanged(QMediaPlayer::State)));
}

MediaSoundPreviewPlayer::~MediaSoundPreviewPlayer()
{

}

void MediaSoundPreviewPlayer::setMedias( QString mediaSrc1, QString mediaSrc2, QString mediaSrc3, QString mediaSrc4 )
{
	if (QFileInfo(mediaSrc1).exists() && isMediaPlayable(mediaSrc1))
	{
		_mediaLoaded1 = false;
		_mediaWait1 = true;
		_pMediaPlayer1->setMedia(QUrl::fromLocalFile(mediaSrc1));
	}
	if (QFileInfo(mediaSrc2).exists() && isMediaPlayable(mediaSrc2))
	{
		_mediaLoaded2 = false;
		_mediaWait2 = true;
		_pMediaPlayer2->setMedia(QUrl::fromLocalFile(mediaSrc2));
	}
	if (QFileInfo(mediaSrc3).exists() && isMediaPlayable(mediaSrc3))
	{
		_mediaLoaded3 = false;
		_mediaWait3 = true;
		_pMediaPlayer3->setMedia(QUrl::fromLocalFile(mediaSrc3));
	}
	if (QFileInfo(mediaSrc4).exists() && isMediaPlayable(mediaSrc4))
	{
		_mediaLoaded4 = false;
		_mediaWait4 = true;
		_pMediaPlayer4->setMedia(QUrl::fromLocalFile(mediaSrc4));
	}
}

void MediaSoundPreviewPlayer::setVolumes( double volume1, double volume2, double volume3, double volume4 )
{
	_pMediaPlayer1->setVolume(volume1/100.0);
	_pMediaPlayer2->setVolume(volume2/100.0);
	_pMediaPlayer3->setVolume(volume3/100.0);
	_pMediaPlayer4->setVolume(volume4/100.0);
}

void MediaSoundPreviewPlayer::onMedia1StateChanged(QMediaPlayer::State newState)
{
	_mediaLoaded1 = true;
	onLoadingComplete();
}

void MediaSoundPreviewPlayer::onMedia2StateChanged(QMediaPlayer::State newState)
{
	_mediaLoaded2 = true;
	onLoadingComplete();
}

void MediaSoundPreviewPlayer::onMedia3StateChanged(QMediaPlayer::State newState)
{
	_mediaLoaded3 = true;
	onLoadingComplete();
}

void MediaSoundPreviewPlayer::onMedia4StateChanged(QMediaPlayer::State newState)
{
	_mediaLoaded4 = true;
	onLoadingComplete();
}

void MediaSoundPreviewPlayer::onLoadingComplete()
{
	bool ready = (!_mediaWait1 || (_mediaWait1 && _mediaLoaded1))
		&& (!_mediaWait2 || (_mediaWait2 && _mediaLoaded2))
		&& (!_mediaWait3 || (_mediaWait3 && _mediaLoaded3))
		&& (!_mediaWait4 || (_mediaWait4 && _mediaLoaded4));
	if (ready && _playRequest)
	{
		if (_mediaLoaded1)
			_pMediaPlayer1->play();
		if (_mediaLoaded2)
			_pMediaPlayer2->play();
		if (_mediaLoaded3)
			_pMediaPlayer3->play();
		if (_mediaLoaded4)
			_pMediaPlayer4->play();
	}
}

void MediaSoundPreviewPlayer::playMedias()
{
	_playRequest = true;
	onLoadingComplete();
}

void MediaSoundPreviewPlayer::stopMedias()
{
	_pMediaPlayer1->stop();
	_pMediaPlayer2->stop();
	_pMediaPlayer3->stop();
	_pMediaPlayer4->stop();
}

bool MediaSoundPreviewPlayer::isMediaPlayable( const QString& filename )
{
	QFileInfo fileinfo(filename);
	QString suffix = fileinfo.suffix().toLower();
	return ( suffix == "avi" ||
		suffix == "mp4" ||
		suffix == "wmf" ||
		suffix == "asf" ||
		suffix == "wmv" ||
		suffix == "mov" ||
		suffix == "mp3" ||
		suffix == "ogg" ||
		suffix == "wma" ||
		suffix == "wav" ||
		suffix == "aiff"
		);
}


