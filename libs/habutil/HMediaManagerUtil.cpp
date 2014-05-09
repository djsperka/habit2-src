/*
 *  HMediaManagerUtil.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/18/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HMediaManagerUtil.h"
#include "HVIPlayer.h"
#include "HAudioPlayer.h"


HMediaManager* createMediaManager(const Habit::ExperimentSettings& es, QWidget* parent)
{
	Q_UNUSED(parent);
	HMediaManager* pmm = new HMediaManager();
	HPlayer *playerLeft = NULL;
	HPlayer *playerCenter = NULL;
	HPlayer *playerRight = NULL;
	HPlayer *playerControl = NULL;
	
	// Monitor settings
	Habit::MonitorSettings ms = es.getMonitorSettings();
	
	// Stimulus Display info
	Habit::StimulusDisplayInfo sdi = es.getStimulusDisplayInfo();

	// AttentionGetter
	Habit::AttentionGetterSettings ags = es.getAttentionGetterSettings();
	
	// Now look at stimuli settings for each of the three phases. 
	Habit::StimuliSettings ssPreTest = es.getPreTestStimuliSettings();
	Habit::StimuliSettings ssHabituation = es.getHabituationStimuliSettings();
	Habit::StimuliSettings ssTest = es.getTestStimuliSettings();
	
	// Create players for each monitor that will be used. 
	// Non-NULL player pointers will indicate that a particular player was assigned to a monitor. 
	if ((ms.getLeftMonitorIndex() > -1)) 
	{
		playerLeft = new HVIPlayer(ms.getLeftMonitorIndex(), NULL, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen);
		pmm->addPlayer(HPlayerPositionType::Left, playerLeft, ms.getLeftMonitorIndex());
	}
	if (ms.getCenterMonitorIndex() > -1)
	{
		playerCenter = new HVIPlayer(ms.getCenterMonitorIndex(), NULL, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen);
		pmm->addPlayer(HPlayerPositionType::Center, playerCenter, ms.getCenterMonitorIndex());
	}
	if ((ms.getRightMonitorIndex() > -1)) 
	{
		playerRight = new HVIPlayer(ms.getRightMonitorIndex(), NULL, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen);
		pmm->addPlayer(HPlayerPositionType::Right, playerRight, ms.getRightMonitorIndex());
	}
	if ((ms.getControlMonitorIndex() > -1)) 
	{
		playerControl = new HAudioPlayer(ms.getControlMonitorIndex());
		pmm->addPlayer(HPlayerPositionType::Sound, playerControl, ms.getControlMonitorIndex());
	}
	
	return pmm;
}

