/*
 * HAudioPlayer5.h
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#ifndef APPS_Q5PLAYER_HAUDIOPLAYER5_H_
#define APPS_Q5PLAYER_HAUDIOPLAYER5_H_

#include "HPlayer5.h"
#include <QWidget>
#include <QDir>
#include <QMediaPlayer>


class HAudioPlayer5: public HPlayer5
{
	Q_OBJECT

	QMediaPlayer m_mediaPlayer;

public:
	HAudioPlayer5(int id = 0, QWidget* w = 0, const QDir& stimRootDir = QDir("/"));
	virtual ~HAudioPlayer5();

	/// Play the stim at index 'number'. Out of range index defaults to background.
	virtual void play(unsigned int number);

	/// Stop playing the current stim. Has no effect on a video that has stopped, or
	/// on an image or background.
	virtual void stop();

	/// Clear screen or equivalent. Emit cleared() when operation complete.
	virtual void clear();

protected slots:

	void stateChanged(QMediaPlayer::State state);
	void mediaStatusChanged(QMediaPlayer::MediaStatus status);

	//private:
//	virtual unsigned int addStimulusPrivate(unsigned int id);

};

#endif /* APPS_Q5PLAYER_HAUDIOPLAYER5_H_ */
