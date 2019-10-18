/*
 * HMMSource.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#include "HMMSource.h"

HMMSource::HMMSource(HMMSourceType t, GstElement *bin, bool loop)
: m_sourceType(t)
, m_bin(bin)
, m_bloop(loop)
{};

void HMMSource::addStream(HMMStreamType t, HMMStream *pstream)
{
	m_streamMap.insert(std::make_pair(t, stream_ptr(pstream)));
}

HMMStream *HMMSource::getStream(HMMStreamType t)
{
	HMMStream *pstream = NULL;
	if (m_streamMap.count(t) == 1)
		pstream = m_streamMap[t].get();
	return pstream;
}



