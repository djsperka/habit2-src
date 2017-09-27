/*
 * HExperimentUtil.h
 *
 *  Created on: May 15, 2014
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTUTIL_H_
#define HEXPERIMENTUTIL_H_


#include "HStateMachine.h"
#include "HGMM.h"
#include "HLookDetector.h"
#include "HEventLog.h"
#include "runsettings.h"
#include "experimentsettings.h"

HStateMachine* createExperiment(QWidget *w, const Habit::RunSettings& runSettings, const Habit::ExperimentSettings& experimentSettings, HLookDetector* pld, HGMM* pmm, HEventLog& log, bool bInputTesting=false);




#endif /* HEXPERIMENTUTIL_H_ */
