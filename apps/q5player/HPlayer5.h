/*
 * HPlayer5.h
 *
 *  Created on: Feb 28, 2017
 *      Author: dan
 */

#ifndef APPS_Q5PLAYER_HPLAYER5_H_
#define APPS_Q5PLAYER_HPLAYER5_H_

#include "HPlayer.h"
#include "HMediaHandle.h"
#include <QMap>
#include <QMediaPlayer>
#include <QWidget>
#include <QDir>

class HPlayer5: public HPlayer
{
	Q_OBJECT

	QMap<unsigned int, HMediaHandle* > m_media;

	unsigned int addStimulusPrivate(unsigned int id);

public:
	HPlayer5(int ID = 0, QWidget* w = 0, const QDir& dir = QDir("/"));
	virtual ~HPlayer5();

protected:
	QMap<unsigned int, HMediaHandle* >& media() { return m_media; }
};

#endif /* APPS_Q5PLAYER_HPLAYER5_H_ */
