/*
 *  HEventLog.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/31/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HEventLog.h"

habit2::HEventLog::~HEventLog()
{
	while (!isEmpty())
		delete takeFirst();
};

habit2::HPhaseLog habit2::HEventLog::getPhaseLog(QString phase)
{
	bool bHavePhaseStart=false;
	HPhaseLog phaselog;
	QListIterator<habit2::HEvent*> events(*this);
	// Move iterator to phase start event
	if (phase == "")
	{
		events.toBack();
		while (events.hasPrevious())
		{
			HEvent* e = events.previous();
			if (e->type() == kPhaseStart)
			{
				bHavePhaseStart = true;
				events.next();			// leave iterator after the phase start event
				break;
			}
		}
	}
	else 
	{
		events.toFront();
		while (events.hasNext())
		{
			HEvent* e = events.next();
			if (e->type() == kPhaseStart)
			{
				HPhaseStartEvent* pse = static_cast<HPhaseStartEvent*>(e);
				if (pse->phase() == phase)
				{
					bHavePhaseStart = true;
					break;
				}
			}
		}
	}
	
	// Now if we found start of phase, get the look times
	if (bHavePhaseStart)
	{
		bool bInterTrial = true;
		bool bPhaseEnd = false;
		int iTotal = 0;
		habit2::HEvent* pevent;
		while (events.hasNext() && !bPhaseEnd)
		{
			pevent = events.next();
			if (bInterTrial)
			{
				// if we are between trials, looking for 
				// TrialStart event
				switch (pevent->type())
				{
					case habit2::kTrialStart:						
					{
						bInterTrial = false;
						iTotal = 0;
						break;
					}
					case habit2::kPhaseEnd:
					{
						bPhaseEnd = true;
						break;
					}
					default:
					{
						break;
					}
				}
			}
			else 
			{
				// if we are not between trials, then we are in one.
				// We are looking for look events and TrialEnd events. 
				switch (pevent->type())
				{
					case kTrialEnd:
					{
						habit2::HTrialEndEvent* pte = static_cast<habit2::HTrialEndEvent*>(pevent);
						if (pte->endtype() == kTrialEndSuccess)
						{
							phaselog.append(iTotal);
						}
						iTotal = 0;
						bInterTrial = true;
						break;
					}
					case kLook:
					{
						habit2::HLookEvent* plook = static_cast<habit2::HLookEvent*>(pevent);
						iTotal += plook->look().lookMS();
						break;
					}
					default:
					{
						break;
					}
				}
			}
		}
	}
	return phaselog;
}

