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
#include "HWorkspaceUtil.h"


HMediaManager* createMediaManager(const Habit::ExperimentSettings& es)
{
	HMediaManager* pmm = new HMediaManager();

	// Stimulus Display info
	Habit::StimulusDisplayInfo sdi = es.getStimulusDisplayInfo();

	// Create players for each monitor that will be used. 
	int iControl = habutilGetMonitorID(HPlayerPositionType::Control);
	int iLeft = habutilGetMonitorID(HPlayerPositionType::Left);
	int iCenter = habutilGetMonitorID(HPlayerPositionType::Center);
	int iRight = habutilGetMonitorID(HPlayerPositionType::Right);
	const HStimulusLayoutType& layoutType = sdi.getStimulusLayoutType();

	QDir baseDir;
	habutilGetStimulusRootDir(baseDir);


	if (iControl > -1 && sdi.getUseISS())
	{
		HPlayer *audio = new HAudioPlayer(iControl, NULL, baseDir);
		pmm->addPlayer(HPlayerPositionType::Sound, audio, iControl);
	}
	if (layoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HPlayer *single = new HVIPlayer(iCenter, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor());
		single->setPreferBufferedStimulus(false);
		pmm->addPlayer(HPlayerPositionType::Center, single, iCenter);
	}
	else if (layoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HPlayer *left = new HVIPlayer(iLeft, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor());
		HPlayer *right = new HVIPlayer(iRight, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor());
		pmm->addPlayer(HPlayerPositionType::Left, left, iLeft);
		pmm->addPlayer(HPlayerPositionType::Right, right, iRight);
	}
	
	return pmm;
}

