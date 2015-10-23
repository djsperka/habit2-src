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
	Q_UNUSED(es);
	HLookDetector *pld = (HLookDetector *)NULL;
	pld = new HKeypadLookDetector(log, es.getHLookSettings().getInclusiveLookTime(), pdialog);
	return pld;
}

//m_pld = new HKeypadLookDetector(minLook, minLookAway, m_eventLog, this, true, false, true);


