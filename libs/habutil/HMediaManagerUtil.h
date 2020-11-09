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

// create widgets at given size
void initializeMediaManager(const Habit::ExperimentSettings& es, int screenWidth, int screenHeight);

// Create widgets fullscreen, using monitor ids found in preferences
void initializeMediaManager(const Habit::ExperimentSettings& es);


