/*
 *  HAudioPlayer.h
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HABITAUDIOPLAYER_H
#define HABITAUDIOPLAYER_H


#include <QtWidgets>
#include <QList>
#include <QTextStream>
#ifdef USING_QT5
#include <QMediaPlayer>
#else
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#endif
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
#ifdef USING_QT5
	QMediaPlayer *m_pMediaPlayer;
	QVideoWidget *m_pVideoWidget;
#else
	Phonon::MediaObject *m_pMediaObject;
	Phonon::VideoWidget *m_pVideoWidget;
	Phonon::AudioOutput *m_pAudioOutput;
#endif

public slots:
	public slots:
		void onPrefinishMarkReached(qint32);
	#ifdef USING_QT5
		void onStateChanged(QMediaPlayer::State newState);
	#else
		void onStateChanged(Phonon::State newState, Phonon::State oldState);
	#endif
	
};

#endif
