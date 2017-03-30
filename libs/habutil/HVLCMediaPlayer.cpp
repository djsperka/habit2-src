/*
 * HVLCPlayer5.cpp
 *
 *  Created on: Feb 28, 2017
 *      Author: dan
 */

#include "HVLCMediaPlayer.h"

#include <QtDebug>
#include <QMapIterator>
#include <vlc/vlc.h>

HVLCMediaPlayer::HVLCMediaPlayer(int ID, QWidget* w, const QDir& dir)
: HPlayer(ID, w, dir)
{
	m_pvlcInstance = new VlcInstance(VlcCommon::args());
	m_pvlcPlayer = new VlcMediaPlayer(m_pvlcInstance);
	//connect(m_pvlcPlayer, SIGNAL(buffering(int)), this, SLOT(buffering(int)));
	//connect(m_pvlcPlayer, SIGNAL(opening()), this, SLOT(opening()));
	connect(m_pvlcPlayer, SIGNAL(playing()), this, SLOT(playing()));
}

HVLCMediaPlayer::~HVLCMediaPlayer()
{
	QMapIterator<unsigned int, VlcMedia *> it(media());
	qInfo() << "~HVLCPlayer5";
	while (it.hasNext())
		delete it.next().value();
	delete m_pvlcPlayer;
//	delete m_pvlcInstance;
}

void HVLCMediaPlayer::buffering(int b)
{
	qInfo() << "buffering " << b << " id " << id();
}

void HVLCMediaPlayer::opening()
{
	qInfo() << "opening " << id();
}

void HVLCMediaPlayer::playing()
{
	QString filename("NowPlayingThis.file");
	//Put resize of widget here?!?
	emit started(id(), filename);
	//qInfo() << "playing " << id();
}

unsigned int HVLCMediaPlayer::addStimulusPrivate(unsigned int id)
{
	VlcMedia *pVlcMedia = new VlcMedia(getStimulusInfo(id).getAbsoluteFileName(getStimulusRoot()), true, m_pvlcInstance);
	connect(pVlcMedia, SIGNAL(parsedChanged(bool)), this, SLOT(parsedChanged(bool)));
	m_media.insert(id, pVlcMedia);
	return id;
}

void HVLCMediaPlayer::parsedChanged(bool b)
{
	qInfo() << "HVLCPlayer5::parsedChanged( " << b << " )";
	// what can we learn about the media now?

	int i = 0;
	unsigned int x=0, y=0;
	i = libvlc_video_get_size	(m_pvlcPlayer->core(), 0, &x, &y);
	qInfo() << "libvlc_video_get_size returned " << i << " size " << x << " X " << y;
	emit sizeChanged(x, y);
}

/// Play the stim at index 'number'. Out of range index defaults to background.
void HVLCMediaPlayer::play(unsigned int number)
{
	qInfo() << "HVLCPlayer5::play(" << number << ")";
	if (media().contains(number))
	{
		m_pvlcPlayer->open(media()[number]);
	}
	else
		qInfo() << "play(" << number << ") - not found";
	qInfo() << "HVLCPlayer5::play(" << number << ") - done";
}

/// Stop playing the current stim. Has no effect on a video that has stopped, or
/// on an image or background.
void HVLCMediaPlayer::stop()
{

}

/// Clear screen or equivalent. Emit cleared() when operation complete.
void HVLCMediaPlayer::clear()
{

}
