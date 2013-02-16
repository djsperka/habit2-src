/*
 *  HExperimentChildState.h
 *  habutil
 *
 *  Created by Oakes Lab on 1/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _H_EXPERIMENTCHILDSTATE_H_
#define _H_EXPERIMENTCHILDSTATE_H_

#include "HLogState.h"
#include "HExperiment.h"

class HExperimentChildState: public HLogState
{
	Q_OBJECT
	
public:
	HExperimentChildState(HExperiment& exp, HEventLog& log, const QString& name) 
	: HLogState(log, name, &exp)
	, m_exp(exp)
	{};

	HExperimentChildState(HExperiment& exp, HEventLog& log, const QString& name, const QString& prefix, QState* parent = 0)
	: HLogState(log, name, prefix, parent)
	, m_exp(exp)
	{};

	HExperiment& experiment() { return m_exp; };

private:
	HExperiment& m_exp;
};
					 
					 
					 
#endif