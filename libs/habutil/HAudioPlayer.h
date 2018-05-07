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


#include <QLabel>
#include <QList>
#include <QTextStream>
#include <QMap>
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>
#include "HStimulusSource.h"
#include "HPlayer.h"

class HAudioPlayer : public HPlayer
{
	Q_OBJECT

	
public:
	HAudioPlayer(int id = 0, QWidget* w = 0, const QDir& stimRootDir = QDir("/"));
	~HAudioPlayer();
	virtual void play(unsigned int number);
	virtual void stop(); 
	virtual void clear();
	friend QTextStream& operator<<(QTextStream& out, const HAudioPlayer& player);

	virtual void loadBuffer(unsigned int id);
	virtual void freeBuffer(unsigned int id);

	
private:
	bool m_pendingStop;
	Phonon::MediaObject *m_pMediaObject;
	Phonon::AudioOutput *m_pAudioOutput;
	QString m_nowPlayingFilename;
	QMap<unsigned int, HStimulusSource*> m_mapSources;


	unsigned int addStimulusPrivate(unsigned int id);

public slots:
	//void onPrefinishMarkReached(qint32);
	//void onStateChanged(Phonon::State newState, Phonon::State oldState);
	void mediaStatusChanged(QMediaPlayer::MediaStatus);

	
};

#endif
