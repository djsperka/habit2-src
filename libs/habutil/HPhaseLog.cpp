/*
 *  HPhaseLog.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/22/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhaseLog.h"
#include <QListIterator>

int habit2::HPhaseLog::totalLookingTime() const
{
	int total=0;
	QListIterator<int> iterator(*this);
	while (iterator.hasNext())
		total += iterator.next();
	return total;
};
