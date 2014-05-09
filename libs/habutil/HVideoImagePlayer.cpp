/*
 *  HVideoImagePlayer.cpp
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 Center for Neuroscience. All rights reserved.
 *
 */

#include "HVideoImagePlayer.h"
#include <QFocusEvent>
#include <QApplication>

bool HVideoImagePlayer::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::ActivationChange)
	{
		qDebug() << "HVideoImagePlayer: activation change ignored.";
		event->ignore();
	}
	if (event->type() == QEvent::KeyPress)
	{
		qDebug() << "HVideoImagePlayer: Got KeyPress Event " << object->metaObject()->className();
		if (this->parent())
			qDebug() << "HVideoImagePlayer: parent is " << this->parent()->metaObject()->className();
		else 
			qDebug() << "HVideoImagePlayer no parent!";
	}		
	if (event->type() == QEvent::KeyRelease)
	{
		qDebug() << "HVideoImagePlayer: Got KeyRelease Event " << object->metaObject()->className();
	}		
    if (event->type() == QEvent::FocusIn)
    {
		QFocusEvent* f = dynamic_cast<QFocusEvent*> (event);
		qDebug("Got FocusIn Event");
        if (object == m_pImageWidget)
        {
            qDebug() << "ImageWidget FocusIn " << f->reason();
        }
		else if (object == m_pVideoWidget)
		{
            qDebug() << "VideoWidget FocusIn " << f->reason();
        }
		else if (object == this)
		{
            qDebug() << "VideoImagePlayer FocusIn " << f->reason();
        }
		
    }
    else if (event->type() == QEvent::FocusOut)
    {
		QFocusEvent* f = dynamic_cast<QFocusEvent*> (event);
		qDebug("Got FocusOut Event");		
        if (object == m_pImageWidget)
        {
            qDebug() << "ImageWidget FocusOut " << f->reason();
        }
		else if (object == m_pVideoWidget)
		{
            qDebug() << "VideoWidget FocusOut " << f->reason();
        }
		else if (object == this)
		{
            qDebug() << "VideoImagePlayer FocusOut " << f->reason();
        }
		
    }
    return false;
}


HVideoImagePlayer::HVideoImagePlayer(int id, QWidget *w, bool fullscreen, bool maintainAspectRatio) : 
HPlayer(id, w), m_pendingClear(false), m_parent(w), m_pMediaObject(0), m_pVideoWidget(0), m_pAudioOutput(0), m_pImageWidget(0), m_isFullScreen(fullscreen), m_maintainAspectRatio(maintainAspectRatio), m_nowPlayingFilename("NONE")
{
	// This combination needed to get the "close window when app exits"
	// right. Make sure to call with the parent as the thing that should 
	// kill this window when closed. 
	
	setWindowFlags(Qt::Window);
	showFullScreen();
	
	// Generate image widget, media object, video widget, audio output
	// Special case is when this player is for audio only stimuli (as for control player and ISS stim)
	
	m_pImageWidget = new HImageWidget(this, true, true);
	m_pImageWidget->setObjectName("ImageWidget");
	m_pMediaObject = new Phonon::MediaObject(this);
	m_pMediaObject->setObjectName("MediaObject");
	m_pVideoWidget = new Phonon::VideoWidget(this);
	m_pVideoWidget->setObjectName("VideoWidget");
	m_pAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);
	
	// connect media object slot to handle looped video when needed
	
	connect(m_pMediaObject, SIGNAL(prefinishMarkReached(qint32)), this, SLOT(onPrefinishMarkReached(qint32)));
	
	// connect media object's stateChanged signal so we can emit started() and stopped() signals.
	
	connect(m_pMediaObject, SIGNAL(stateChanged (Phonon::State, Phonon::State)), 
			this, SLOT(onStateChanged(Phonon::State, Phonon::State)));
	
	
	// connect image widget's painted() signal to our onImagePainted() slot so we can emit started() signal. 
	// TODO: will we need stopped signal from image widget?
	connect(m_pImageWidget, SIGNAL(painted()), this, SLOT(onImagePainted()));

											
	// Both widgets initially hidden
	
	m_pVideoWidget->hide();
	m_pImageWidget->hide();
	
	m_pVideoWidget->setScaleMode(Phonon::VideoWidget::FitInView);
	if (maintainAspectRatio)
	{
		m_pVideoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
	}
	else
	{
		m_pVideoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
	}
	
	// Create paths
	Phonon::createPath(m_pMediaObject, m_pVideoWidget);
	Phonon::createPath(m_pMediaObject, m_pAudioOutput);
	
}


HVideoImagePlayer::~HVideoImagePlayer()
{
	qDebug("HVideoImagePlayer::~HVideoImagePlayer()");
	m_pVideoWidget->hide();
	m_pVideoWidget->close();
    delete m_pVideoWidget;
    m_pVideoWidget = 0;
	
    delete m_pAudioOutput;
    m_pAudioOutput = 0;
	
    delete m_pMediaObject;
    m_pMediaObject = 0;
}



void HVideoImagePlayer::onStateChanged(Phonon::State newState, Phonon::State oldState)
{
	Q_UNUSED(oldState);
	if (newState == Phonon::PlayingState)
	{
		emit started(m_id, m_nowPlayingFilename);
	}		
	return;
}

void HVideoImagePlayer::onImagePainted()
{
	if (m_pendingClear)
	{
		m_pendingClear = false;
		emit cleared(m_id);
	}
	else 
	{
		emit started(m_id, m_nowPlayingFilename);
	}
	return;
}
		
void HVideoImagePlayer::stop()
{
	m_pMediaObject->stop();
}

void HVideoImagePlayer::clear()
{
	if (getCurrentStimulusType() == HStimulusSource::BACKGROUND)
	{
		emit cleared(m_id);
	}
	else 
	{
		m_pendingClear = true;
		play(-1);
	}
}

void HVideoImagePlayer::play(unsigned int number)
{
	HStimulusSource::HStimulusSourceType newType = getStimulusType(number);
	switch (getCurrentStimulusType()) 
	{
		case HStimulusSource::BACKGROUND:
			// Init state, or background state
			switch (newType) {
				case HStimulusSource::VIDEO:
					m_pMediaObject->setCurrentSource(m_sources.value(number)->filename());
					m_pVideoWidget->setGeometry(geometry());
					m_pVideoWidget->show();
					m_pAudioOutput->setVolume(m_sources.value(number)->getAudioBalance());
					m_pImageWidget->hide();
					m_pMediaObject->play();
					m_pVideoWidget->setFullScreen(m_isFullScreen);
					m_nowPlayingFilename = (m_sources.value(number))->filename();
					break;
				case HStimulusSource::IMAGE:
					m_pImageWidget->setGeometry(QRect(0, 0, geometry().width(), geometry().height()));
					m_pImageWidget->show();	
					m_pImageWidget->setCurrentSource((m_sources.value(number))->filename());
					m_nowPlayingFilename = (m_sources.value(number))->filename();
					break;
				default:
					break;
			}
			break;
		case HStimulusSource::VIDEO:
			// showing video - stop it
			stop();
			switch (newType) {
				case HStimulusSource::BACKGROUND:
					m_pVideoWidget->hide();
					m_nowPlayingFilename = "NONE";
					break;
				case HStimulusSource::VIDEO:
					m_pMediaObject->setCurrentSource((m_sources[number]->filename()));
					m_pVideoWidget->setGeometry(geometry());
					m_pVideoWidget->show();
					m_pAudioOutput->setVolume(m_sources.value(number)->getAudioBalance());
					m_pImageWidget->hide();
					m_pMediaObject->play();
					m_pVideoWidget->setFullScreen(m_isFullScreen);
					m_nowPlayingFilename = (m_sources.value(number))->filename();
					break;
				case HStimulusSource::IMAGE:
					m_pVideoWidget->hide();
					m_pImageWidget->setGeometry(QRect(0, 0, geometry().width(), geometry().height()));
					m_pImageWidget->show();	
					m_pImageWidget->setCurrentSource((m_sources.value(number))->filename());
					m_nowPlayingFilename = (m_sources.value(number))->filename();
					break;					
				default:
					break;
			}
			break;
		case HStimulusSource::IMAGE:
			// showing image
			switch (newType) {
				case HStimulusSource::BACKGROUND:
					m_pImageWidget->hide();
					m_nowPlayingFilename = "NONE";
					break;
				case HStimulusSource::VIDEO:
					m_pMediaObject->setCurrentSource((m_sources[number]->filename()));
					m_pVideoWidget->setGeometry(geometry());
					m_pImageWidget->hide();
					m_pVideoWidget->show();
					m_pAudioOutput->setVolume(m_sources.value(number)->getAudioBalance());
					m_pMediaObject->play();
					m_pVideoWidget->setFullScreen(m_isFullScreen);
					if (m_parent) m_parent->activateWindow(); //Hack Alert!
					m_nowPlayingFilename = (m_sources.value(number))->filename();
					break;
				case HStimulusSource::IMAGE:
					m_pImageWidget->setCurrentSource((m_sources.value(number))->filename());
					m_nowPlayingFilename = (m_sources.value(number))->filename();
					break;
				default:
					break;
			}
			break;
		default:
			break;		
	}
	m_iCurrentStim = number;
	
}

void HVideoImagePlayer::onPrefinishMarkReached(qint32 msec)
{
	Q_UNUSED(msec);
	if (m_iCurrentStim > -1 && m_iCurrentStim < m_sources.count() && (m_sources.value(m_iCurrentStim))->type() == HStimulusSource::VIDEO && (m_sources.value(m_iCurrentStim))->isLooped())
	{
		m_pMediaObject->pause();
		m_pMediaObject->seek(0);
		m_pMediaObject->play();
	}
}

