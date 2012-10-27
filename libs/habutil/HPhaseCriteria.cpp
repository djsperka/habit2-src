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

bool HPhaseFixedNCriteria::isPhaseComplete()
{
	return nCompleted() >= m_N;
};

bool HPhaseTotalLookingTimeCriteria::isPhaseComplete()
{
	return totalLookingTime() >= m_msTotal;
};

bool HPhaseHabituationCriteria::getBasisSum(int& iBasisSum, int& iBasisWindowStart)
{
	bool bval = false;
	iBasisSum = 0;
	iBasisWindowStart = -1;
	if (m_c.getBasis() == CriterionSettings::eFirstN)
	{
		if (m_triallogs.size() >= (int)m_c.getWindowSize())
		{
			bval = getWindowSum(iBasisSum, 0);
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
		if (m_c.getWindowType() == CriterionSettings::eFixedWindow)
			iStep = m_c.getWindowSize();
		iBasisWindowStart = -1;
		
		for (int i=0; i<(m_triallogs.size()-(int)m_c.getWindowSize()+1); i++)
		{
			if (getWindowSum(itemp, i) && itemp > bmax) 
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

bool HPhaseHabituationCriteria::getWindowSum(int& sum, int ifirst)
{
	bool bval = false;
	unsigned int n = 0;
	sum = 0;
	if (ifirst >= m_triallogs.size()) return false;
	else 
	{
		for (int i=ifirst; i<m_triallogs.size() && n < m_c.getWindowSize(); i++)
		{
			if (m_triallogs.at(i).isCompleted()) sum += m_triallogs.at(i).totalLookingTime();
			n++;
		}
	}
	// If there weren't enough to fill the window, return 0
	bval = (n == m_c.getWindowSize());
	return bval;
};

bool HPhaseHabituationCriteria::isPhaseComplete()
{
	bool bval = false;
	int iWindowStart = -1;
	int basisSum = 0;
	int windowSum;
	double threshold;
	int iStep = 1;
	if (getBasisSum(basisSum, iWindowStart))
	{
		threshold = (double)basisSum * m_c.getPercent() / 100.0;
		if (m_c.getWindowType() == CriterionSettings::eFixedWindow)
			iStep = m_c.getWindowSize();
		for (int i=iWindowStart; i<m_triallogs.size(); i+=iStep)
		{
			if (getWindowSum(windowSum, i) && windowSum < threshold)
			{
				bval = true;
				break;
			}
		}
	}
	return bval;
};
