/*
 *  HNonPlayer.h
 *  habutil
 *
 *  Created by Oakes Lab on 8/24/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef HABITNONPLAYER_H
#define HABITNONPLAYER_H


#include <QLabel>
#include <QMap>
#include <QTextStream>
//#include "HStimulusSource.h"
#include "HPlayer.h"

class HNonPlayer : public HPlayer
{
	Q_OBJECT
	
public:
	HNonPlayer(int id = 0, QWidget* w = 0) { Q_UNUSED(id); Q_UNUSED(w); };
	~HNonPlayer() {};
	virtual void play(unsigned int number);
	virtual void stop(); 
	virtual void clear();
	friend QTextStream& operator<<(QTextStream& out, const HNonPlayer& player);
	
private:
	unsigned int addStimulusPrivate(unsigned int id);

};

#endif
