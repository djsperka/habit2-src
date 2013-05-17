/*
 * HResultsUtil.h
 *
 *  Created on: May 7, 2013
 *      Author: Oakeslab
 */

#ifndef HRESULTSUTIL_H_
#define HRESULTSUTIL_H_

#include "experimentsettings.h"
#include "runsettings.h"
#include "subjectsettings.h"
#include "HEventLog.h"

bool saveExperimentResults(const QString& filename, const Habit::ExperimentSettings& expSettings, const Habit::RunSettings& runSettings, const Habit::SubjectSettings& subjectSettings, const HEventLog& log);
bool loadExperimentResults(const QString& filename, Habit::ExperimentSettings& expSettings, Habit::RunSettings& runSettings, Habit::SubjectSettings& subjectSettings, HEventLog& log);


#endif /* HRESULTSUTIL_H_ */
