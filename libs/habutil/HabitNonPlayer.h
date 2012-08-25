/*
 *  HabitNonPlayer.h
 *  habutil
 *
 *  Created by Oakes Lab on 8/24/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef HABITNONPLAYER_H
#define HABITNONPLAYER_H


#include <QtGui/QLabel>
#include <QList>
#include <QTextStream>
#include "StimulusSource.h"
#include "HabitPlayer.h"

class HabitNonPlayer : public HabitPlayer
{
	Q_OBJECT
	
public:
	HabitNonPlayer(int id = 0, QWidget* w = 0) { Q_UNUSED(id); Q_UNUSED(w); };
	~HabitNonPlayer() {};
	virtual void play(int number);
	virtual void stop(); 
	friend QTextStream& operator<<(QTextStream& out, const HabitNonPlayer& player);
	
private:
	
};

#endif