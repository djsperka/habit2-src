/*
 * MM1.cpp
 *
 *  Created on: Sep 6, 2019
 *      Author: dan
 */

#include "MM1.h"
#include <stdexcept>


MM1::MM1(bool bVideo, bool bAudio, const std::string& bkgd)
: m_bVideo(bVideo)
, m_bAudio(bAudio)
, m_uiCurrentSource(0)
{
	m_pipeline = gst_pipeline_new (NULL);
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	gst_bus_add_watch(bus, &MM1::busCallback, this);
	gst_object_unref(bus);


	if (m_bVideo)
	{
		m_conv = gst_element_factory_make ("videoconvert", NULL);
		m_scale = gst_element_factory_make ("videoscale", NULL);
		m_vsink = gst_element_factory_make("autovideosink", NULL);
		gst_object_ref(m_conv);
		gst_object_ref(m_scale);
		gst_object_ref(m_vsink);
		gst_bin_add_many(GST_BIN(m_pipeline), m_conv, m_scale, m_vsink, NULL);
		if (!gst_element_link_many (m_conv, m_scale, m_vsink, NULL))
		{
			throw new std::runtime_error("Cannot link conv-scale-sink");
		}

		// set up background
		GError *err=NULL;
		m_bkgd = gst_parse_bin_from_description(bkgd.c_str(), TRUE, &err);
		if (!m_bkgd)
		{
			throw new std::runtime_error("bad bkgd description");
		}
		gst_object_ref(m_bkgd);
		gst_bin_add(GST_BIN(m_pipeline), m_bkgd);
		if (!gst_element_link(m_bkgd, m_conv))
		{
			throw new std::runtime_error("Cannot link bkgd into pipeline");
		}
	}

	if (m_bAudio)
	{
		g_print("Audio not implemented\n");
	}

	gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

}

MM1::~MM1()
{
	gst_element_set_state(m_pipeline, GST_STATE_NULL);

}

// add a (single) file as a source
// note, returned index is NOT the index into m_sources - reserve index 0 for background.
// Use returned index for prerolling, playing.

unsigned int MM1::addSource(const std::string& filename)
{
	SourceStruct ss;
	ss.filename = filename;
	ss.filesrc = ss.dbin = NULL;
	m_sources.push_back(ss);
	return m_sources.size();
}

bool MM1::preroll(unsigned int id)
{
	if (id < 1 || id > m_sources.size())
	{
		g_print("cannot preroll, no such source (%u)\n", id);
		return false;
	}
	else
	{
		g_print("prerolling %u\n", id);
		m_sources[id].filesrc = gst_element_factory_make ("filesrc", NULL);
		m_sources[id].dbin = gst_element_factory_make ("decodebin", NULL);
		g_object_set (m_sources[id].filesrc, "location", m_sources[id].filename, NULL);
		gst_object_ref(m_sources[id].filesrc);
		gst_object_ref(m_sources[id].dbin);
		gst_bin_add_many (GST_BIN (m_pipeline), m_sources[id].filesrc, m_sources[id].dbin, NULL);
		if (!gst_element_link_many (m_sources[id].filesrc, m_sources[id].dbin, NULL))
		{
			g_error ("Failed to link elements");
		}
		g_signal_connect (m_sources[id].dbin, "pad-added", G_CALLBACK(pad_added_cb), NULL);
		gst_element_sync_state_with_parent (m_sources[id].filesrc);
		gst_element_sync_state_with_parent (m_sources[id].dbin);
	}
}

gboolean MM1::busCallback(GstBus *, GstMessage *msg, gpointer p)
{
	g_print("MM1::busCallback(%s - %s)\n", GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME(msg));
	return TRUE;
}

void mm1::padAddedCallback(GstElement * element, GstPad * pad, gpointer user_data)
{
	GstCaps *caps;
	GstStructure *s;
	const gchar *name;

	caps = gst_pad_get_current_caps (pad);
	s = gst_caps_get_structure (caps, 0);
	name = gst_structure_get_name (s);

	g_print("padAddedCallback: %s\n", name);
	if (strcmp (name, "video/x-raw") == 0)
	{
		// block pad
		stim->vsrcpad = pad;
		stim->haveV = true;
		gst_object_ref(stim->vsrcpad);
		stim->probeidV = gst_pad_add_probe (stim->vsrcpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, pad_probe_block_cb, user_data, NULL);
	}

	gst_caps_unref (caps);
}
