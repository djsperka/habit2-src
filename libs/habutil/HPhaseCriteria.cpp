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

bool HPhaseFixedNCriteria::isPhaseComplete(const HPhaseLog& log)
{
	return log.nCompleted() >= m_N;
};

bool HPhaseTotalLookingTimeCriteria::isPhaseComplete(const HPhaseLog& log)
{
	return log.totalLookingTime() >= m_msTotal;
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

bool HPhaseHabituationCriteria::isPhaseComplete(const HPhaseLog& log)
{
	bool bval = false;
	int iWindowStart = -1;
	int basisSum = 0;
	int windowSum;
	double threshold;
	int iStep = 1;
	if (getBasisSum(log, basisSum, iWindowStart))
	{
		threshold = (double)basisSum * m_c.getPercent() / 100.0;
		if (m_c.getWindowType() == HCriterionWindowType::HCriterionWindowFixed)
			iStep = m_c.getWindowSize();
		for (int i=iWindowStart; i<log.size(); i+=iStep)
		{
			if (getWindowSum(log, windowSum, i) && windowSum < threshold)
			{
				bval = true;
				break;
			}
		}
	}
	return bval;
};
