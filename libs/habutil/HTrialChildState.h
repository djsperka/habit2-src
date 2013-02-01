/*
 *  HTrialChildState.h
 *  habutil
 *
 *  Created by Oakes Lab on 1/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _HTRIALCHILDSTATE_H_
#define _HTRIALCHILDSTATE_H_

#include <QString>
#include "HLogState.h"

class HTrial;

class HTrialChildState: public HLogState
{
	Q_OBJECT
	
public:
	HTrialChildState(HTrial& trial, HEventLog& log, const QString& name, QState* parent = 0)
	: HLogState(log, name, parent)
	, m_trial(trial)
	{};
	
	virtual ~HTrialChildState() {};
	
	HTrial& trial() { return m_trial; };
	
private:
	HTrial& m_trial;
};		

#endif