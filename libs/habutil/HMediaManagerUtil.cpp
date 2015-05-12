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


HMediaManager* createMediaManager(const Habit::ExperimentSettings& es, QWidget* parent)
{
	Q_UNUSED(parent);
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
		//pmm->addPlayer(HPlayerPositionType::Sound, new HAudioPlayer(iControl, parent), iControl);
		pmm->addPlayer(HPlayerPositionType::Sound, new HAudioPlayer(iControl, NULL, baseDir), iControl);
	}
	if (layoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		//pmm->addPlayer(HPlayerPositionType::Center, new HVIPlayer(iCenter, parent, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor()), iCenter);
		pmm->addPlayer(HPlayerPositionType::Center, new HVIPlayer(iCenter, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor()), iCenter);
	}
	else if (layoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		//pmm->addPlayer(HPlayerPositionType::Left, new HVIPlayer(iLeft, parent, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor()), iLeft);
		//pmm->addPlayer(HPlayerPositionType::Right, new HVIPlayer(iRight, parent, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor()), iRight);
		pmm->addPlayer(HPlayerPositionType::Left, new HVIPlayer(iLeft, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor()), iLeft);
		pmm->addPlayer(HPlayerPositionType::Right, new HVIPlayer(iRight, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor()), iRight);
	}
	
	return pmm;
}

