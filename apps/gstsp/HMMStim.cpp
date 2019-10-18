/*
 * HMMStim.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#include "HMMStim.h"

void HMMStim::addSource(HMMStimPosition pos, HMMSource *psrc)
{
	m_sourceMap.insert(make_pair(pos, source_ptr(psrc)));
}

HMMSource *HMMStim::getSource(HMMStimPosition pos)
{
	HMMSource *psrc = NULL;
	if (m_sourceMap.count(pos) == 1)
	{
		psrc = m_sourceMap[pos].get();
	}
	return psrc;
}



