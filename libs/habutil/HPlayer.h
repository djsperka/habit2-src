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

#include <QMap>
#include <QDir>
#include "stimulusinfo.h"


// The HPlayer plays stimuli for Habit. 
// The player holds a list of stimuli and plays them via play(int)
// and playAG(). Internally, the stimuli are stored in a QList. 
// The attention getter stim (AG) is always at index 0, and the others
// are at index>0. This is a convenience for Habit, which allows users
// to number stimuli with positive integers. 

class HPlayer : public QObject
{

	Q_OBJECT
	
public:
	HPlayer(int ID = 0, QObject* w = 0, const QDir& dir = QDir("/"));
	virtual ~HPlayer();

	/// get the id
	int id() { return m_id; }
	
	/// Play the stim at index 'number'. Out of range index defaults to background.
	virtual void play(unsigned int number) = 0;
	
	/// Play the currently configured attention getter. 
	//virtual void playAG() { play(0); };
	
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
	//virtual unsigned int addStimulus(QString filename, int volume=0, bool isLooped = false);

	// TODO: this will replace addStimulus(id, filename, etc) and addStimulus(id);
	// Will call addStimulusPrivate(id) = 0;
	// Subclasses may assume that getStimulusInfo(id) will return the correct thing.
	
	unsigned int addStimulus(const unsigned int id, const Habit::StimulusInfo& info);

	/// should stimuli be buffered? This is advisory, subclasses may ignore. Put here especially for
	/// video types, see HVIPlayer
	bool preferBufferedStimulus() const { return 	m_preferBufferedStimulus; };
	void setPreferBufferedStimulus(bool buffer) { m_preferBufferedStimulus = buffer; };

	// dummy value to be returned when getStimulusInfo gets a bad key.
	static const Habit::StimulusInfo dummyStimulusInfo;

protected:	
	/// An id number for this player. Can be used to indicate screen number, but not required. 
	int m_id;
	
	/// Map of stimulus sources to be used. The [0] index is used as the attention getter,
	/// so calling addAG() always replaces the current attention getter.
	//QMap<unsigned int, HStimulusSource *> m_sources;
	
	/// index of currently playing stim. An out of range index defaults to playing background.
	//int m_iCurrentStim;

	// Get the StimulusInfo object for given key.
	const Habit::StimulusInfo& getStimulusInfo(unsigned int key);

	// Get current stim info
	//const Habit::StimulusInfo& getCurrentStimulusInfo() { return getStimulusInfo(m_iCurrentStim); };

	// returns the next key to be used. Equal to the largest current key + 1.
	unsigned int nextKey() { return m_mapPStimulusInfo.count() - 1;};

	unsigned int count() { return m_mapPStimulusInfo.count(); };

	const QDir& getStimulusRoot() { return m_dirStimulusRoot; };

private:

	// map of StimulusInfo objects, by key.
	QMap<unsigned int, const Habit::StimulusInfo* > m_mapPStimulusInfo;

	/// stimulus root directory. Any stimuli with relative paths will be found here, hopefully.
	QDir m_dirStimulusRoot;

	/// Should stimulus files be buffered by subclasses?
	bool m_preferBufferedStimulus;

	virtual unsigned int addStimulusPrivate(unsigned int id) = 0;


Q_SIGNALS:
	void started(int i, const QString& filename);
	void cleared(int i);

};


//QTextStream& operator<<(QTextStream& out, const HPlayer& player);

#endif
