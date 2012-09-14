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


// This class implements the HabitPlayer interface to play images and video stimuli. 
// It has an ugly hack which I found necessary - that is the usage of the parent widget 
// pointer. It appears that when this class plays a video at full screen, the VideoWidget
// object takes the focus. That's a problem when using this class in an experimental 
// context, because the users will expect that keyboard focus remains on the main experiment
// widget/dialog regardless of the image/video displayed. The users will use the keypad to 
// indicate where the subject is looking, and if the display of a particular video image
// moves the focus away the user may not notice (and look transitions may be missed), or 
// they will have to click on the dialog to move focus back. Hence, the implementation of 
// play() forces focus back to the widget 'w' passed here as an argument. None of this
// affects the display of the stimuli - they play at full screen and visually there is no 
// difference when the focus is on them or something else. 


class HabitVideoImagePlayer : public HabitPlayer
{
	Q_OBJECT
	
public:
	HabitVideoImagePlayer(int id = 0, QWidget* w = 0, bool fullscreen = true, bool maintainAspectRatio = true);
	~HabitVideoImagePlayer() {};
	virtual void play(int number);
	virtual void stop(); 
	friend QTextStream& operator<<(QTextStream& out, const HabitVideoImagePlayer& player);
	bool eventFilter(QObject *object, QEvent *event);

protected:
	
private:
	QWidget* m_parent;
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