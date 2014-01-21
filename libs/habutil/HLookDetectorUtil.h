/*
 * HLookDetectorUtil.h
 *
 *  Created on: Jan 15, 2014
 *      Author: Oakeslab
 */

#ifndef HLOOKDETECTORUTIL_H_
#define HLOOKDETECTORUTIL_H_

#include "HLookDetector.h"
#include "experimentsettings.h"

HLookDetector* createLookDetector(const Habit::ExperimentSettings& es, HEventLog& log, QWidget* pdialog);


#endif /* HLOOKDETECTORUTIL_H_ */
