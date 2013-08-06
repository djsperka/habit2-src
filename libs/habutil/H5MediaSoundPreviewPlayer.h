/*
 * H5MediaSoundPreviewPlayer.h
 *
 *  Created on: Jul 26, 2013
 *      Author: Oakeslab
 */

#ifndef H5MEDIASOUNDPREVIEWPLAYER_H_
#define H5MEDIASOUNDPREVIEWPLAYER_H_

#include <QtWidgets>
#include <QTime>
#include <QMediaPlayer>

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
	void onMedia1StateChanged( QMediaPlayer::State );
	void onMedia2StateChanged( QMediaPlayer::State );
	void onMedia3StateChanged( QMediaPlayer::State );
	void onMedia4StateChanged( QMediaPlayer::State );
private:
	void onLoadingComplete();
	bool isMediaPlayable(const QString& filename);

private:
	QMediaPlayer* _pMediaPlayer1;
	QMediaPlayer* _pMediaPlayer2;
	QMediaPlayer* _pMediaPlayer3;
	QMediaPlayer* _pMediaPlayer4;
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


#endif /* H5MEDIASOUNDPREVIEWPLAYER_H_ */
