/*
 *  HabitPlayer.h
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 Center for Neuroscience. All rights reserved.
 *
 */

#ifndef HABITPLAYER_H
#define HABITPLAYER_H

#include <QtGui/QLabel>
#include <QList>
#include <QTextStream>
#include "StimulusSource.h"



// The HabitPlayer plays stimuli for Habit. 
// The player holds a list of stimuli and plays them via play(int)
// and playAG(). Internally, the stimuli are stored in a QList. 
// The attention getter stim (AG) is always at index 0, and the others
// are at index>0. This is a convenience for Habit, which allows users
// to number stimuli with positive integers. 

class HabitPlayer : public QLabel
{
	Q_OBJECT
	
public:
	HabitPlayer(int ID = 0, QWidget* w = 0);
	~HabitPlayer() {};
	virtual void play(int number) = 0;
	virtual void playAG() { play(0); };
	virtual void stop() = 0; 
	
	bool eventFilter(QObject *object, QEvent *event);

	// Add a stimulus to the player. This function stores the stimuli at index
	// positions 1, 2, .... By default, the first position, index 0, is reserved
	// for the attention getter (AG). That position is initialized with an empty
	// stim, so even if no AG has been added, the first stim added will still be 
	// at position 1. The return value is the stim position number, and should 
	// be used in a call to play(). 
	virtual int addStimulus(QString filename, bool isLooped = false);
	
	// Add an attention getter stimulus to the player. The AG will always have
	// index 0. It can be played by calling play(0) or playAG().
	virtual int addAG(QString filename, bool isLooped = false);

	int count() const { return m_sources.count(); };
	const StimulusSource& source(int i) const
	{
		return m_sources.at(i);
	};

protected:	
	int m_id;
	QList<StimulusSource> m_sources;
	int m_iCurrentStim;
	StimulusSource::StimulusSourceType getCurrentType();


private:
	
signals:
	void started(int i);

};


QTextStream& operator<<(QTextStream& out, const HabitPlayer& player);

#endif