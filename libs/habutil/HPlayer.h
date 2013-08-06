/*
 *  HPlayer.h
 *  myp
 *
 *  Created by Oakes Lab on 6/4/12.
 *  Copyright 2012 Center for Neuroscience. All rights reserved.
 *
 */

#ifndef HABITPLAYER_H
#define HABITPLAYER_H

#ifdef USING_QT5
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QList>
#include <QTextStream>
#include "HStimulusSource.h"



// The HPlayer plays stimuli for Habit. 
// The player holds a list of stimuli and plays them via play(int)
// and playAG(). Internally, the stimuli are stored in a QList. 
// The attention getter stim (AG) is always at index 0, and the others
// are at index>0. This is a convenience for Habit, which allows users
// to number stimuli with positive integers. 

class HPlayer : public QWidget
{
	Q_OBJECT
	
public:
	HPlayer(int ID = 0, QWidget* w = 0);
	virtual ~HPlayer();

	
	/// Play the stim at index 'number'. Out of range index defaults to background.
	virtual void play(int number) = 0;
	
	/// Play the currently configured attention getter. 
	virtual void playAG() { play(0); };
	
	/// Stop playing the current stim. Has no effect on a video that has stopped, or
	/// on an image or background. 
	virtual void stop() = 0; 
	
	/// Clear screen or equivalent. Emit cleared() when operation complete. 
	virtual void clear() = 0;	

	// Add a stimulus to the player. This function stores the stimuli at index
	// positions 1, 2, .... By default, the first position, index 0, is reserved
	// for the attention getter (AG). That position is initialized with an empty
	// stim, so even if no AG has been added, the first stim added will still be 
	// at position 1. The return value is the stim position number, and should 
	// be used in a call to play(). 
	virtual int addStimulus(QString filename, int volume=0, bool isLooped = false);
	
	// add dummy (background) stimulus
	virtual int addStimulus();
	
	// Add an attention getter stimulus to the player. The AG will always have
	// index 0. It can be played by calling play(0) or playAG().
	virtual int addAG(QString filename, int volume=0, bool isLooped = false);

	int count() const { return m_sources.count(); };
	const HStimulusSource& source(int i) const
	{
		return m_sources.at(i);
	};

protected:	
	/// An id number for this player. Can be used to indicate screen number, but not required. 
	int m_id;
	
	/// List of stimulus sources to be used. The [0] index is used as the attention getter, 
	/// so calling addAG() always replaces the current attention getter.
	QList<HStimulusSource> m_sources;
	
	/// index of currently playing stim. An out of range index defaults to playing background.
	int m_iCurrentStim;
	
	/// Gets the type of stim corresponding to index
	HStimulusSource::HStimulusSourceType getStimulusType(int index);
	
	/// Gets the type of the stim corresponding to m_iCurrentStim.
	HStimulusSource::HStimulusSourceType getCurrentStimulusType()
	{
		return getStimulusType(m_iCurrentStim);
	};


signals:
	void started(int i);
	void cleared(int i);

};


QTextStream& operator<<(QTextStream& out, const HPlayer& player);

#endif
