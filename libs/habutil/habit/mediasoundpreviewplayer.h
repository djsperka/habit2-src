#ifndef MEDIASOUNDPREVIEWPLAYER_H
#define MEDIASOUNDPREVIEWPLAYER_H

#include <QtGui/QWidget>
#include <QtCore/QTime>
#include <Phonon/MediaObject>

namespace Phonon {
	class MediaObject;
	class VideoWidget;
	class AudioOutput;
}

class MediaSoundPreviewPlayer : public QWidget
{
	Q_OBJECT

public:
	
	MediaSoundPreviewPlayer(QWidget *parent);
	~MediaSoundPreviewPlayer();
	void setMedias(QString mediaSrc1, QString mediaSrc2, QString mediaSrc3, QString mediaSrc4);
	void setVolumes(double volume1, double volume2, double volume3, double volume4 );
	void playMedias();
	void stopMedias();
protected slots:
	void onMedia1StateChanged( Phonon::State, Phonon::State );
	void onMedia2StateChanged( Phonon::State, Phonon::State );
	void onMedia3StateChanged( Phonon::State, Phonon::State );
	void onMedia4StateChanged( Phonon::State, Phonon::State );
private:
	void onLoadingComplete();
	bool isMediaPlayable(const QString& filename);

private:
	Phonon::MediaObject * _mediaObject1;
	Phonon::MediaObject * _mediaObject2;
	Phonon::MediaObject * _mediaObject3;
	Phonon::MediaObject * _mediaObject4;
	Phonon::AudioOutput * _audioOutput1;
	Phonon::AudioOutput * _audioOutput2;
	Phonon::AudioOutput * _audioOutput3;
	Phonon::AudioOutput * _audioOutput4;
	bool _mediaLoaded1;
	bool _mediaLoaded2;
	bool _mediaLoaded3;
	bool _mediaLoaded4;
	bool _mediaWait1;
	bool _mediaWait2;
	bool _mediaWait3;
	bool _mediaWait4;
	bool _playRequest;
};

#endif // MEDIASOUNDPREVIEWPLAYER_H
