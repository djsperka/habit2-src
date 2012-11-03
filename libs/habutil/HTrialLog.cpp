/*
 *  HTrialLog.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/16/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HTrialLog.h"

int HTrialLog::totalLookingTime() const
{
	int total = 0;
	foreach(const HLook *plook, *this)
	{
		total += plook->lookMS();
	}
	return total;
}

bool operator==(const HTrialLog& lhs, const HTrialLog& rhs)
{
	return lhs.getStatus()==rhs.getStatus() && static_cast<const QList<const HLook *>&>(lhs) == static_cast<const QList<const HLook *>&>(rhs);
}

