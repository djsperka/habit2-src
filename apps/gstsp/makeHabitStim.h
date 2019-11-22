/*
 * makeHabitStim.h
 *
 *  Created on: Nov 20, 2019
 *      Author: dan
 */

#ifndef MAKEHABITSTIM_H_
#define MAKEHABITSTIM_H_

#include "Stim.h"
#include "stimulusdisplayinfo.h"
#include "stimulussettings.h"
#include "HMM.h"

hmm::Stim *makeHabitStim(const Habit::StimulusSettings& ss, const Habit::StimulusDisplayInfo& info, hmm::HMM& mm);





#endif /* MAKEHABITSTIM_H_ */
