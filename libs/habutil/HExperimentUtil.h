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

Habit::HStimulusSettingsList getOrderOfStimuli(const Habit::PhaseRunSettings& prs, unsigned int ntrials, const Habit::StimuliSettings& ss);
HStateMachine* createExperiment(QWidget *w, const Habit::RunSettings& runSettings, const Habit::ExperimentSettings& experimentSettings, HGMM *phgmm, HLookDetector* pld, HEventLog& log, bool bInputTesting=false);
//void populateMediaManager(const Habit::HPhaseSettings& ps, const Habit::PhaseRunSettings& phaseRunSettings, QList< QPair<int, QString> >& stimidListOrdered);
//void getOrderedStimidList(const Habit::HPhaseSettings& ps, const Habit::RunSettings& runSettings, QList< QPair<int, QString> >& stimidListOrdered);
//PhaseStimulusTrialOrderMap getStimulusTrialOrder(const Habit::PhaseRunSettings& prs, int context, unsigned int ntrials, const Habit::StimuliSettings& ss);


#endif /* HEXPERIMENTUTIL_H_ */
