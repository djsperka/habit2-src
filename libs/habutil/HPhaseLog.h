/*
 *  HPhaseLog.h
 *  habutil
 *
 *  Created by Oakes Lab on 10/22/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HPHASELOG_H_
#define _HPHASELOG_H_

#include "HTrialLog.h"
#include <QObject>
#include <QList>
#include <QDataStream>



namespace habit2
{

	// List of integers. Each int is the total looking time for a single
	// completed trial. 
	class HPhaseLog: public QList<int>
	{

	public:
		HPhaseLog() : QList<int>() {};
		virtual ~HPhaseLog() {};

		int nCompleted() const { return size(); };
		int totalLookingTime() const;
	};

};
#endif