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


#include <QtGui/QLabel>
#include <QList>
#include <QTextStream>
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
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
	Phonon::MediaObject *m_pMediaObject;
	Phonon::AudioOutput *m_pAudioOutput;
	QString m_nowPlayingFilename;

public slots:
	void onPrefinishMarkReached(qint32);
	void onStateChanged(Phonon::State newState, Phonon::State oldState);
	
};

#endif
