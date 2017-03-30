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
#include <QFrame>
#include "HVLCMediaPlayer.h"
#include "HVLCVideoWidget.h"
#else
#include "HVIPlayer.h"
#include "HAudioPlayer.h"
#endif
#include "HWorkspaceUtil.h"


#if QT_VERSION >= 0x050000


// Always first create a media manager.
// In 2.2 HPlayer is a QObject, not a QWidget. That makes the notion of the
// HPlayer similar to the usage of the term in multimedia packages like vlc.
// After creating the manager you can populate it - the media are created and
// saved for use later.
// In 2.2 there are new utility functions here
// createPlayerWidget() et al - these return a QWidget. It must be shown
// - call show(), even if you add it to a layout, etc.
//

QWidget *createMediaPlayerWidget(HMediaManager *pmm)
{
	// Create a single frame to hold all viewers (even if there's just one)

}

HMediaManager* createMediaManager(const Habit::ExperimentSettings& es)
{
	HMediaManager* pmm = new HMediaManager();
	Habit::StimulusDisplayInfo sdi = es.getStimulusDisplayInfo();
	QDir rootDir;
	habutilGetStimulusRootDir(rootDir);

#if 0
	if (sdi.getUseISS())
	{
		HVLCPlayer5 *sound = new HVLCPlayer5(habutilGetMonitorID(HPlayerPositionType::Control), NULL, rootDir);
		pmm->addPlayer(HPlayerPositionType::Sound, sound);
	}
	if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HVLCPlayer5 *single = new HVLCPlayer5(habutilGetMonitorID(HPlayerPositionType::Center), NULL, rootDir);
		single->setPreferBufferedStimulus(false);
		pmm->addPlayer(HPlayerPositionType::Center, single);
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HVLCPlayer5 *left = new HVLCPlayer5(habutilGetMonitorID(HPlayerPositionType::Left), NULL, rootDir);
		HVLCPlayer5 *right = new HVLCPlayer5(habutilGetMonitorID(HPlayerPositionType::Right), NULL, rootDir);
		pmm->addPlayer(HPlayerPositionType::Left, left);
		pmm->addPlayer(HPlayerPositionType::Right, right);
	}
#endif
	return pmm;
}


HPreviewMediaManager* createPreviewMediaManager(const Habit::StimulusDisplayInfo& sdi)
{
	HPreviewMediaManager* pmm = new HPreviewMediaManager(sdi.getStimulusLayoutType());
	return pmm;
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
