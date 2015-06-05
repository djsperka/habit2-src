/*
 * HVIPlayer.cpp
 *
 *  Created on: Nov 15, 2013
 *      Author: Oakeslab
 */

#include "HVIPlayer.h"
#include "stimulusinfo.h"

using namespace Habit;

HVIPlayer::HVIPlayer(int id, QWidget *w, const QDir& stimRootDir, bool fullscreen, QColor background, bool maintainAspectRatio) :
HPlayer(id, w, stimRootDir), m_pendingClear(false), m_parent(w), m_pMediaObject(0), m_pVideoWidget(0), m_pAudioOutput(0), m_pImageWidget(0), m_isFullScreen(fullscreen), m_maintainAspectRatio(maintainAspectRatio), m_nowPlayingFilename("NONE")
{
	// This combination needed to get the "close window when app exits"
	// right. Make sure to call with the parent as the thing that should
	// kill this window when closed.

	// djs These calls can be made in HMediaManager.
	// That class changes the size and moves this widget when a player is added.
	//setWindowFlags(Qt::Window);
	//showFullScreen();

	// Create a stacked layout. Add the VideoWidget and the ImageWidget to it.
	m_pStackedLayout = new QStackedLayout;
	m_pStackedLayout->setContentsMargins(QMargins(0, 0, 0, 0));
	setLayout(m_pStackedLayout);

	// Generate image widget, media object, video widget, audio output
	// Each widget is added to the stacked widget.

	QPalette palette;
	palette.setColor(QPalette::Window, background);
	QLabel *label = new QLabel();
	label->setAutoFillBackground(true);
	label->setPalette(palette);
	m_backgroundIndex = m_pStackedLayout->addWidget(label);

	m_pImageWidget = new HImageWidget(NULL, true, true, background);
	m_imageIndex = m_pStackedLayout->addWidget(m_pImageWidget);

	m_pMediaObject = new Phonon::MediaObject(this);
	m_pVideoWidget = new Phonon::VideoWidget;
	m_videoIndex = m_pStackedLayout->addWidget(m_pVideoWidget);

	m_pAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);

	// connect media object slot to handle looped video when needed

	connect(m_pMediaObject, SIGNAL(prefinishMarkReached(qint32)), this, SLOT(onPrefinishMarkReached(qint32)));

	// connect media object's stateChanged signal so we can emit started() and stopped() signals.

	connect(m_pMediaObject, SIGNAL(stateChanged (Phonon::State, Phonon::State)),
			this, SLOT(onStateChanged(Phonon::State, Phonon::State)));


	// connect image widget's painted() signal to our onImagePainted() slot so we can emit started() signal.
	// TODO: will we need stopped signal from image widget?
	connect(m_pImageWidget, SIGNAL(painted()), this, SLOT(onImagePainted()));

	// connect layout's currentChanged signal to our onCurrentChanged() slot so we can emit started() and cleared() signal
	// for the background widget
	connect(m_pStackedLayout, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));


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


HVIPlayer::~HVIPlayer()
{
}


void HVIPlayer::onStateChanged(Phonon::State newState, Phonon::State oldState)
{
	Q_UNUSED(oldState);

	//QSize s = sizeHint();
	//qDebug() << (int)oldState << "->" << (int)newState << " " << s.width() << "x" << s.height();

	if (newState == Phonon::PlayingState)
	{
		emit started(m_id, m_nowPlayingFilename);
	}

	return;
}

void HVIPlayer::onImagePainted()
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

void HVIPlayer::onCurrentChanged(int index)
{
	// if the index is the background index, emit started()
	if (index == m_backgroundIndex)
	{
//		qDebug() << "HVIPlayer::onCurrentChanged(" << index << ") emit started(" << m_id << ", " << m_nowPlayingFilename << ")";
		emit started(m_id, m_nowPlayingFilename);
	}
//	else
//	{
//		qDebug() << "HVIPlayer::onCurrentChanged(" << index << ") not background index " << m_backgroundIndex;
//	}
}

void HVIPlayer::stop()
{
	if (getCurrentStimulusType() == HStimulusSource::VIDEO)
		m_pMediaObject->stop();
}

void HVIPlayer::clear()
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


HStimulusSource::HStimulusSourceType HVIPlayer::getStimulusType(unsigned int index)
{
	HStimulusSource::HStimulusSourceType type = HStimulusSource::BACKGROUND;
	if (m_mapSources.contains(index))
		type = m_mapSources.value(index)->type();
	return type;
}


void HVIPlayer::play(unsigned int number)
{
	HStimulusSource::HStimulusSourceType oldType = getStimulusType(m_iCurrentStim);
	HStimulusSource::HStimulusSourceType newType = getStimulusType(number);
	HStimulusSource *s;	// Beware! play() can be called with number = -1 (for background). Must not fetch source in that case.


	// Stop video if one is now playing.
	if (oldType == HStimulusSource::VIDEO)
	{
		stop();
	}

	switch (newType)
	{
		case HStimulusSource::VIDEO:
			s = m_mapSources.value(number);
			m_nowPlayingFilename = s->filename();
			if (s->hasBuffer())
			{
				m_pMediaObject->setCurrentSource(*(new Phonon::MediaSource(s->buffer())));
			}
			else
			{
				m_pMediaObject->setCurrentSource(s->filename());
			}
			m_pAudioOutput->setVolume((double)s->getAudioBalance()/100.0);
			m_pMediaObject->play();
			m_pStackedLayout->setCurrentIndex(m_videoIndex);
			break;
		case HStimulusSource::IMAGE:
			s = m_mapSources.value(number);
			m_nowPlayingFilename = s->filename();
			m_pImageWidget->setGeometry(QRect(0, 0, geometry().width(), geometry().height()));
			m_pImageWidget->setCurrentSource(s->filename());
			m_pStackedLayout->setCurrentIndex(m_imageIndex);
			break;
		case HStimulusSource::BACKGROUND:
			m_nowPlayingFilename = QString("Background");
			m_pStackedLayout->setCurrentIndex(m_backgroundIndex);
			break;
		default:
			m_nowPlayingFilename = "UNKNOWN";
			break;
	}
	m_iCurrentStim = number;

}


void HVIPlayer::onPrefinishMarkReached(qint32 msec)
{
	Q_UNUSED(msec);
	if (m_iCurrentStim > -1 && m_iCurrentStim < m_mapSources.count() && (m_mapSources.value(m_iCurrentStim))->type() == HStimulusSource::VIDEO && (m_mapSources.value(m_iCurrentStim))->isLooped())
	{
		m_pMediaObject->pause();
		m_pMediaObject->seek(0);
		m_pMediaObject->play();
	}
}

unsigned int HVIPlayer::addStimulusPrivate(const unsigned int id)
{
	const Habit::StimulusInfo& info = getStimulusInfo(id);
	HStimulusSource* s = new HStimulusSource(&info, getStimulusRoot());
	m_mapSources.insert(id, s);
	qDebug() << "HVIPlayer::addStimulusPrivate(" << id << ")";
	return id;
}
