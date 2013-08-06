/*
 * H5AudioPlayer.h
 *
 *  Created on: Jul 26, 2013
 *      Author: Oakeslab
 */

#ifndef H5AUDIOPLAYER_H_
#define H5AUDIOPLAYER_H_

#include <QtWidgets>
#include <QList>
#include <QTextStream>
#include <QMediaPlayer>
#include "HStimulusSource.h"
#include "HPlayer.h"

class HAudioPlayer : public HPlayer
{
	Q_OBJECT

public:
	HAudioPlayer(int id = 0, QWidget* w = 0);
	~HAudioPlayer() {};
	virtual void play(int number);
	virtual void stop();
	virtual void clear();
	friend QTextStream& operator<<(QTextStream& out, const HAudioPlayer& player);

private:
	bool m_pendingStop;
	QMediaPlayer *m_pMediaPlayer;

public slots:
	public slots:
		void onPrefinishMarkReached(qint32);
		void onStateChanged(QMediaPlayer::State newState);

};

#endif /* H5AUDIOPLAYER_H_ */
