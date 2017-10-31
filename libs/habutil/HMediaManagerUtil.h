/*
 *  HMediaManagerUtil.h
 *  habutil
 *
 *  Created by Oakes Lab on 9/18/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HGMM.h"
#include "HPreviewMediaManager.h"
#include "experimentsettings.h"
#include "stimulussettings.h"
#include "stimulusdisplayinfo.h"
#include <QVector>
#include <QWidget>

// Will create players and add stimuli to them based on the experiment settings. It is assumed that the 
// experiment settings have been fully loaded from the db - loadFromDB is NOT called in this function. 
// The vectors are filled with ints corresponding to the positions of the stimuli configured for each phase. 
// If, for example, there are three PreTest stimuli, 3 Habituation stimuli, and 3 Test stimuli, then 
// vPreTest will contain 1, 2, 3; vHabituation will contain 4, 5, 6; vTest will contain 7, 8,9. 
// Position 0 in the players is reserved for the attention getter (even if there is no attention getter
// used this position is filled with a background stimulus). 

HGMM* createMediaManager(const Habit::ExperimentSettings& es, int screenWidth, int screenHeight);
HGMM* createMediaManager(const Habit::StimulusDisplayInfo& sdi, int screenWidth, int screenHeight);
HGMM* createMediaManager(const Habit::ExperimentSettings& es);


