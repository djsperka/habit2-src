/*
 *  HPhaseCriteria.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/9/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhaseCriteria.h"
#include "HEvents.h"
#include <algorithm>


using namespace Habit;

bool HPhaseFixedNCriteria::isPhaseComplete(const HPhaseLog& log, bool& isHabituated, int& eventNumber)
{
	bool b = false;
	isHabituated = false;
	eventNumber = HEventType::HEventUndefined.number();	// no events
	if (log.nCompleted() >= m_N)
	{
		b = true;
		isHabituated = true;
	}
	return b;
};

bool HPhaseTotalLookingTimeCriteria::isPhaseComplete(const HPhaseLog& log, bool& isHabituated, int& eventNumber)
{
	bool b = false;
	bool bdummy=false;	// will be ignored
	int ignored=0;		// this too
	isHabituated = false;
	eventNumber = HEventType::HEventUndefined.number();	// no events, unless see below
	if (log.totalLookingTime() >= m_msTotal)
	{
		b = true;
		isHabituated = true;
		eventNumber = HEventType::HHabituationSuccess.number();
	}
	else if (HPhaseFixedNCriteria::isPhaseComplete(log, bdummy, ignored))
	{
		b = true;
		eventNumber = HEventType::HHabituationFailure.number();
	}
	return b;
};

bool HPhaseHabituationCriteria::getBasisSum(const HPhaseLog& log, int& iBasisSum, int& iBasisWindowStart)
{
	bool bval = false;
	int iTempSum = 0;
	int iStep = 1;
	if (m_c.getWindowType() == HCriterionWindowType::HCriterionWindowFixed)
		iStep = m_c.getWindowSize();
	iBasisSum = 0;
	iBasisWindowStart = -1;
	if (m_c.getBasis() == HCriterionBasisType::HCriterionBasisFirstN)
	{
		// When a min basis value is NOT required, just check the first n trials.
		// If a valid sum is gotten (i.e. if there are at least n trials) then a
		// basis will be found.
		if (!m_c.getRequireMinBasisValue())
		{
			if (getWindowSum(log, iTempSum, 0))
			{
				bval = true;
				iBasisSum = iTempSum;
				iBasisWindowStart = 0;
			}
		}
		else
		{
			// In the case where a min basis value IS REQUIRED, then we take the
			// first n trials that satisfy the min basis requirement.
			for (int i=0; !bval && getWindowSum(log, iTempSum, i); i+=iStep)
			{
				if (iTempSum > (int)m_c.getMinBasisValue())
				{
					bval = true;
					iBasisSum = iTempSum;
					iBasisWindowStart = i;
				}
			}

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

		for (int i=0; getWindowSum(log, itemp, i); i+=iStep)
		{
			if (itemp > bmax)
			{
				bmax = itemp;
				iBasisWindowStart = i;
			}
		}

		if ((m_c.getRequireMinBasisValue() && bmax > (int)m_c.getMinBasisValue()) ||
			(!m_c.getRequireMinBasisValue() && bmax > 0))
		{
			iBasisSum = bmax;
			bval = true;
		}

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

bool HPhaseHabituationCriteria::isPhaseComplete(const HPhaseLog& log, bool& isHabituated, int& eventNumber)
{
	bool b = false;
	bool bdummy = false;	// will be ignored
	int iWindowStart = -1;
	int basisSum = 0;
	int windowSum;
	double threshold;
	int iStep = 1;
	isHabituated = false;
	eventNumber = HEventType::HEventUndefined.number();
	if (getBasisSum(log, basisSum, iWindowStart))
	{
		threshold = (double)basisSum * m_c.getPercent() / 100.0;
		if (m_c.getWindowType() == HCriterionWindowType::HCriterionWindowFixed)
			iStep = m_c.getWindowSize();
		if (m_c.getExcludeBasisWindow())
			iWindowStart += m_c.getWindowSize();
		for (int i=iWindowStart; i<std::min<int>(log.size(), getN()-m_c.getWindowSize()+1); i+=iStep)
		{
			if (getWindowSum(log, windowSum, i) && windowSum < threshold)
			{
				b = true;
				isHabituated = true;
				eventNumber = HEventType::HHabituationSuccess.number();
				break;
			}
		}
	}
	
	// If phase is not complete, check if max number of trials reached.
	int ignored;
	if (!b && HPhaseFixedNCriteria::isPhaseComplete(log, bdummy, ignored))
	{
		b = true;
		eventNumber = HEventType::HHabituationFailure.number();
	}
	return b;
};
