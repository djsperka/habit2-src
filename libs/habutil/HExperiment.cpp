/*
 * HExperiment.cpp
 *
 *  Created on: Oct 6, 2015
 *      Author: Oakeslab
 */

#include "HExperiment.h"

void HExperiment::onAttention()
{
	eventLog().append(new HAttentionEvent(HElapsedTimer::elapsed()));
}

void HExperiment::onLook(HLook l)
{
	eventLog().append(new HLookEvent(l, HElapsedTimer::elapsed()));
}



