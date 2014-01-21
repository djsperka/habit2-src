/*
 * HLookDetectorUtil.cpp
 *
 *  Created on: Jan 15, 2014
 *      Author: Oakeslab
 */

#include "HLookDetectorUtil.h"


HLookDetector* createLookDetector(const Habit::ExperimentSettings& es, HEventLog& log, QWidget* pdialog)
{
	HLookDetector *pld = (HLookDetector *)NULL;
	int lookTimeMS = es.getDesignSettings().getPretestTrialsInfo().getLookTimes();
	int lookAwayTimeMS = es.getDesignSettings().getHabituationTrialsInfo().getLookTimes();
	pld = new HKeypadLookDetector(lookTimeMS, lookAwayTimeMS, log, this);
	return pld;
}



