/*
 * HMMPort.cpp
 *
 *  Created on: Oct 16, 2019
 *      Author: dan
 */

#include "HMMPort.h"
#include "HMMStim.h"
#include <exception>
#include <sstream>
#include <QtDebug>

HMMPort::HMMPort()
: m_mixer(nullptr)
{
}

HMMPort::~HMMPort()
{
}

void HMMPort::addVideoEle(HMMStimPosition pos, GstElement *ele)
{
	if (m_mapPosEle.count(pos) > 0)
		throw std::runtime_error("port already has ele at this stim position");
	m_mapPosEle[pos] = ele;
}

void HMMPort::addAudioMixer(GstElement *mixer)
{
	if (m_mixer)
		throw std::runtime_error("port has audio mixer, cannot add another");
	m_mixer = mixer;
}

void HMMPort::connect(HMMStim& stim)
{
	// check that the port has nothing connected...
	std::ostringstream oss;
	bool bThrow = false;
	for (auto it: m_mapPosEle)
	{
		GstPad *pad = gst_element_get_static_pad(it.second, "sink");
		if (gst_pad_is_linked(pad))
		{
			bThrow = true;
			oss << "port pad \"" << (int)it.first << " still connected. ";
		}
		gst_object_unref(pad);
	}

	if (!m_listRequestPads.empty())
	{
		bThrow = true;
		oss << "audio request pads still present. ";
	}

//	if (bThrow)
//		throw std::runtime_error(oss.str().c_str());

	// OK now connect all video streams in 'stim' to the port.
	//
	HMMStim::HMMStimPosSourceMap::iterator it;
	for (it = stim.sourceMap().begin(); it!= stim.sourceMap().end(); it++)
	{
		qDebug() << "StimPosition: " << (int)(it->first) << "SourceType: " << (int)(it->second->sourceType());
		// does this source have a video stream?
		HMMStream *pstream = it->second->getStream(HMMStreamType::VIDEO);
		if (pstream)
		{
			if (m_mapPosEle.count(it->first) > 0)
			{
				GstPad *pad = gst_element_get_static_pad(m_mapPosEle[it->first], "sink");
				gst_pad_link(pstream->srcpad(), pad);
				GstClockTime abs = gst_clock_get_time(gst_element_get_clock(m_mapPosEle[it->first]));
				GstClockTime base = gst_element_get_base_time(m_mapPosEle[it->first]);
				gst_pad_set_offset(pad, abs-base);
				gst_object_unref(pad);
			}
		}
	}
}

void HMMPort::disconnect()
{
	// Any pads connected will be unceremoniously disconnected.
	// Any request pads from the mixer will be returned

	for (std::list<GstPad *>::iterator it = m_listRequestPads.begin(); it != m_listRequestPads.end(); it++)
	{
		throw std::runtime_error("HMMPort::disconnect () not implemented for audio");
	}

	for (HMMPortPosEleMap::iterator it = m_mapPosEle.begin(); it != m_mapPosEle.end(); it++)
	{
		GstPad *pad = gst_element_get_static_pad(m_mapPosEle[it->first], "sink");
		if (gst_pad_is_linked(pad))
		{
			gst_pad_unlink(gst_pad_get_peer(pad), pad);
		}
		gst_object_unref(pad);
	}
}

