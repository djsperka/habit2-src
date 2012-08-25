/*
 *  HabitVideoImagePlayer.h
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 Center for Neuroscience. All rights reserved.
 *
 */

#ifndef HABITVIDEOIMAGEPLAYER_H
#define HABITVIDEOIMAGEPLAYER_H

#include <QtGui/QLabel>
#include <QList>
#include <QTextStream>
#include <QKeyEvent>
#include <Phonon/MediaObject>
#include <Phonon/VideoWidget>
#include <Phonon/AudioOutput>
#include <Phonon/MediaSource>
#include "ImageWidget.h"
#include "StimulusSource.h"
#include "HabitPlayer.h"

class HabitVideoImagePlayer : public HabitPlayer
{
	Q_OBJECT
	
public:
	HabitVideoImagePlayer(int id = 0, QWidget* w = 0, bool fullscreen = true, bool maintainAspectRatio = true);
	~HabitVideoImagePlayer() {};
	virtual void play(int number);
	virtual void stop(); 
	friend QTextStream& operator<<(QTextStream& out, const HabitVideoImagePlayer& player);
	
protected:
	void keyPressEvent(QKeyEvent* event);
	
private:
	Phonon::MediaObject *m_pMediaObject;
	Phonon::VideoWidget *m_pVideoWidget;
	Phonon::AudioOutput *m_pAudioOutput;
	ImageWidget *m_pImageWidget;
	bool m_isFullScreen;
	bool m_maintainAspectRatio;

signals:
	void started(int);
	
public slots:
	void onPrefinishMarkReached(qint32);
	void onStateChanged(Phonon::State newState, Phonon::State oldState);
	void onImagePainted();
};

#endif