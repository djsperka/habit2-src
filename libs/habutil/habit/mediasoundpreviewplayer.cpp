#include "mediasoundpreviewplayer.h"
#include <Phonon/AudioOutput>
#include <Phonon/MediaObject>
#include <QtCore/QFileInfo>

MediaSoundPreviewPlayer::MediaSoundPreviewPlayer(QWidget *parent)
	: QWidget(parent)
{
	_mediaObject1 = new Phonon::MediaObject(this);
	_mediaObject2 = new Phonon::MediaObject(this);
	_mediaObject3 = new Phonon::MediaObject(this);
	_mediaObject4 = new Phonon::MediaObject(this);
	_audioOutput1 = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	_audioOutput2 = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	_audioOutput3= new Phonon::AudioOutput(Phonon::VideoCategory, this);
	_audioOutput4= new Phonon::AudioOutput(Phonon::VideoCategory, this);
	Phonon::createPath(_mediaObject1, _audioOutput1);
	Phonon::createPath(_mediaObject2, _audioOutput2);
	Phonon::createPath(_mediaObject3, _audioOutput3);
	Phonon::createPath(_mediaObject4, _audioOutput4);
	_mediaWait1 = _mediaWait2 = _mediaWait3 = _mediaWait4 = false;
	_mediaLoaded1 = _mediaLoaded2 = _mediaLoaded3 = _mediaLoaded4 = false;
	_playRequest = false;
	connect(_mediaObject1, SIGNAL(stateChanged (Phonon::State, Phonon::State)), this, SLOT(onMedia1StateChanged(Phonon::State, Phonon::State)));
	connect(_mediaObject2, SIGNAL(stateChanged (Phonon::State, Phonon::State)), this, SLOT(onMedia2StateChanged(Phonon::State, Phonon::State)));
	connect(_mediaObject3, SIGNAL(stateChanged (Phonon::State, Phonon::State)), this, SLOT(onMedia3StateChanged(Phonon::State, Phonon::State)));
	connect(_mediaObject4, SIGNAL(stateChanged (Phonon::State, Phonon::State)), this, SLOT(onMedia4StateChanged(Phonon::State, Phonon::State)));
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
			try
			{
			_mediaObject1->clear();
			}
			catch (...)
			{

			}
			_mediaObject1->setCurrentSource(Phonon::MediaSource(mediaSrc1));
		
		
	}
	if (QFileInfo(mediaSrc2).exists() && isMediaPlayable(mediaSrc2))
	{
		
		_mediaLoaded2 = false;
		_mediaWait2 = true;
		try{
		_mediaObject2->clear();
		}
		catch(...)
		{

		}
		_mediaObject2->setCurrentSource(Phonon::MediaSource(mediaSrc2));
		
	}
	if (QFileInfo(mediaSrc3).exists() && isMediaPlayable(mediaSrc3))
	{
		
		_mediaLoaded3 = false;
		_mediaWait3 = true;
		try {
		_mediaObject3->clear();
		}
		catch(...)
		{

		}
		_mediaObject3->setCurrentSource(Phonon::MediaSource(mediaSrc3));
		
	}
	if (QFileInfo(mediaSrc4).exists() && isMediaPlayable(mediaSrc4))
	{
		
		_mediaLoaded4 = false;
		_mediaWait4 = true;
		try {
		_mediaObject4->clear();
		}
		catch(...)
		{

		}
		_mediaObject4->setCurrentSource(Phonon::MediaSource(mediaSrc4));
		
	}
}

void MediaSoundPreviewPlayer::setVolumes( double volume1, double volume2, double volume3, double volume4 )
{
	_audioOutput1->setVolume(volume1/100.0);
	_audioOutput2->setVolume(volume2/100.0);
	_audioOutput3->setVolume(volume3/100.0);
	_audioOutput4->setVolume(volume4/100.0);
}

void MediaSoundPreviewPlayer::onMedia1StateChanged( Phonon::State old, Phonon::State state )
{
//	qDebug() << "stateold: " << old << " statenew:" << state;
	if ((old == Phonon::StoppedState) && (state == Phonon::LoadingState))
	{
		_mediaLoaded1 = true;
		onLoadingComplete();
	}
}

void MediaSoundPreviewPlayer::onMedia2StateChanged( Phonon::State old, Phonon::State state )
{
//	qDebug() << "stateold: " << old << " statenew:" << state;
	if ((old == Phonon::StoppedState) && (state == Phonon::LoadingState))
	{
		_mediaLoaded2 = true;
		onLoadingComplete();
	}
}

void MediaSoundPreviewPlayer::onMedia3StateChanged( Phonon::State old, Phonon::State state )
{
	if ((old == Phonon::StoppedState) && (state == Phonon::LoadingState))
	{
		_mediaLoaded3 = true;
		onLoadingComplete();
	}
}

void MediaSoundPreviewPlayer::onMedia4StateChanged( Phonon::State old, Phonon::State state )
{
	if ((old == Phonon::StoppedState) && (state == Phonon::LoadingState))
	{
		_mediaLoaded4 = true;
		onLoadingComplete();
	}
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
			_mediaObject1->play();
		if (_mediaLoaded2)
			_mediaObject2->play();
		if (_mediaLoaded3)
			_mediaObject3->play();
		if (_mediaLoaded4)
			_mediaObject4->play();
	}
}

void MediaSoundPreviewPlayer::playMedias()
{
	_playRequest = true;
	onLoadingComplete();
}

void MediaSoundPreviewPlayer::stopMedias()
{
	_mediaObject1->clear();
	_mediaObject2->clear();
	_mediaObject3->clear();
	_mediaObject4->clear();
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
