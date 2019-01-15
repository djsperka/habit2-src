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

HGMM* initializeMediaManager(const Habit::ExperimentSettings& es, int screenWidth, int screenHeight);
HGMM* initializeMediaManager(const Habit::StimulusDisplayInfo& sdi, int screenWidth, int screenHeight);
HGMM* initializeMediaManager(const Habit::ExperimentSettings& es);


