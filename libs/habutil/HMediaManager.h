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
	QMap<HPlayerPositionType, HPlayer *> m_players;
	QMap<unsigned int, Habit::StimulusSettings> m_mapStim;
	unsigned int nextKey();
	unsigned int addStimulus(unsigned int key, const Habit::StimulusSettings& ss);
public:

	HMediaManager();
	~HMediaManager();
	void addPlayer(const HPlayerPositionType& ppt, HPlayer* player, int screenIndex=-1);
	void clear();
	unsigned int addAG(const Habit::StimulusSettings& ags);
	void addStimuli(const Habit::StimuliSettings& ss, QList<unsigned int>& idList);
	unsigned int addStimulus(const Habit::StimulusSettings& ss);
	Habit::StimulusSettings getStimulusSettings(unsigned int key);

	const QMap<unsigned int, Habit::StimulusSettings>& map() { return m_mapStim; };
	static const unsigned int backgroundKey;

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
