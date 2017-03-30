/*
 * HPlayer5.h
 *
 *  Created on: Feb 28, 2017
 *      Author: dan
 */

#ifndef APPS_Q5PLAYER_HPLAYER5_H_
#define APPS_Q5PLAYER_HPLAYER5_H_

#include "HPlayer.h"
#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <QMap>
#include <QWidget>
#include <QDir>

class HVLCMediaPlayer: public HPlayer
{
	Q_OBJECT

	QMap<unsigned int, VlcMedia *> m_media;
    VlcInstance *m_pvlcInstance;
    VlcMediaPlayer *m_pvlcPlayer;

	unsigned int addStimulusPrivate(unsigned int id);

public:
	HVLCMediaPlayer(int ID = 0, QWidget* w = 0, const QDir& dir = QDir("/"));
	virtual ~HVLCMediaPlayer();

	/// Play the stim at index 'number'. Out of range index defaults to background.
	virtual void play(unsigned int number);

	/// Stop playing the current stim. Has no effect on a video that has stopped, or
	/// on an image or background.
	virtual void stop();

	/// Clear screen or equivalent. Emit cleared() when operation complete.
	virtual void clear();

	VlcInstance *vlcInstance() { return m_pvlcInstance; }
	VlcMediaPlayer *vlcMediaPlayer() { return m_pvlcPlayer; }

protected:
	QMap<unsigned int, VlcMedia *>& media() { return m_media; }

signals:
	void sizeChanged(unsigned int, unsigned int);

private slots:
	void buffering(int b);
	void opening();
	void playing();
	void parsedChanged(bool b);

};

#endif /* APPS_Q5PLAYER_HPLAYER5_H_ */
