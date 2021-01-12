/*
 *  HMediaManagerUtil.h
 *  habutil
 *
 *  Created by Oakes Lab on 9/18/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HGMM.h"
#include "experimentsettings.h"
#include "stimulusdisplayinfo.h"

// create new HGMM with widgets of given size. Retrieve the widgets with
// HStimulusWidget *getHStimulusWidget(const HPlayerPositionType& type);
HGMM* createMediaManager(const Habit::StimulusDisplayInfo& sdi, int w, int h, const QString& name);

// same, but for full screen. Will use preferences for monitor assignments
HGMM* createMediaManager(const Habit::StimulusDisplayInfo& sdi, const QString& name);

// create widgets at given size
void initializeMediaManager(const Habit::ExperimentSettings& es, int screenWidth, int screenHeight);

// Create widgets fullscreen, using monitor ids found in preferences
void initializeMediaManager(const Habit::ExperimentSettings& es);


