/*
 * HVIPlayer.cpp
 *
 *  Created on: Nov 15, 2013
 *      Author: Oakeslab
 */

#include "HVIPlayer.h"

HVIPlayer::HVIPlayer(int id, QWidget *w, bool fullscreen, bool maintainAspectRatio) :
HPlayer(id, w), m_pendingClear(false), m_parent(w), m_pMediaObject(0), m_pVideoWidget(0), m_pAudioOutput(0), m_pImageWidget(0), m_isFullScreen(fullscreen), m_maintainAspectRatio(maintainAspectRatio)
{
	// This combination needed to get the "close window when app exits"
	// right. Make sure to call with the parent as the thing that should
	// kill this window when closed.

	// djs These calls can be made in HMediaManager.
	// That class changes the size and moves this widget when a player is added.
	//setWindowFlags(Qt::Window);
	showFullScreen();

	// The VideoWidget and ImageWidget and a background-color label widget share the stacked widget
//	m_pStackedWidget = new QStackedWidget;
//	QVBoxLayout *layout = new QVBoxLayout;
//	layout->setContentsMargins(QMargins(0,0,0,0));
//	layout->addWidget(m_pStackedWidget);
//	setLayout(layout);

	// Create a stacked layout. Add the VideoWidget and the ImageWidget to it.
	m_pStackedLayout = new QStackedLayout;
	m_pStackedLayout->setContentsMargins(QMargins(0, 0, 0, 0));
	setLayout(m_pStackedLayout);

	// Generate image widget, media object, video widget, audio output
	// Special case is when this player is for audio only stimuli (as for control player and ISS stim)
	// Each widget is added to the stacked widget. The order is important!

	m_pImageWidget = new HImageWidget(NULL, true, true);
	//m_pStackedWidget->addWidget(m_pImageWidget);
	m_pStackedLayout->addWidget(m_pImageWidget);

	m_pMediaObject = new Phonon::MediaObject(this);
	m_pVideoWidget = new Phonon::VideoWidget;
	//m_pStackedWidget->addWidget(m_pVideoWidget);
	m_pStackedLayout->addWidget(m_pVideoWidget);


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

	//m_pVideoWidget->hide();
	//m_pImageWidget->hide();

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
	qDebug("HVIPlayer::~HVIPlayer()");
	m_pVideoWidget->hide();
	m_pVideoWidget->close();
    delete m_pVideoWidget;
    m_pVideoWidget = 0;

    delete m_pAudioOutput;
    m_pAudioOutput = 0;

    delete m_pMediaObject;
    m_pMediaObject = 0;
}


void HVIPlayer::onStateChanged(Phonon::State newState, Phonon::State oldState)
{
	//Q_UNUSED(oldState);
	if (newState == Phonon::PlayingState)
	{
		emit started(m_id);
		QSize s = sizeHint();
		qDebug() << (int)oldState << "->" << "Playing" << " " << s.width() << "x" << s.height();
	}

	// test
	if (oldState == Phonon::LoadingState)
	{
		QSize s = sizeHint();
		qDebug() << "Loading->" << (int)newState << " " << s.width() << "x" << s.height();
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
		emit started(m_id);
	}
	return;
}


void HVIPlayer::stop()
{
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




void HVIPlayer::play(int number)
{
	HStimulusSource::HStimulusSourceType newType = getStimulusType(number);
	switch (newType)
	{
		case HStimulusSource::VIDEO:
			m_pMediaObject->setCurrentSource((m_sources.at(number)).filename());
			//m_pVideoWidget->setGeometry(geometry());
			qDebug() << "VIDEO player geom " << geometry().left() << "," << geometry().bottom() << "," << geometry().width() << "," << geometry().height();
			qDebug() << "VIDEO widget geom " << m_pVideoWidget->geometry().left() << "," << m_pVideoWidget->geometry().bottom() << "," << m_pVideoWidget->geometry().width() << "," << m_pVideoWidget->geometry().height();
			m_pAudioOutput->setVolume(m_sources.at(number).getAudioBalance());
			m_pMediaObject->play();
			m_pStackedLayout->setCurrentIndex(1);
			//m_pVideoWidget->setFullScreen(m_isFullScreen);
			break;
		case HStimulusSource::IMAGE:
			m_pImageWidget->setGeometry(QRect(0, 0, geometry().width(), geometry().height()));
			m_pImageWidget->setCurrentSource((m_sources.at(number)).filename());
			m_pStackedLayout->setCurrentIndex(0);
			qDebug() << "IMAGE player geom " << geometry().left() << "," << geometry().bottom() << "," << geometry().width() << "," << geometry().height();
			qDebug() << "IMAGE widget geom " << m_pImageWidget->geometry().left() << "," << m_pImageWidget->geometry().bottom() << "," << m_pImageWidget->geometry().width() << "," << m_pImageWidget->geometry().height();
			break;
		case HStimulusSource::BACKGROUND:
			m_pStackedLayout->setCurrentIndex(-1);
			break;
		default:
			break;
	}
	m_iCurrentStim = number;

}








#if 0
void HVIPlayer::play(int number)
{
	HStimulusSource::HStimulusSourceType newType = getStimulusType(number);
	switch (getCurrentStimulusType())
	{
		case HStimulusSource::BACKGROUND:
			// Init state, or background state
			switch (newType) {
				case HStimulusSource::VIDEO:
					m_pMediaObject->setCurrentSource((m_sources.at(number)).filename());
					m_pVideoWidget->setGeometry(geometry());
					qDebug() << "background-VIDEO geom " << geometry().left() << "," << geometry().bottom() << "," << geometry().width() << "," << geometry().height();
					qDebug() << "background-VIDEO geom " << m_pVideoWidget->geometry().left() << "," << m_pVideoWidget->geometry().bottom() << "," << m_pVideoWidget->geometry().width() << "," << m_pVideoWidget->geometry().height();
					m_pAudioOutput->setVolume(m_sources.at(number).getAudioBalance());
					m_pMediaObject->play();
					m_pStackedWidget->setCurrentIndex(1);
					//m_pVideoWidget->setFullScreen(m_isFullScreen);
					break;
				case HStimulusSource::IMAGE:
					m_pImageWidget->setGeometry(QRect(0, 0, geometry().width(), geometry().height()));
					m_pImageWidget->setCurrentSource((m_sources.at(number)).filename());
					m_pStackedWidget->setCurrentIndex(0);
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
					break;
				case HStimulusSource::VIDEO:
					m_pMediaObject->setCurrentSource((m_sources[number].filename()));
					m_pVideoWidget->setGeometry(geometry());
					qDebug() << "VIDEO-VIDEO geom " << geometry().left() << "," << geometry().bottom() << "," << geometry().width() << "," << geometry().height();
					qDebug() << "VIDEO-VIDEO geom " << m_pVideoWidget->geometry().left() << "," << m_pVideoWidget->geometry().bottom() << "," << m_pVideoWidget->geometry().width() << "," << m_pVideoWidget->geometry().height();
					m_pAudioOutput->setVolume(m_sources.at(number).getAudioBalance());
					m_pMediaObject->play();
					m_pStackedWidget->setCurrentIndex(1);	// no need, really
					//m_pVideoWidget->setFullScreen(m_isFullScreen);
					break;
				case HStimulusSource::IMAGE:
					m_pImageWidget->setGeometry(QRect(0, 0, geometry().width(), geometry().height()));
					m_pStackedWidget->setCurrentIndex(0);
					m_pImageWidget->setCurrentSource((m_sources.at(number)).filename());
					break;
				default:
					break;
			}
			break;
		case HStimulusSource::IMAGE:
			// showing image
			switch (newType) {
				case HStimulusSource::BACKGROUND:
					m_pStackedWidget->setCurrentIndex(-1);
					break;
				case HStimulusSource::VIDEO:
					m_pMediaObject->setCurrentSource((m_sources[number].filename()));
					m_pVideoWidget->setGeometry(geometry());
					qDebug() << "image-VIDEO geom " << geometry().left() << "," << geometry().bottom() << "," << geometry().width() << "," << geometry().height();
					qDebug() << "image-VIDEO geom " << m_pVideoWidget->geometry().left() << "," << m_pVideoWidget->geometry().bottom() << "," << m_pVideoWidget->geometry().width() << "," << m_pVideoWidget->geometry().height();
					m_pStackedWidget->setCurrentIndex(1);
					m_pAudioOutput->setVolume(m_sources.at(number).getAudioBalance());
					m_pMediaObject->play();
					//m_pVideoWidget->setFullScreen(m_isFullScreen);
					if (m_parent) m_parent->activateWindow(); //Hack Alert!
					break;
				case HStimulusSource::IMAGE:
					m_pImageWidget->setCurrentSource((m_sources.at(number)).filename());
					m_pStackedWidget->setCurrentIndex(0);
					break;
				default:
					break;
			}
		default:
			break;
	}
	m_iCurrentStim = number;

}
#endif

void HVIPlayer::onPrefinishMarkReached(qint32 msec)
{
	Q_UNUSED(msec);
	if (m_iCurrentStim > -1 && m_iCurrentStim < m_sources.count() && (m_sources.at(m_iCurrentStim)).type() == HStimulusSource::VIDEO && (m_sources.at(m_iCurrentStim)).isLooped())
	{
		m_pMediaObject->pause();
		m_pMediaObject->seek(0);
		m_pMediaObject->play();
	}
}


