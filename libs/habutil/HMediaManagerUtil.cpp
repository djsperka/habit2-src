/*
 *  HMediaManagerUtil.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/18/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HMediaManagerUtil.h"

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#else
#include "HVIPlayer.h"
#include "HAudioPlayer.h"
#endif
#include "HWorkspaceUtil.h"


#if QT_VERSION >= 0x050000

HMediaManager* createMediaManager(const Habit::ExperimentSettings& es)
{
	return (HMediaManager *)NULL;
}

HPreviewMediaManager* createPreviewMediaManager(const Habit::StimulusDisplayInfo& sdi)
{
	return (HPreviewMediaManager *)NULL;
}

#else

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
		HPlayer *single = new HVIPlayer(iCenter, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor(), sdi.isOriginalAspectRatioMaintained());
		single->setPreferBufferedStimulus(false);
		pmm->addPlayer(HPlayerPositionType::Center, single, iCenter);
	}
	else if (layoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HPlayer *left = new HVIPlayer(iLeft, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor(), sdi.isOriginalAspectRatioMaintained());
		HPlayer *right = new HVIPlayer(iRight, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor(), sdi.isOriginalAspectRatioMaintained());
		pmm->addPlayer(HPlayerPositionType::Left, left, iLeft);
		pmm->addPlayer(HPlayerPositionType::Right, right, iRight);
	}
	
	return pmm;
}


HPreviewMediaManager* createPreviewMediaManager(const Habit::StimulusDisplayInfo& sdi)
{
	HPreviewMediaManager* pmm = new HPreviewMediaManager(sdi.getStimulusLayoutType());
	HPlayer *psound = NULL;
	if (sdi.getUseISS())
	{
		psound = new HAudioPlayer(-1, NULL, dirStimRoot);
		pmm->addPlayer(HPlayerPositionType::Sound, p, -1);
	}

	HPlayer *p0 = new HVIPlayer(-1, NULL, dirStimRoot, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor());
	p0->setPreferBufferedStimulus(false);
	HPlayer *p1l = new HVIPlayer(-1, NULL, dirStimRoot, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor());
	p1l->setPreferBufferedStimulus(false);
	HPlayer *p1r = new HVIPlayer(-1, NULL, dirStimRoot, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor());
	p1r->setPreferBufferedStimulus(false);

	pmm->addPlayers(p0, p1l, p1r, psound);


}

#endif  /* QT_VERSION < 5 */
