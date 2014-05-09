/*
 *  HLogState.h
 *  habutil
 *
 *  Created by Oakes Lab on 1/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HLOGSTATE_H_
#define _HLOGSTATE_H_

#include "HState.h"
#include "HEventLog.h"

/*
 * Classes derived from this will be able to access the event log for 
 * the experiment. 
 */

class HLogState: public HState
{
//	Q_OBJECT
	
public:
	HLogState(HEventLog& log, const QString& name, QState* parent = 0)
	: HState(name, parent)
	, m_eventLog(log)
	{};
	
	HLogState(HEventLog& log, const QString& name, const QString& prefix, QState* parent = 0)
	: HState(name, prefix, parent)
	, m_eventLog(log)
	{};
	
	HEventLog& eventLog() { return m_eventLog; };

private:
	HEventLog& m_eventLog;

};



#endif
