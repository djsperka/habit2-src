/*
 * HMMSource.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#include "Source.h"
using namespace hmm;

Source::Source(HMMSourceType t, GstElement *bin, bool loop, unsigned int volume)
: m_sourceType(t)
, m_bin(bin)
, m_bloop(loop)
{};

void Source::addStream(HMMStreamType t, Stream *pstream)
{
	m_streamMap.insert(std::make_pair(t, pstream));
}

Stream *Source::getStream(HMMStreamType t)
{
	Stream *pstream = NULL;
	if (m_streamMap.count(t) == 1)
		pstream = m_streamMap[t];
	return pstream;
}



