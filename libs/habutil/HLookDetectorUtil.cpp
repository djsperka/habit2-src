/*
 * HLookDetectorUtil.cpp
 *
 *  Created on: Jan 15, 2014
 *      Author: Oakeslab
 */

#include "HLookDetectorUtil.h"
#include "HKeypadLookDetector.h"


HLookDetector* createLookDetector(const Habit::ExperimentSettings& es, HEventLog& log, QWidget* pdialog)
{
	HLookDetector *pld = (HLookDetector *)NULL;
	int lookTimeMS = es.getDesignSettings().getPretestTrialsInfo().getLookTimes();
	int lookAwayTimeMS = es.getDesignSettings().getHabituationTrialsInfo().getLookTimes();

	// Assume that we use the keypad look detector.
	// This will change when habit is modified to allow using eye trackers. Presumably the experiment
	// settings, or maybe the RunSettings will have an indication of the look detector type
	// to be used.

	pld = new HKeypadLookDetector(lookTimeMS, lookAwayTimeMS, log, pdialog);
	return pld;
}

//m_pld = new HKeypadLookDetector(minLook, minLookAway, m_eventLog, this, true, false, true);


