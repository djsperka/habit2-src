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
	for (std::pair<HMMStimPosition, SourceP> p : m_sourceMap)
	{
		delete p.second;
	}
	m_sourceMap.clear();
}

Source *Stim::getSource(HMMStimPosition pos)
{
	Source *psrc = NULL;
	g_print("stim has %lu sources\n", m_sourceMap.size());
	for (HMMStimPosSourceMap::iterator it = m_sourceMap.begin(); it!= m_sourceMap.end(); it++)
	{
		g_print("Source at position ", (int)it->first);
	}
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

//void Stim::addSource(HMMStimPosition pos, HMMSourceType stype, GstElement *pipeline, const std::string& filename, bool loop, int volume)
//{
//	FileSource *pfilesource = new FileSource(stype, this, filename, loop, volume, pipeline);
//	//gst_bin_add(GST_BIN(pipeline), pfilesource->bin());
//	m_sourceMap[pos] = pfilesource;
//}

void Stim::preroll(GstElement *pipeline)
{
	m_stimState = HMMStimState::PREROLLING;

	// construct a stim preroll counter....
	StimPrerollCounter *psc = new StimPrerollCounter(this, m_sourceMap.size());
	for (auto p: m_sourceMap)
	{
		p.second->preroll(pipeline, psc);
	}
}

//hmm::Source *HabitStimFactory::makeSourceFromFile(GstElement *pipeline, const std::string& filename, HMMSourceType stype, void *userdata, bool loop, unsigned int volume)
//{
//	std::string uri("file://");
//	uri.append(filename);
//	g_print("HMM::makeSourceFromFile(%s)\n", uri.c_str());
//	GstElement *ele = gst_element_factory_make("uridecodebin", NULL);
//	if (!ele)
//		g_print("NULL ele!\n");
//	g_object_set (ele, "uri", uri.c_str(), NULL);
//
//	// add ele to pipeline. pipeline takes ownership, will unref when ele is removed.
//	if (!gst_bin_add(GST_BIN(pipeline), ele))
//		g_print("ERROR- cannot add uridecodebin\n");
//
//	// Source does not ref the ele, assumes that its in a pipeline and ref'd there.
//	// TODO: not sure I like how ownership seems ill-defined.
//	Source *psrc = new Source(stype, ele, loop, volume);
//
//	g_signal_connect (psrc->bin(), "pad-added", G_CALLBACK(HMM::padAddedCallback), userdata);
//	g_signal_connect (psrc->bin(), "no-more-pads", G_CALLBACK(HMM::noMorePadsCallback), userdata);
//
//	return psrc;
//}

