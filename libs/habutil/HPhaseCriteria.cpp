/*
 *  HPhaseCriteria.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/9/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhaseCriteria.h"


using namespace Habit;

bool HPhaseFixedNCriteria::isPhaseComplete(const HPhaseLog& log, bool& isHabituated)
{
	bool b = false;
	isHabituated = false;
	if (log.nCompleted() >= m_N)
	{
		b = true;
		isHabituated = true;
	}
	return b;
};

bool HPhaseTotalLookingTimeCriteria::isPhaseComplete(const HPhaseLog& log, bool& isHabituated)
{
	bool b = false;
	bool bdummy=false;	// will be ignored
	isHabituated = false;
	if (log.totalLookingTime() >= m_msTotal)
	{
		b = true;
		isHabituated = true;
	}
	else if (HPhaseFixedNCriteria::isPhaseComplete(log, bdummy))
	{
		b = true;
	}
	return b;
};

bool HPhaseHabituationCriteria::getBasisSum(const HPhaseLog& log, int& iBasisSum, int& iBasisWindowStart)
{
	bool bval = false;
	iBasisSum = 0;
	iBasisWindowStart = -1;
	if (m_c.getBasis() == HCriterionBasisType::HCriterionBasisFirstN)
	{
		if (log.size() >= (int)m_c.getWindowSize())
		{
			bval = getWindowSum(log, iBasisSum, 0);
			if (bval)
				iBasisWindowStart = 0;
		}
		else 
		{
			iBasisWindowStart = -1;
		}
	}
	else 
	{
		int bmax = 0;
		int itemp;
		int iStep = 1;
		if (m_c.getWindowType() == HCriterionWindowType::HCriterionWindowFixed)
			iStep = m_c.getWindowSize();
		iBasisWindowStart = -1;
		
		for (int i=0; i<(log.size()-(int)m_c.getWindowSize()+1); i++)
		{
			if (getWindowSum(log, itemp, i) && itemp > bmax) 
			{
				bmax = itemp;
				iBasisWindowStart = i;
			}
		}
		iBasisSum = bmax;
		if (bmax > 0) bval = true;
	}
	return bval;
};

bool HPhaseHabituationCriteria::getWindowSum(const HPhaseLog& log, int& sum, int ifirst)
{
	sum = 0;
	if (ifirst + (int)m_c.getWindowSize() > log.size()) return false;
	else 
	{
		for (int i=ifirst; i < ifirst + (int)m_c.getWindowSize(); i++)
			sum += log.at(i);
	}
	return true;
};

bool HPhaseHabituationCriteria::isPhaseComplete(const HPhaseLog& log, bool& isHabituated)
{
	bool b = false;
	bool bdummy = false;	// will be ignored
	int iWindowStart = -1;
	int basisSum = 0;
	int windowSum;
	double threshold;
	int iStep = 1;
	isHabituated = false;
	if (getBasisSum(log, basisSum, iWindowStart))
	{
		threshold = (double)basisSum * m_c.getPercent() / 100.0;
		if (m_c.getWindowType() == HCriterionWindowType::HCriterionWindowFixed)
			iStep = m_c.getWindowSize();
		for (int i=iWindowStart; i<log.size(); i+=iStep)
		{
			if (getWindowSum(log, windowSum, i) && windowSum < threshold)
			{
				b = true;
				isHabituated = true;
				break;
			}
		}
	}
	
	// If phase is not complete, check if max number of trials reached.
	if (!b && HPhaseFixedNCriteria::isPhaseComplete(log, bdummy))
	{
		b = true;
	}
	return b;
};
