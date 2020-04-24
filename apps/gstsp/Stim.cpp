/*
 * HMMStim.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#include <gst/gst.h>
#include <sstream>
#include "Stim.h"
using namespace hmm;

Stim::~Stim()
{
	// delete source map
	g_print("deleting stim %s with %lu sources\n", m_prefix.c_str(), m_sourceMap.size());
	for (std::pair<HMMStimPosition, SourceP> p : m_sourceMap)
	{
		g_print("about to delete source\n");
		delete p.second;
	}
	m_sourceMap.clear();
	g_print("deleting stim - done\n");
}

Source *Stim::getSource(HMMStimPosition pos)
{
	Source *psrc = NULL;
	if (m_sourceMap.count(pos) == 1)
	{
		psrc = m_sourceMap[pos];
	}
	return psrc;
}


void Stim::addSource(HMMStimPosition pos, Source *src)
{
	src->setParent(this);
	m_sourceMap[pos] = src;
}

void Stim::preroll(GstElement *pipeline, Counter *pc)
{
	m_stimState = HMMStimState::PREROLLING;

	// construct a stim preroll counter....
	//StimPrerollCounter *psc = new StimPrerollCounter(this, m_sourceMap.size());
	for (auto p: m_sourceMap)
	{
		p.second->preroll(pipeline, pc);
	}
}

