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

class HPhaseCriteria : public HPhaseLog
{
	Q_OBJECT
public:
	HPhaseCriteria() : HPhaseLog() {};
	virtual ~HPhaseCriteria() {};
	virtual bool isPhaseComplete() = 0;
};


class HPhaseFixedNCriteria: public HPhaseCriteria
{
	Q_OBJECT
public:
	HPhaseFixedNCriteria(int N): HPhaseCriteria(), m_N(N) {};
	virtual ~HPhaseFixedNCriteria() {};
	bool isPhaseComplete();
private:
	int m_N;
};

class HPhaseTotalLookingTimeCriteria: public HPhaseCriteria
{
	Q_OBJECT
public:
	HPhaseTotalLookingTimeCriteria(int msTotal) : HPhaseCriteria(), m_msTotal(msTotal) {};
	virtual ~HPhaseTotalLookingTimeCriteria() {};
	bool isPhaseComplete();
private:
	int m_msTotal;
};

class HPhaseHabituationCriteria: public HPhaseCriteria
{
	Q_OBJECT
public:
	HPhaseHabituationCriteria(Habit::CriterionSettings c) : HPhaseCriteria(), m_c(c) {};
	virtual ~HPhaseHabituationCriteria() {};
	bool isPhaseComplete();
	
	// Get the sum used as basis for the habituation criteria. 
	
	bool getBasisSum(int& iBasisSum, int& iBasisWindowStart);
	
	// Get the total looking time for the trials 'window' starting at the trial with index 'ifirst'. 
	// If there are not enough trials completed to make a 'window' then return 0.
	
	bool getWindowSum(int& iWindowSum, int ifirst);
private:
	Habit::CriterionSettings m_c;	
};	


#endif