/*
 * HVIPlayer5.h
 *
 *  Created on: Feb 28, 2017
 *      Author: dan
 */

#ifndef APPS_Q5PLAYER_HVIPLAYER5_H_
#define APPS_Q5PLAYER_HVIPLAYER5_H_

#include "HPlayer5.h"

class HVIPlayer5: public HPlayer5
{
	Q_OBJECT

private:
	QMediaPlayer m_mediaPlayer;

public:

	HVIPlayer5(int id = 0, QWidget* w = 0, const QDir& stimRootDir = QDir("/"));
	virtual ~HVIPlayer5() {};

	/// Play the stim at index 'number'. Out of range index defaults to background.
	virtual void play(unsigned int number);

	/// Stop playing the current stim. Has no effect on a video that has stopped, or
	/// on an image or background.
	virtual void stop();

	/// Clear screen or equivalent. Emit cleared() when operation complete.
	virtual void clear();

	/// Set video output to the fgiven widget. In HVIPlayer5 only, not HAudioPlayer5
	void setVideoOutput(QVideoWidget *output);

protected slots:

	void stateChanged(QMediaPlayer::State state);
	void mediaStatusChanged(QMediaPlayer::MediaStatus status);

};

#endif /* APPS_Q5PLAYER_HVIPLAYER5_H_ */
