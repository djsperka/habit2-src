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

#include <QObject>
#include <QList>
#include <QDataStream>


// HPhaseLog
//
// Helper class for doing phase criteria tests.
// Derived from an HEventLog, this is a list of ints, each int represents
// the total looking MS for a single _completed_ trial in a particular phase. 
//
// Normally you'd fetch one of these from HEventLog::instance().getPhaseLog("phase_name")
// 


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