/*
 * HExperimentUtil.h
 *
 *  Created on: May 15, 2014
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTUTIL_H_
#define HEXPERIMENTUTIL_H_


#include "HExperiment.h"
#include "HMediaManager.h"
#include "HLookDetector.h"
#include "HEventLog.h"
#include "runsettings.h"
#include "experimentsettings.h"

HExperiment* createExperiment(QWidget *w, const Habit::RunSettings& runSettings, const Habit::ExperimentSettings& experimentSettings, HLookDetector* pld, HMediaManager* pmm, HEventLog& log);




#endif /* HEXPERIMENTUTIL_H_ */
