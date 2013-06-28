/*
 * HReliability.cpp
 *
 *  Created on: Jun 17, 2013
 *      Author: Oakeslab
 */

#include "HReliability.h"
#include "HEvents.h"
#include "HElapsedTimer.h"
#include "HLookDetector.h"


bool HReliabilityEndTransition::eventTest(QEvent* e)
{
	bool b = true;

    if (e->type() != QEvent::Type(HReliabilityEndQEvent::ReliabilityEndType))
        b = false;

    return b;
}


bool HReliabilityTransition::eventTest(QEvent* e)
{
	bool blog = true;

    if (e->type() != QEvent::Type(HReliabilityQEvent::ReliabilityType))
        return false;

    // All events are of type HReliabilityQEvent.
    // These events were queued in the state machine's queue using 'postDelayedEvent()'.
    // Each event has an HEvent from the log of the original run.

	HReliabilityQEvent* prevent = static_cast<HReliabilityQEvent*>(e);

	HEvent* phevent = prevent->hevent()->clone(HElapsedTimer::elapsed());

	// Some events require look detector be turned on or off.

	if (phevent->type() == HEventType::HEventPhaseStart)
	{
		// update labels
	}
	else if (phevent->type() == HEventType::HEventPhaseEnd)
	{
		// update  labels
	}
	else if (phevent->type() == HEventType::HEventTrialStart)
	{
		// update  labels
	}
	else if (phevent->type() == HEventType::HEventTrialEnd
		|| phevent->type() == HEventType::HEventAGRequest
		|| phevent->type() == HEventType::HEventAGEnd
		|| phevent->type() == HEventType::HEventStimRequest
		|| phevent->type() == HEventType::HEventStimEnd
		|| phevent->type() == HEventType::HEventExperimentStart)
	{
		// do nothing
	}
	else if (phevent->type() == HEventType::HEventAGStart)
	{
		// turn on ld
		m_ld.enableAGLook();
	}
	else if (phevent->type() == HEventType::HEventStimStart)
	{
		// turn on ld
		m_ld.enableLook();
	}
	else
	{
		blog = false;
	}

	if (blog)
	{
		m_log.append(phevent);
	}
	return true;
}



HReliability::HReliability(HEventLog& log, HLookDetector& ld, HResults& r, QState* parent)
: HLogState(log, "HReliability", parent)
, m_log(log)
, m_ld(ld)
, m_results(r)
{
};


