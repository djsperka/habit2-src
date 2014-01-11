/*
 * HVIPlayer.h
 *
 *  Created on: Nov 15, 2013
 *      Author: Oakeslab
 */

#ifndef HVIPLAYER_H_
#define HVIPLAYER_H_

#include <QtGui>
#include <QList>
#include <QTextStream>
#include <QKeyEvent>
#include <Phonon/MediaObject>
#include <Phonon/VideoWidget>
#include <Phonon/AudioOutput>
#include <Phonon/MediaSource>
#include "HImageWidget.h"
#include "HStimulusSource.h"
#include "HPlayer.h"


// This class implements the HPlayer interface to play images and video stimuli.
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



class HVIPlayer : public HPlayer
{
	Q_OBJECT

public:
	HVIPlayer(int id = 0, QWidget* w = 0, bool fullscreen = true, bool maintainAspectRatio = true);
	~HVIPlayer();
	virtual void play(int number);
	virtual void stop();
	virtual void clear();
	friend QTextStream& operator<<(QTextStream& out, const HVIPlayer& player);
	//bool eventFilter(QObject *object, QEvent *event);

protected:

private:
	bool m_pendingClear;
	QWidget* m_parent;
	QStackedLayout *m_pStackedLayout;
	Phonon::MediaObject *m_pMediaObject;
	Phonon::VideoWidget *m_pVideoWidget;
	Phonon::AudioOutput *m_pAudioOutput;
	HImageWidget *m_pImageWidget;
	bool m_isFullScreen;
	bool m_maintainAspectRatio;
	QString m_nowPlayingFilename;

//signals:
//	void started(int);

public slots:
	void onPrefinishMarkReached(qint32);
	void onStateChanged(Phonon::State newState, Phonon::State oldState);
	void onImagePainted();
};




#endif /* HVIPLAYER_H_ */
