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

HPhaseCriteria* createPhaseCriteria(const HabituationSettings& habsettings, int ntrials)
{
	HPhaseCriteria* pcrit = (HPhaseCriteria*)NULL;
	//const CriterionSettings& criterionSettings = habsettings.getCriterionSettings();
	
	if (habsettings.getHabituationType() == HHabituationType::HHabituationTypeFixedN)
	{
		pcrit = new HPhaseFixedNCriteria(ntrials);
	}
	else if (habsettings.getHabituationType() == HHabituationType::HHabituationTypeCriterion)
	{
		pcrit = new HPhaseHabituationCriteria(habsettings.getCriterionSettings(), ntrials);		
	}
	else if (habsettings.getHabituationType() == HHabituationType::HHabituationTypeTotalLookingTime)
	{
		// total look time is currently in 10ths of a sec. Must change that to ms....
		// Changed to ms. Do not multiply by 100.  djs 7/15/2013
		pcrit = new HPhaseTotalLookingTimeCriteria(habsettings.getTotalLookLengthToEnd(), ntrials);
	}
	else 
	{
		// Must avoid this possibility!
		qCritical() << "createPhaseCriteria: Habituation Type " << habsettings.getHabituationType().name() << " number " << habsettings.getHabituationType().number();
	}	
	Q_ASSERT_X(pcrit, "createPhaseCriteria", "No phase criteria created. Probably an incorrect type. Check log.");
	return pcrit;
}
