/*
 * HMMPort.cpp
 *
 *  Created on: Oct 16, 2019
 *      Author: dan
 */

#include "Port.h"
#include "Stim.h"
#include <exception>
#include <sstream>
#include <QtDebug>

using namespace hmm;

Port::Port()
{
}

Port::~Port()
{
}

void Port::addVideoEle(HMMStimPosition pos, GstElement *ele)
{
	if (m_mapPosVideo.count(pos) > 0)
		throw std::runtime_error("port already has ele at this stim position");
	m_mapPosVideo[pos] = ele;
}

void Port::addAudioEle(HMMStimPosition pos, GstElement *ele)
{
	if (m_mapPosAudio.count(pos) > 0)
		throw std::runtime_error("port already has ele at this stim position");
	m_mapPosAudio[pos] = ele;
}

void Port::addVideoSink(HMMStimPosition pos, GstElement *sink)
{
	if (m_mapPosVideoSink.count(pos) > 0)
		throw std::runtime_error("port already has video sink at this stim position");
	m_mapPosVideoSink[pos] = sink;
}

void Port::addAudioSink(HMMStimPosition pos, GstElement *sink)
{
	if (m_mapPosAudioSink.count(pos) > 0)
		throw std::runtime_error("port already has audio sink at this stim position");
	m_mapPosAudioSink[pos] = sink;
}

void Port::connect(Stim& stim)
{
	// check that the port has nothing connected...
	g_print("Port::connect() - start\n");
	std::ostringstream oss;
	bool bThrow = false;
	for (auto it: m_mapPosVideo)
	{
		GstPad *pad = gst_element_get_static_pad(it.second, "sink");
		if (gst_pad_is_linked(pad))
		{
			bThrow = true;
			oss << "port pad \"" << (int)it.first << " still connected. ";
		}
		gst_object_unref(pad);
	}

	// OK now connect all video streams in 'stim' to the port.
	Stim::HMMStimPosSourceMap::iterator it;
	for (it = stim.sourceMap().begin(); it!= stim.sourceMap().end(); it++)
	{
		qDebug() << "StimPosition: " << (int)(it->first) << "SourceType: " << (int)(it->second->sourceType());
		// does this source have a video stream?
		Stream *pstream = it->second->getStream(HMMStreamType::VIDEO);
		if (pstream)
		{
			g_print("Found a video stream for this source\n");
			if (m_mapPosVideo.count(it->first) > 0)
			{
				GstPad *pad = gst_element_get_static_pad(m_mapPosVideo[it->first], "sink");
				GstPadLinkReturn r = gst_pad_link(pstream->srcpad(), pad);
				if (r != GST_PAD_LINK_OK)
				{
					g_print("pad link error %d\n", (int)r);
					throw std::runtime_error("Cannot link pads.");
				}
				// set offset on the downstream pad, but not when initializing the pipeline.
				GstClock *clock = gst_element_get_clock(m_mapPosVideo[it->first]);
				if (clock)
				{
					GstClockTime abs = gst_clock_get_time(clock);
					GstClockTime base = gst_element_get_base_time(m_mapPosVideo[it->first]);
					gst_pad_set_offset(pad, abs-base);
					gst_object_unref(clock);
				}
				else
				{
					g_print("clock not set, do not set offset on pad....\n");
				}
				gst_object_unref(pad);
			}
			else
			{
				oss << "Stim pos " << (int)it->first << " port does not have video ele at this pos ";
				throw std::runtime_error(oss.str().c_str());
			}
		}
		else
		{
			g_print("NO VIDEO STREAM FOUND for this source\n");
		}
	}
	g_print("Port::connect() - done\n");
}

void Port::disconnect()
{
	// Any pads connected will be unceremoniously disconnected.
	// Any request pads from the mixer will be returned

//	for (std::list<GstPad *>::iterator it = m_listRequestPads.begin(); it != m_listRequestPads.end(); it++)
//	{
//		throw std::runtime_error("HMMPort::disconnect () not implemented for audio");
//	}
	g_print("Port::disconnect() not implemented for audio\n");

	for (HMMPortPosEleMap::iterator it = m_mapPosVideo.begin(); it != m_mapPosVideo.end(); it++)
	{
		GstPad *pad = gst_element_get_static_pad(m_mapPosVideo[it->first], "sink");
		if (gst_pad_is_linked(pad))
		{
			gst_pad_unlink(gst_pad_get_peer(pad), pad);
		}
		gst_object_unref(pad);
	}
}

std::vector<HMMStimPosition> Port::getVideoPositions() const
{
	std::vector<HMMStimPosition> vec;
	for (HMMPortPosEleMap::const_iterator it = m_mapPosVideo.begin(); it!= m_mapPosVideo.end(); it++)
		vec.push_back(it->first);
	return vec;
}

std::vector<HMMStimPosition> Port::getAudioPositions() const
{
	std::vector<HMMStimPosition> vec;
	for (HMMPortPosEleMap::const_iterator it = m_mapPosAudio.begin(); it!= m_mapPosAudio.end(); it++)
		vec.push_back(it->first);
	return vec;
}

void Port::setWidget(HMMStimPosition pos, GValue& value)
{
	// find sink for 'pos'
	if (m_mapPosVideoSink.count(pos) == 1)
	{
		g_object_set_property(G_OBJECT(m_mapPosVideoSink[pos]), "widget", &value);
	}
	else
		throw std::runtime_error("Cannot set port at this pos");
}

