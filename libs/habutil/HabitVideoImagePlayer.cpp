/*
 *  HabitVideoImagePlayer.cpp
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 Center for Neuroscience. All rights reserved.
 *
 */

#include "HabitVideoImagePlayer.h"
#include <QFocusEvent>
#include <QApplication>

bool HabitVideoImagePlayer::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::ActivationChange)
	{
		qDebug() << "HabitVideoImagePlayer: activation change ignored.";
		event->ignore();
	}
	if (event->type() == QEvent::KeyPress)
	{
		qDebug() << "HabitVideoImagePlayer: Got KeyPress Event " << object->metaObject()->className();
		if (this->parent())
			qDebug() << "HabitVideoImagePlayer: parent is " << this->parent()->metaObject()->className();
		else 
			qDebug() << "HabitVideoImagePlayer no parent!";
	}		
	if (event->type() == QEvent::KeyRelease)
	{
		qDebug() << "HabitVideoImagePlayer: Got KeyRelease Event " << object->metaObject()->className();
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


HabitVideoImagePlayer::HabitVideoImagePlayer(int id, QWidget *w, bool fullscreen, bool maintainAspectRatio) : 
HabitPlayer(id, w), m_parent(w), m_pMediaObject(0), m_pVideoWidget(0), m_pAudioOutput(0), m_pImageWidget(0), m_isFullScreen(fullscreen), m_maintainAspectRatio(maintainAspectRatio)
{
	// This combination needed to get the "close window when app exits"
	// right. Make sure to call with the parent as the thing that should 
	// kill this window when closed. 
	
	setObjectName("HabitVideoImagePlayer");
	setWindowFlags(Qt::Window);
	showFullScreen();
	
	// Generate image widget, media object, video widget, audio output
	// Special case is when this player is for audio only stimuli (as for control player and ISS stim)
	
	m_pImageWidget = new ImageWidget(this, true, true);
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

void HabitVideoImagePlayer::onStateChanged(Phonon::State newState, Phonon::State oldState)
{
	Q_UNUSED(oldState);
	if (newState == Phonon::PlayingState)
	{
		emit started(m_id);
	}		
	return;
}

void HabitVideoImagePlayer::onImagePainted()
{
	emit started(m_id);
	return;
}
		
void HabitVideoImagePlayer::stop()
{
	m_pMediaObject->stop();
}

void HabitVideoImagePlayer::play(int number)
{
	StimulusSource::StimulusSourceType newType = getStimulusType(number);
	switch (getCurrentStimulusType()) 
	{
		case StimulusSource::BACKGROUND:
			// Init state, or background state
			switch (newType) {
				case StimulusSource::VIDEO:
					m_pMediaObject->setCurrentSource((m_sources.at(number)).filename());
					m_pVideoWidget->setGeometry(geometry());
					m_pVideoWidget->show();
					m_pAudioOutput->setVolume(m_sources.at(number).getAudioBalance());
					m_pImageWidget->hide();
					m_pMediaObject->play();
					m_pVideoWidget->setFullScreen(m_isFullScreen);
					break;
				case StimulusSource::IMAGE:
					m_pImageWidget->setGeometry(QRect(0, 0, geometry().width(), geometry().height()));
					m_pImageWidget->show();	
					m_pImageWidget->setCurrentSource((m_sources.at(number)).filename());
					break;
				default:
					break;
			}
			break;
		case StimulusSource::VIDEO:
			// showing video - stop it
			stop();
			switch (newType) {
				case StimulusSource::BACKGROUND:
					m_pVideoWidget->hide();
					break;
				case StimulusSource::VIDEO:
					m_pMediaObject->setCurrentSource((m_sources[number].filename()));
					m_pVideoWidget->setGeometry(geometry());
					m_pVideoWidget->show();
					m_pAudioOutput->setVolume(m_sources.at(number).getAudioBalance());
					m_pImageWidget->hide();
					m_pMediaObject->play();
					m_pVideoWidget->setFullScreen(m_isFullScreen);
					break;
				case StimulusSource::IMAGE:
					m_pVideoWidget->hide();
					m_pImageWidget->setGeometry(QRect(0, 0, geometry().width(), geometry().height()));
					m_pImageWidget->show();	
					m_pImageWidget->setCurrentSource((m_sources.at(number)).filename());
					break;					
				default:
					break;
			}
			break;
		case StimulusSource::IMAGE:
			// showing image
			switch (newType) {
				case StimulusSource::BACKGROUND:
					m_pImageWidget->hide();
					break;
				case StimulusSource::VIDEO:
					m_pMediaObject->setCurrentSource((m_sources[number].filename()));
					m_pVideoWidget->setGeometry(geometry());
					m_pImageWidget->hide();
					m_pVideoWidget->show();
					m_pAudioOutput->setVolume(m_sources.at(number).getAudioBalance());
					m_pMediaObject->play();
					m_pVideoWidget->setFullScreen(m_isFullScreen);
					m_parent->activateWindow(); //Hack Alert!
					break;
				case StimulusSource::IMAGE:
					m_pImageWidget->setCurrentSource((m_sources.at(number)).filename());
					break;
				default:
					break;
			}
		default:
			break;		
	}
	m_iCurrentStim = number;
	
}

void HabitVideoImagePlayer::onPrefinishMarkReached(qint32 msec)
{
	Q_UNUSED(msec);
	if (m_iCurrentStim > -1 && m_iCurrentStim < m_sources.count() && (m_sources.at(m_iCurrentStim)).type() == StimulusSource::VIDEO && (m_sources.at(m_iCurrentStim)).isLooped())
	{
		m_pMediaObject->pause();
		m_pMediaObject->seek(0);
		m_pMediaObject->play();
	}
}

