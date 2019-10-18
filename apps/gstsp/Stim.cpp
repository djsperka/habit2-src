/*
 * HMMStim.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#include "Stim.h"
using namespace hmm;

void Stim::addSource(HMMStimPosition pos, Source *psrc)
{
	m_sourceMap.insert(make_pair(pos, source_ptr(psrc)));
}

Source *Stim::getSource(HMMStimPosition pos)
{
	Source *psrc = NULL;
	if (m_sourceMap.count(pos) == 1)
	{
		psrc = m_sourceMap[pos].get();
	}
	return psrc;
}



