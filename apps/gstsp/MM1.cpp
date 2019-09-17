/*
 * MM1.cpp
 *
 *  Created on: Sep 6, 2019
 *      Author: dan
 */

#include "MM1.h"
#include <stdexcept>

const MMStimPosition VIDEO_POS(1);

void MMStim::addSource(MMStimPosition pos, MMSource* psrc)
{
	m_sourcemap.insert(pos, psrc);
}

std::string MMStim::uri() const
{
	std::string s("file://");
	s.append(filename);
	return s;
}

void MMStim::set_counter(int c)
{
	g_atomic_int_set(&counter, c);
}

bool MMStim::dec_counter()
{
	if (g_atomic_int_dec_and_test(&counter))
	{
		if (stimState == MMStimState::PREROLLING)
		{
			prerolled = true;
			g_print("Stim is prerolled\n");
		}
		else if (stimState == MMStimState::PENDING_PAUSE)
		{
			g_print("Pause complete - post msg on bus\n");
			GstBus *bus;
			bus = gst_pipeline_get_bus (GST_PIPELINE (m_pipeline));
			gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST (m_pipeline), gst_structure_new_empty ("swap")));
			gst_object_unref (GST_OBJECT (bus));
		}
	}
	g_print("MMStim: counter is %d\n", counter);
	return prerolled;
}

void MMSource::inc_counter()
{
	g_atomic_int_inc(&counter);
	g_print("inc: counter is %d\n", counter);
}

bool MMSource::dec_counter()
{
	if (g_atomic_int_dec_and_test(&counter))
	{
		MMStim *pmmstim = (MMStim *)pstim;
		if (stimState == MMStimState::PREROLLING)
		{
			prerolled = true;
			g_print("prerolled\n");
			pmmstim->dec_counter();
		}
		else if (stimState == MMStimState::PENDING_PAUSE)
		{
			g_print("Source is paused\n");
			pmmstim->dec_counter();
		}
	}
	g_print("dec: counter is %d\n", counter);
	return prerolled;
}

void MMSource::set_counter(int c)
{
	counter = c;
}

void MMSource::addStream(MMStream *pstream)
{
	streams.push_back(pstream);
}

MM1::MM1(bool bVideo, bool bAudio, const std::string& bkgd)
: m_bVideo(bVideo)
, m_bAudio(bAudio)
, m_ulCurrentStim(0)
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
		gst_element_set_name(m_bkgd, "background");
		gst_bin_add(GST_BIN(m_pipeline), m_bkgd);
		if (!gst_element_link(m_bkgd, m_conv))
		{
			throw new std::runtime_error("Cannot link bkgd into pipeline");
		}

		// Create MMStim and MMSource for bkgd
		MMStim *pstim = new MMStim(bkgd, m_pipeline);
		m_ulBkgd = m_ulCurrentStim = addStim(pstim);

		MMSource *pmmsrc = new MMSource();
		pmmsrc->ele = m_bkgd;

		pmmsrc->sourceType = MMSourceType::VIDEO_ONLY;
		pmmsrc->set_counter(1);
		pmmsrc->prerolled = false;
		pmmsrc->pstim = pstim;

		// and a stream for the source

		// tmp get bin src pad
		GstPad *tmppad;
		GstPad *tmppadSink;

		tmppadSink = gst_element_get_static_pad(m_conv, "sink");
		tmppad = gst_pad_get_peer(tmppadSink);	// note this will not be unref'd, as we're keeping it in pmmstream
		gst_object_unref(tmppadSink);

		MMStream *pmmstream = new MMStream();
		pmmstream->isVideo = true;
		pmmstream->srcpad = tmppad;
		pmmsrc->addStream(pmmstream);
		pstim->addSource(VIDEO_POS, pmmsrc);
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
// note, returned index is NOT the index into sources - reserve index 0 for background.
// Use returned index for prerolling, playing.

unsigned int MM1::addStim(const std::string& filename)
{
	MMStim *pstim = new MMStim(filename, m_pipeline);
	size_t index = stimMap.size() + 1;
	stimMap.insert(index, pstim);
	return index;
}

unsigned int MM1::addStim(MMStim* pstim)
{
	size_t index = stimMap.size() + 1;
	stimMap.insert(index, pstim);
	return index;
}

std::string MM1::getUri(const std::string& filename)
{
	std::string s("file://");
	s.append(filename);
	return s;
}

bool MM1::hasStim(unsigned long id)
{
	return stimMap.count(id)>0;
}

const MMStim& MM1::getStim(unsigned long id)
{
	return stimMap.at(id);
}

void MM1::preroll(unsigned long id)
{
	if (stimMap.count(id) != 1)
		throw std::runtime_error("id not in stimMap, cannot preroll.");

	g_print("prerolling %lu\n", id);
	MMStim& mmstim = stimMap.at(id);

	// at this point we would set in motion the preroll for _each_of_the_sources_, e.g. left,
	// right, ISS, or whatever topology is in use.
	//
	// Prerolling the stim means prerolling each individual source. Only when all sources are prerolled
	// is the stim fully prerolled.
	//
	// instead, assume a single source, video only
	MMSource *pmmsrc = new MMSource();
	pmmsrc->ele = gst_element_factory_make("uridecodebin", NULL);
	g_object_set (pmmsrc->ele, "uri", mmstim.uri().c_str(), NULL);
	gst_object_ref(pmmsrc->ele);
	gst_bin_add(GST_BIN(m_pipeline), pmmsrc->ele);

	pmmsrc->sourceType = MMSourceType::VIDEO_ONLY;
	pmmsrc->set_counter(1);
	pmmsrc->prerolled = false;
	pmmsrc->pstim = &mmstim;
	pmmsrc->set_stim_state(MMStimState::PREROLLING);
	mmstim.addSource(VIDEO_POS, pmmsrc);
	mmstim.set_counter(1);			// this is the total number of sources (files) getting prerolled
	mmstim.set_stim_state(MMStimState::PREROLLING);

	// no linking, just deal with pad-added
	g_signal_connect (pmmsrc->ele, "pad-added", G_CALLBACK(MM1::padAddedCallback), pmmsrc);
	g_signal_connect (pmmsrc->ele, "no-more-pads", G_CALLBACK(MM1::noMorePadsCallback), pmmsrc);

	gst_element_sync_state_with_parent (pmmsrc->ele);

	return;
}

void MM1::play(unsigned long id)
{
	if (stimMap.count(id) != 1)
		throw std::runtime_error("id not in stimMap, cannot play.");

	g_print("play %lu\n", id);
	MMStim& mmstim = stimMap.at(id);

	if (!mmstim.isPrerolled())
		throw std::runtime_error("id found, not prerolled. Preroll first.");

	// The stim in 'mmstim' is prerolled and ready to play. It is a partial pipeline,
	// with a set of src pads matching the preferred pattern (number of video, audio, and audio source(s).
	// The stim in 'mmstimCurrent' is the currently playing stim.
	//
	// Our goal is to make mmstim the currently running stim.
	//
	// First, block ALL of the currently running stim. To do that we set blocking probes on each of the
	// connected src pads coming from that stim (all those src pads mentioned above). When we know that ALL
	// of the streams are blocked, we can then detach the mmstimCurrent, and attach mmstim.
	// One detail that is important is to set the offset on the sink pad side of the new connection. The offset
	// should be the current running time (VERIFY THIS)

	// The stopping process requires us to set two vars: a counter - the number of sources in the stim, and
	// the state - to indicate to dec_counter just what it should do when the counter hits zero.
	// The counter is decremented inside the blocking probe(s) - thus by setting the counter to the number
	// of streams and then setting a blocking probe on each, we can block and know exactly WHEN the last source
	// is fully blocked. A similar scheme is employed at the source level (counter for total number of streams
	// used)

	MMStim& mmstimCurrent = stimMap.at(m_ulCurrentStim);
	mmstimCurrent.set_stim_state(MMStimState::PENDING_PAUSE);
	mmstimCurrent.set_counter(mmstimCurrent.sourcemap().size());
	g_print("this stim has %lu sources\n", mmstimCurrent.sourcemap().size());
	for (boost::ptr_map<MMStimPosition, MMSource>::const_iterator it=mmstimCurrent.sourcemap().begin();
			it != mmstimCurrent.sourcemap().end(); it++)
	{
//		gchar* name = gst_element_get_name(it->ele);
//		g_print("will play stim %s: source %s\n", mmstim.uri().c_str(), name);
//		g_free(name);
		it->second->set_stim_state(MMStimState::PENDING_PAUSE);
		it->second->set_counter(it->second->streams.size());
		g_print("stim stream counter set to %lu\n", it->second->streams.size());
		for (boost::ptr_list<MMStream>::iterator iq=it->second->streams.begin();
					iq != it->second->streams.end(); iq++)
		{
			iq->probeid = gst_pad_add_probe(iq->srcpad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &MM1::padProbeBlockCallback, it->second, NULL);
		}
	}
	// in order to play the new stim we have to
	// 1. block the currently playing stim
	// 2. splice in the (preerolled) stim
	// 3. set the pad offset on the receiving pad



}

gboolean MM1::busCallback(GstBus *bus, GstMessage *msg, gpointer user_data)
{
	//g_print("MM1::busCallback(%s - %s)\n", GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME(msg));
	MM1* pmm1 = (MM1 *)user_data;
	switch (GST_MESSAGE_TYPE (msg))
	{
	case GST_MESSAGE_ERROR:
	{
		GError *err = NULL;
		gchar *name, *debug = NULL;

		name = gst_object_get_path_string (msg->src);
		gst_message_parse_error (msg, &err, &debug);

		g_printerr ("ERROR: from element %s: %s\n", name, err->message);
		if (debug != NULL)
		g_printerr ("Additional debug info:\n%s\n", debug);

		g_error_free (err);
		g_free (debug);
		g_free (name);

		// TODO should throw ex here
		//g_main_loop_quit (loop);
		break;
	}
	case GST_MESSAGE_WARNING:
	{
		GError *err = NULL;
		gchar *name, *debug = NULL;

		name = gst_object_get_path_string (msg->src);
		gst_message_parse_warning (msg, &err, &debug);

		g_printerr ("ERROR: from element %s: %s\n", name, err->message);
		if (debug != NULL)
		g_printerr ("Additional debug info:\n%s\n", debug);

		g_error_free (err);
		g_free (debug);
		g_free (name);
		break;
	}
	case GST_MESSAGE_EOS:
	{
		g_print ("Got EOS\n");
		//g_main_loop_quit (loop);
		break;
	}
	case GST_MESSAGE_APPLICATION:
	{
		if (gst_message_has_name (msg, "swap"))
		{
			/* it's our message */
			g_print ("Ready to do switch\n");

			// At this point the MM1 would know what to do with the various
			// streams. The video streams should be mapped from the MMStimPosition
			// to an actual widget, and the audio (if used) should be routed to
			// the mixer as appropriate (have to deal with the iss logic)

			// For this example, I am assuming and only caring about a single
			// video stream in the source position "1". In current case, that
			// refers to the sink pad on m_conv - in a more complex example
			// the video portion of the pipeline videoconvert->videoscale would be
			// further connected to qtgstvideosink, and there would be one of
			// these chains for left, right, etc.

			// so, get the source from the current stim at position VIDEO_POS

			const MMStim& stim = pmm1->getStim(pmm1->getPendingID());
			const MMSource& source = stim.sourcemap().at(VIDEO_POS);

#if 0
			// detach vsrcpad newstim->vsrcpad from conv sink pad
			GstPad *convsink = gst_element_get_static_pad(conv, "sink");
			gst_pad_unlink(currentstim->vsrcpad, convsink);

			// get rid of test src
			gst_element_set_state(pdata->testsrc, GST_STATE_NULL);

			// link new src and set offset
			gst_pad_link(newstim->vsrcpad, convsink);

			GstClockTime abs = gst_clock_get_time(gst_element_get_clock(conv));
			GstClockTime base = gst_element_get_base_time(conv);
			// same base time GstClockTime base2 = gst_element_get_base_time(newstim->dbin);
			// TRY SETTING ON SINK gst_pad_set_offset(newstim->vsrcpad, abs-base);
			gst_pad_set_offset(convsink, abs-base);
			gst_object_unref(convsink);

			g_print("GST_MESSAGE_APPLICATION:  abs %u base %u run time %u\n", abs, base, GST_TIME_AS_MSECONDS(abs-base));


			// unblock vsrcpad
			gst_pad_remove_probe(newstim->vsrcpad, newstim->probeidV);
			newstim->vblocked = false;
#endif
		}
		break;
	}
	default:
		break;
	}
	return TRUE;
}

void MM1::noMorePadsCallback(GstElement *, MMSource *pmmsrc)
{
	pmmsrc->dec_counter();
}


void MM1::padAddedCallback(GstElement *, GstPad * pad, MMSource *pmmsrc)
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
		if (pmmsrc->sourceType == MMSourceType::VIDEO_ONLY || pmmsrc->sourceType == MMSourceType::AUDIO_VIDEO)
		{
			pmmsrc->inc_counter();

			// save this stream
			MMStream *pstream = new MMStream();
			pstream->isVideo = true;
			pstream->probeid = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &MM1::padProbeBlockCallback, pmmsrc, NULL);
			pstream->srcpad = pad;
			gst_object_ref(pad);
			pmmsrc->addStream(pstream);
		}
	}

	gst_caps_unref (caps);
}

GstPadProbeReturn MM1::padProbeBlockCallback(GstPad *, GstPadProbeInfo *, gpointer user_data)
{
	MMSource *pmmsrc = (MMSource *)user_data;
	pmmsrc->dec_counter();
	return GST_PAD_PROBE_OK;
}
