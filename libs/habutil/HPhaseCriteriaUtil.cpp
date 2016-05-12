/*
 *  HPhaseCriteriaUtil.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 3/4/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhaseCriteriaUtil.h"

using namespace Habit;

HPhaseCriteria* createPhaseCriteria(const HabituationSettings& habSettings)
{
	HPhaseCriteria* pcrit = (HPhaseCriteria*)NULL;
	//const CriterionSettings& criterionSettings = habsettings.getCriterionSettings();
	
	if (habSettings.getHabituationType() == HHabituationType::HHabituationTypeFixedN)
	{
		pcrit = new HPhaseFixedNCriteria(habSettings.getNTrials());
	}
	else if (habSettings.getHabituationType() == HHabituationType::HHabituationTypeCriterion)
	{
		pcrit = new HPhaseHabituationCriteria(habSettings.getCriterionSettings(), habSettings.getNTrials());
	}
	else if (habSettings.getHabituationType() == HHabituationType::HHabituationTypeTotalLookingTime)
	{
		// total look time is currently in 10ths of a sec. Must change that to ms....
		// Changed to ms. Do not multiply by 100.  djs 7/15/2013
		pcrit = new HPhaseTotalLookingTimeCriteria(habSettings.getTotalLookLengthToEnd(), habSettings.getNTrials());
	}
	else 
	{
		// Must avoid this possibility!
		qCritical() << "createPhaseCriteria: Habituation Type " << habSettings.getHabituationType().name() << " number " << habSettings.getHabituationType().number();
	}	
	Q_ASSERT_X(pcrit, "createPhaseCriteria", "No phase criteria created. Probably an incorrect type. Check log.");
	return pcrit;
}
