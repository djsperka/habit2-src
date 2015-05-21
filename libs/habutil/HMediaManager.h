/*
 *  HMediaManager.h
 *  habutil
 *
 *  The media manager owns and controls the display of images, video and audio
 *  stimuli for habit. Classes which inherit the HPlayer interface provide 
 *  the nuts and bolts to deliver stimuli to a single "screen". There is no 
 *  requirement on what the individual HPlayers actually do. They should provide
 *  a SIGNAL(playerStarted(int)) which indicates when a stim has started playing 
 *  or its display has been initiated. 
 *
 *  One word on destruction: The HVideoImagePlayers require that top level windows 
 *  be created. That means they lose their parental relationships, and so they are
 *  not automaticaly closed when the parent dialog is closed. Thus, it is important
 *  to overload the closeEvent() method in the parent dialog. The HMediaManager
 *  destructor should be called (call delete on its pointer) - this will in turn 
 *  close the widgets that are owned by the HPlayers, and they will disappear from 
 *  the screen they are displayed on. 

 void HControlPanel::closeEvent(QCloseEvent* e)
 {
    if (pmediamanager)
    {
       delete pmediamanager;
       pmediamanager = NULL;
    }
    e->accept();
 }
 
 *  Created by Oakes Lab on 7/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HMEDIAMANAGER_H_
#define _HMEDIAMANAGER_H_

#include <QObject>
#include <QMap>
#include "HPlayer.h"
#include "HTypes.h"
#include "attentiongettersettings.h"
#include "stimulisettings.h"


class HMediaManager : public QObject
{
	Q_OBJECT

private:
	bool m_pendingStartSignal;
	bool m_pendingAGStartSignal;
	bool m_pendingClearSignal;
	int m_pendingStimNumber;
	bool m_bPlayersAreFullScreen;	// if set, players will be moved and resized to be full screen.
									// if not set, then do not adjust position of players here when added.

	// A map containing HPlayers. The key is the HPlayerPositionType (Left, Right, etc)
	QMap<HPlayerPositionType, HPlayer *> m_players;

	// A map containing StimulusSettings objects, key is an integer.
	//QMap<unsigned int, Habit::StimulusSettings> m_mapStim;
	QMap<unsigned int, const Habit::StimulusSettings *> m_mapPStimulusSettings;

	// A map containing lists of stimulus keys (keys into m_mapStim). Here the key
	// is HStimContext.
	// getContextStimList fetches a list of keys for the stimuli added for the Test phase.
	QMap<HStimContext, QList<unsigned int> > m_mapContext;

	// Get the next key for m_mapStim. The values are doled out sequentially. Nothing special,
	// this is a convenience.
	unsigned int nextKey();

	unsigned int addStimulus(unsigned int key, const Habit::StimulusSettings& ss);

	void addOrAppendList(const HStimContext& c, const QList<unsigned int>& list);

public:

	HMediaManager(bool bPlayersAreFullScreen = true);
	~HMediaManager();
	void addPlayer(const HPlayerPositionType& ppt, HPlayer* player, int screenIndex=-1);
	void clear();
	unsigned int addAG(const Habit::StimulusSettings& ags);
	void addStimuli(const Habit::StimuliSettings& ss);
	unsigned int addStimulus(const Habit::StimulusSettings& ss);

	const Habit::StimulusSettings& getStimulusSettings(unsigned int key) const;
	unsigned int getContextStimList(const HStimContext& c, QList<unsigned int>& list);

	//const QMap<unsigned int, const Habit::StimulusSettings>& map() { return m_mapStim; };
	const QMap<unsigned int, const Habit::StimulusSettings *>& pmap() { return m_mapPStimulusSettings; };

	static const unsigned int backgroundKey;
	static const unsigned int agKey;

	// used as placeholder for background and AG. Also returned when getStimulusSettings is passed a bad key.
	static const Habit::StimulusSettings dummyStimulusSettings;

public slots:

	void stim(int);
	void ag();
	void playerStarted(int i, const QString& filename);
	void playerCleared(int i);

signals:
	void agStarted(int);
	void stimStarted(int);
	void cleared();
	void screen(int, const QString&);
};

#endif
