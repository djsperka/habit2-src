/*
 *  HPhaseCriteria.h
 *  habutil
 *
 *  Created by Oakes Lab on 10/9/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HPHASECRITERIA_H_
#define _HPHASECRITERIA_H_

#include "HPhaseLog.h"
#include "criterionsettings.h"



// Abstract class used for determining whether a phase is completed.
// Subclasses must implement isPhaseComplete(). 

class HPhaseCriteria
{
public:
	HPhaseCriteria() {};
	virtual ~HPhaseCriteria() {};
	virtual bool isPhaseComplete(const HPhaseLog& log) = 0;
};


class HPhaseFixedNCriteria: public HPhaseCriteria
{
public:
	HPhaseFixedNCriteria(int N): HPhaseCriteria(), m_N(N) {};
	virtual ~HPhaseFixedNCriteria() {};
	bool isPhaseComplete(const HPhaseLog& log);
private:
	int m_N;
};

class HPhaseTotalLookingTimeCriteria: public HPhaseCriteria
{
public:
	HPhaseTotalLookingTimeCriteria(int msTotal) : HPhaseCriteria(), m_msTotal(msTotal) {};
	virtual ~HPhaseTotalLookingTimeCriteria() {};
	bool isPhaseComplete(const HPhaseLog& log);
private:
	int m_msTotal;
};

class HPhaseHabituationCriteria: public HPhaseCriteria
{
public:
	HPhaseHabituationCriteria(Habit::CriterionSettings c) : HPhaseCriteria(), m_c(c) {};
	virtual ~HPhaseHabituationCriteria() {};
	bool isPhaseComplete(const HPhaseLog& log);
	
	// Get the sum used as basis for the habituation criteria. 
	
	bool getBasisSum(const HPhaseLog& log, int& iBasisSum, int& iBasisWindowStart);
	
	// Get the total looking time for the trials 'window' starting at the trial with index 'ifirst'. 
	// If there are not enough trials completed to make a 'window' then return 0.
	
	bool getWindowSum(const HPhaseLog& log, int& iWindowSum, int ifirst);
private:
	Habit::CriterionSettings m_c;	
};	


#endif