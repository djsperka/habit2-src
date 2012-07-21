/*
 *  HabitAudioPlayer.h
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
#include "StimulusSource.h"
#include "HabitPlayer.h"

class HabitAudioPlayer : public HabitPlayer
{
	Q_OBJECT
	
public:
	HabitAudioPlayer(int id = 0, QWidget* w = 0);
	~HabitAudioPlayer() {};
	virtual void play(int number);
	virtual void stop(); 
	friend QTextStream& operator<<(QTextStream& out, const HabitAudioPlayer& player);
	
private:
	Phonon::MediaObject *m_pMediaObject;
	Phonon::AudioOutput *m_pAudioOutput;

public slots:
	void onPrefinishMarkReached(qint32);
	void onStateChanged(Phonon::State newState, Phonon::State oldState);
	
};

#endif