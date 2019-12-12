/*
 * HMM.cpp
 *
 *  Created on: Sep 17, 2019
 *      Author: dan
 */

#include "HMM.h"
#include <algorithm>
#include <vector>
#include <sstream>
using namespace hmm;

bool f_looping = false;


const HMMStimPosition HMM::STIMPOS_CONTROL = 0;
const HMMStimPosition HMM::STIMPOS_LEFT = 1;
const HMMStimPosition HMM::STIMPOS_CENTER = 2;
const HMMStimPosition HMM::STIMPOS_RIGHT = 3;
const HMMStimPosition HMM::STIMPOS_AUDIO = 4;


void HMMConfiguration::addVideoSink(HMMStimPosition pos, const std::string& name, const std::string& sink)
{
	video[pos] = std::make_pair(name, sink);
}

void HMMConfiguration::addAudioSink(HMMStimPosition pos, const std::string& name, const std::string& sink)
{
	audio[pos] = std::make_pair(name, sink);
}

HMM::HMM(const HMMConfiguration& config, StimFactory& factory)
: m_factory(factory)
{
//	GstElement *conv, *scale, *vsink;
	m_iidBkgd = 0;		// TODO: should do this somehow with addStimInfo? There id starts at 1, so OK with 0 here.

	// launch main loop thread
	m_pgml = g_main_loop_new(NULL, FALSE);
	m_gthread = g_thread_new("HMM-main-loop", &HMM::threadFunc, m_pgml);

	// create pipeline
	m_pipeline = gst_pipeline_new (NULL);
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	gst_bus_add_watch(bus, &HMM::busCallback, this);
	gst_object_unref(bus);

	// Configure port object. Video first.
	for (StimPosTailMap::const_iterator it = config.video.begin(); it!= config.video.end(); it++)
	{

		// Configure the pipeline first.
		// TODO: must specify alternate sinks here e.g. gstqtvideosink, now its hard coded to do autovideosink
		// By adding a video element to the port - it will always search for a video stream from the source
		// with the same stim position (StimPosition is like "left", "right", "center", etc.

		g_print("HMM: Configuring pipeline, add video path \"%s\" at position %d using sink \"%s\"\n", it->second.first.c_str(), (int)it->first, it->second.second.c_str());
		GstElement *conv, *scale, *vsink;
		conv = gst_element_factory_make ("videoconvert", NULL);
		scale = gst_element_factory_make ("videoscale", NULL);

		// create video sink. The sink factory name was passed in the config.
		vsink = gst_element_factory_make(it->second.second.c_str(), NULL);
		if (!vsink)
		{
			throw std::runtime_error("Cannot create sink element. Check GST_PLUGIN_PATH.");
		}
		gst_object_ref(conv);
		gst_object_ref(vsink);
		gst_bin_add_many(GST_BIN(m_pipeline), conv, scale, vsink, NULL);
		if (!gst_element_link_many (conv, scale, vsink, NULL))
		{
			throw std::runtime_error("Cannot link conv-scale-sink");
		}
		m_port.addVideoEle(it->first, conv);	// port takes ownership of the conv element
		m_port.addVideoSink(it->first, vsink);
		//=====end pipeline config. The port keeps the conv ele with the "StimPosition" value.

//		// Now configure background source for this particular video path. A new stim was created above,
//		// here we create  a test element and create the source/stream manually.
//
//		GstElement *ptest = gst_element_factory_make ("videotestsrc", NULL);
//		gst_bin_add(GST_BIN(m_pipeline), ptest);
//		GstPad *pad = gst_element_get_static_pad(ptest, "src");
//		Stream *pstream = new Stream(pad);
//		Source *psource = new Source(hmm::HMMSourceType::VIDEO_ONLY, ptest, false);
//		psource->addStream(hmm::HMMStreamType::VIDEO, pstream);
//		pbkgd->addSource(it->first, psource);
	}

	// Now configure audio - note we may also configure background for audio

	for (StimPosTailMap::const_iterator it = config.audio.begin(); it!= config.audio.end(); it++)
	{
		g_print("HMM: Configuring pipeline, add audio path \"%s\"\n", it->second.first.c_str());
		GstElement *audioSink = NULL, *audioMixer=NULL;
		audioSink = gst_element_factory_make(it->second.second.c_str(), NULL);
		if (!audioSink)
		{
			std::ostringstream oss;
			oss << "Cannot create sink \"" << it->second.second.c_str() << "\" at audio pos " << it->first;
			throw std::runtime_error(oss.str());
		}
		audioMixer = gst_element_factory_make("audiomixer", NULL);
		if (!audioMixer)
			throw std::runtime_error("Cannot create audio mixer");

		gst_bin_add_many(GST_BIN(m_pipeline), audioMixer, audioSink, NULL);
		gst_element_link(audioMixer, audioSink);
		m_port.addAudioEle(it->first, audioMixer);
		m_port.addAudioSink(it->first, audioSink);
	}

	m_iidBkgd = 0;
	m_instanceMap.insert(std::make_pair(m_iidBkgd, m_factory(0, *this)));
	m_iidCurrent = m_iidBkgd;
	g_print("HMM(): m_port.connect( bkgd = %lu)\n", m_iidBkgd);
	m_port.connect(*getStimInstance(m_iidBkgd));
	gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

}

gpointer HMM::threadFunc(gpointer user_data)
{
	GMainLoop *pgml = (GMainLoop *)user_data;
	// create and start main loop here
	qDebug() << "HMM::threadFunc: starting main loop\n";
	g_main_loop_run(pgml);
	qDebug() << "HMM::threadFunc: main loop ended\n";
	return NULL;
}


HMM::~HMM()
{
	// exit main loop
	qDebug() << "HMM::~HMM: quit main loop";
	g_main_loop_quit(m_pgml);
	qDebug() << "HMM::~HMM: g_thread_join...";
	g_thread_join(m_gthread);
	qDebug() << "HMM::~HMM: g_thread_join done\n";

	g_main_loop_unref(m_pgml);
	g_thread_unref(m_gthread);

}


Stim *HMM::getStimInstance(HMMInstanceID id)
{
	StimP pstim = NULL;
	if (m_instanceMap.count(id) > 0)
	{
		pstim = m_instanceMap[id];
	}
	return pstim;
}

void HMM::dump(const char *c)
{
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline()), GST_DEBUG_GRAPH_SHOW_ALL, c);
}

HMMInstanceID HMM::preroll(HMMStimID id)
{
	g_print("prerolling stimID %lu\n", id);

	// at this point we would set in motion the preroll for _each_of_the_sources_, e.g. left,
	// right, ISS, or whatever topology is in use.
	//
	// Prerolling the stim means prerolling each individual source. Only when all sources are prerolled
	// is the stim fully prerolled.

	HMMInstanceID instanceID = (HMMInstanceID)(1000 + m_instanceMap.size());
	m_instanceMap[instanceID] = m_factory(id, *this);

	// Now sync all elements in the newly-created Stim with parent.
	for (std::map<HMMStimPosition, Stim::SourceP>::iterator it = m_instanceMap[instanceID]->sourceMap().begin(); it != m_instanceMap[instanceID]->sourceMap().end(); it++)
	{
		gst_element_sync_state_with_parent(it->second->bin());
	}
	return instanceID;
}


void HMM::play(const HMMInstanceID& id)
{
	if (m_instanceMap.count(id) != 1)
		throw std::runtime_error("instance id not found, cannot play.");

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

	Stim *pstimCurrent = m_instanceMap[m_iidCurrent];
	Stim *pstimPending = m_instanceMap[id];

	PlayStimCounter *pcounterPlay = new PlayStimCounter(pstimCurrent, pstimPending, this, (int)pstimCurrent->sourceMap().size());
	for (std::pair<HMMStimPosition, Stim::SourceP> p: pstimCurrent->sourceMap())
	{
		Stim::SourceP psrc = p.second;
		Stim::SourceP psrcPending = pstimPending->getSource(p.first);
		g_print("will set blocking (IDLE) probe for src pos %d\n", p.first);
		if (!psrcPending)
			throw std::runtime_error("Pending source not found corresponding to current src");

		NoopCounter *pcounter = new NoopCounter((int)(psrc->streamMap().size()), pcounterPlay);
		for (std::pair<HMMStreamType, Source::StreamP> pp: psrc->streamMap())
		{
			// now put blocking probe on each stream, with a Noop counter holding a swap counter

			if (pp.first == HMMStreamType::VIDEO)
			{
				pp.second->setProbeID(
						gst_pad_add_probe(pp.second->srcpad(), (GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM|GST_PAD_PROBE_TYPE_IDLE), &HMM::padProbeBlockCallback, pcounter, NULL)
						);
				g_print("added probe for stream type %d: block %lu\n", pp.first, pp.second->getProbeID());
			}
		}
	}
}


//HMMStimID HMM::swap(HMMStimID id)
//{
//	HMMInstanceID current = m_iidCurrent;
//	g_print("HMM::swap(current %lu new %lu\n", current, id);
//	for (std::pair<const HMMStimPosition, Stim::source_ptr>& p: getStimInstance(m_iidCurrent)->sourceMap())
//	{
//		if (p.first > 0)
//		{
//			Source* psrc = p.second.get();
//			Source* psrcPending = getStim(id)->getSource(p.first);
//			if (!psrcPending)
//				throw std::runtime_error("Pending source not found corresponding to current src");
//			// does current source have a video stream?
//			Stream *pvideoCurrent = psrc->getStream(HMMStreamType::VIDEO);
//			Stream *pvideoPending = psrcPending->getStream(HMMStreamType::VIDEO);
//
//			if (pvideoCurrent)
//			{
//				if (!pvideoPending)
//					throw std::runtime_error("Pending source missing video stream");
//
//				// unlink
//				HMMVideoTail* ptail = getVideoTail(p.first);
//				if (!ptail)
//					throw std::runtime_error("cannot find video tail");
//				GstPad *psink = gst_element_get_static_pad(ptail->m_conv, "sink");
//				gst_pad_unlink(pvideoCurrent->srcpad(), psink);
//
//				// link
//				gst_pad_link(pvideoPending->srcpad(), psink);
//
//				// set offset on tail pad
//				GstClockTime abs = gst_clock_get_time(gst_element_get_clock(ptail->m_conv));
//				GstClockTime base = gst_element_get_base_time(ptail->m_conv);
//				gst_pad_set_offset(psink, abs-base);
//				gst_object_unref(psink);
//
//				g_print("Set offset on tail pad to %lums\n", GST_TIME_AS_MSECONDS(abs-base));
//				// remove probe on the pending video src pad
//				g_print("remove probe on pad, probeid %d\n", pvideoPending->getProbeID());
//				gst_pad_remove_probe(pvideoPending->srcpad(), pvideoPending->getProbeID());
//
//				// finally, do flushing seek on removed source
//				// TODO - the actual disposition of the source really controlled at the stim level
//
//				GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
//				GstStructure *structure = gst_structure_new("seek", "id", G_TYPE_ULONG, current, "pos", G_TYPE_INT, p.first,  NULL);
//				gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(pipeline()), structure));
//				gst_object_unref(bus);
//
//
//			}
//			else
//				throw std::runtime_error("Current source has video pos but no video stream");
//		}
//	}
//	m_stimidCurrent = id;
//	g_print("HMM::swap - done\n");
//	return current;
//}

gboolean HMM::busCallback(GstBus *bus, GstMessage *msg, gpointer user_data)
{
	HMM* phmm = (HMM *)user_data;
	//g_print("MM1::busCallback(%s - %s)\n", GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME(msg));

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
	case GST_MESSAGE_SEGMENT_DONE:
	{
		g_print("MM1::busCallback(%s - %s) SEGMENT_DONE\n", GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME(msg));
		break;
	}
	case GST_MESSAGE_ASYNC_DONE:
	{
		g_print("MM1::busCallback(%s - %s) ASYNC_DONE\n", GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME(msg));
		break;
	}
	case GST_MESSAGE_APPLICATION:
	{
		g_print("MM1::busCallback(%s - %s)\n", GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME(msg));
		if (gst_message_has_name (msg, "seek"))
		{
			g_print("Got seek msg\n");
			HMMStimID id;
			HMMStimPosition pos;
			Source *psrc;
			if (FALSE == gst_structure_get(gst_message_get_structure(msg), "psrc", G_TYPE_POINTER, &psrc, NULL))
				throw std::runtime_error("Cannot get source ptr from msg");

			if (!phmm)
				throw std::runtime_error("Bus message (seek) does not carry hmm pointer!");

			// flushing seek
			if (!gst_element_seek(psrc->bin(), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				throw std::runtime_error("seek failed");
			}
		}
		else if (gst_message_has_name (msg, "loop"))
		{
			g_print("Got loop msg\n");
			HMMStimID id;
			HMMStimPosition pos;
			Source *psrc;
			if (FALSE == gst_structure_get(gst_message_get_structure(msg), "psrc", G_TYPE_POINTER, &psrc, NULL))
				throw std::runtime_error("Cannot get source from msg");

			if (!phmm)
				throw std::runtime_error("Bus message (seek) does not carry hmm pointer!");

			// NON-flushing seek
			if (!gst_element_seek(psrc->bin(), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				throw std::runtime_error("LOOPING seek failed");
			}
		}
		else if (gst_message_has_name (msg, "play"))
		{
			Stim *pCurrent;
			Stim *pPending;
			g_print("Got play msg\n");
			if (FALSE == gst_structure_get(gst_message_get_structure(msg), "current", G_TYPE_POINTER, &pCurrent, "pending", G_TYPE_POINTER, &pPending, NULL))
				throw std::runtime_error("Cannot get stuff from play msg");
			g_print("disconnect\n");
			phmm->port().disconnect();
			g_print("connect\n");
			phmm->port().connect(*pPending);
			g_print("done.\n");
		}
		break;
	}
	default:
		break;
	}
	return TRUE;
}

Source *HMM::makeSourceFromColor(unsigned long aarrggbb)
{
	std::ostringstream oss;
	GError *gerror = NULL;
	GstElement *src;
	oss << "videotestsrc pattern=solid-color foreground-color=" << aarrggbb;
	src = gst_parse_bin_from_description(oss.str().c_str(), true, &gerror);
	if (src == NULL || gerror != NULL)
		throw std::runtime_error("Cannot create color source");
	gst_object_ref(src);
	return new Source(hmm::HMMSourceType::VIDEO_ONLY, src, false, 0);
}


Source *HMM::makeSourceFromFile(const std::string& filename, HMMSourceType stype, void *userdata, bool loop, unsigned int volume)
{
	std::string uri("file://");
	uri.append(filename);
	GstElement *ele = gst_element_factory_make("uridecodebin", NULL);
	g_object_set (ele, "uri", uri.c_str(), NULL);
	gst_object_ref(ele);
	gst_bin_add(GST_BIN(m_pipeline), ele);
	Source *psrc = new Source(stype, ele, loop, volume);

	g_signal_connect (psrc->bin(), "pad-added", G_CALLBACK(HMM::padAddedCallback), userdata);
	g_signal_connect (psrc->bin(), "no-more-pads", G_CALLBACK(HMM::noMorePadsCallback), userdata);

	return psrc;
}

void HMM::noMorePadsCallback(GstElement *, SourcePrerollCounter *pcounter)
{
	pcounter->decrement();
}

void HMM::padAddedCallback(GstElement *, GstPad * pad, SourcePrerollCounter *pcounter)
{
	GstCaps *caps;
	GstStructure *s;
	const gchar *name;
	Stream *pstream = NULL;

	caps = gst_pad_get_current_caps (pad);
	s = gst_caps_get_structure (caps, 0);
	name = gst_structure_get_name (s);

	g_print("padAddedCallback: %s\n", name);
	if (strcmp (name, "video/x-raw") == 0)
	{
		if (pcounter->source()->sourceType() == HMMSourceType::VIDEO_ONLY ||
			pcounter->source()->sourceType() == HMMSourceType::AUDIO_VIDEO)
		{
			pcounter->increment();

			// check if stream is a still image. If so, add imagefreeze
			const GValue *v = gst_structure_get_value(s, "framerate");
			if (v && GST_VALUE_HOLDS_FRACTION(v))
			{
				gint num = gst_value_get_fraction_numerator(v);
				if (num == 0)
				{
					GstElement *freeze = gst_element_factory_make("imagefreeze", NULL);
					if (!freeze)
						throw std::runtime_error("Cannot create freeze element");
					GstPad *sinkpad = gst_element_get_static_pad(freeze, "sink");
					if (!sinkpad)
						throw std::runtime_error("Cannot get sinkpad from imagefreeze");
					gst_bin_add(GST_BIN(pcounter->pipeline()), freeze);
					gst_pad_link(pad, sinkpad);
					gst_object_unref(sinkpad);
					GstPad *srcpad = gst_element_get_static_pad(freeze, "src");
					gst_object_ref(srcpad);
					pstream = new Stream(srcpad);
				}
				else
				{
					gst_object_ref(pad);
					pstream = new Stream(pad);
				}
			}
			else
				throw std::runtime_error("NO FRAMERATE IN CAPS");

			// save this stream
			pcounter->source()->addStream(HMMStreamType::VIDEO, pstream);

			// set blocking probe on the src pad
			pstream->setProbeID(gst_pad_add_probe(pstream->srcpad(), GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HMM::padProbeBlockCallback, pcounter, NULL));

			// set event probe on src pad.
			pstream->setEventProbeID(gst_pad_add_probe(pstream->srcpad(), GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HMM::eventProbeCallback, pcounter, NULL));
		}
	}

	gst_caps_unref (caps);
}

GstPadProbeReturn HMM::padProbeBlockCallback(GstPad *, GstPadProbeInfo *, gpointer user_data)
{
	Counter *pcounter = (Counter *)user_data;
	if (pcounter->decrement())
	{
		g_print("HMM::padProbeBlockCallback: decrement() returned TRUE, DO NOT delete counter\n");
		//delete pcounter;
	}
	return GST_PAD_PROBE_OK;
}

GstPadProbeReturn HMM::padProbeIdleCallback(GstPad *pad, GstPadProbeInfo *, gpointer user_data)
{
	Counter *pcounter = (Counter *)user_data;
	g_print("HMM::padProbeIdleCallback, from element %s pad %s ghost? %s\n", GST_ELEMENT_NAME(GST_PAD_PARENT(pad)), GST_PAD_NAME(pad), (GST_IS_GHOST_PAD(pad) ? "YES" : "NO"));
	if (GST_IS_GHOST_PAD(pad))
	{
		GstPad *targetpad = gst_ghost_pad_get_target(GST_GHOST_PAD(pad));
		g_print("target pad %s/%s\n", GST_ELEMENT_NAME(GST_PAD_PARENT(targetpad)), GST_PAD_NAME(targetpad));
	}
	if (pcounter->decrement())
	{
		g_print("HMM::padIdleBlockCallback: decrement() returned TRUE, DO NOT delete counter\n");

		//delete pcounter;
	}
	g_print("HMM::padProbeIdleCallback, from element %s - done.\n", GST_ELEMENT_NAME(GST_PAD_PARENT(pad)));
	return GST_PAD_PROBE_OK;
}


GstPadProbeReturn HMM::eventProbeCallback(GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	//HMMSource* psrcPending = (HMMSource *)p;
	SourcePrerollCounter *pcounter = (SourcePrerollCounter *)p;

	// Note: GST_PAD_PROBE_INFO_EVENT(d) returns a GstEvent* or NULL if info->data does not have an event.
	// In this routine, because its an event probe, it should always contain an event?

	GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
	if (event)
	{
		//GstElement* parent = GST_PAD_PARENT(pad);
		//qDebug() << sDebugPrefix << "Event type " << GST_EVENT_TYPE_NAME(event) << " from " << GST_ELEMENT_NAME(GST_PAD_PARENT(pad));
		g_print("HMM::eventProbeCallback - Got event type %s from %s\n", GST_EVENT_TYPE_NAME(event), GST_ELEMENT_NAME(GST_PAD_PARENT(pad)));
		if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT_DONE)
		{
			g_print("Got segment_done event from %s\n", GST_ELEMENT_NAME(GST_PAD_PARENT(pad)));

			// post bus message to flush
			GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pcounter->pipeline()));
			GstStructure *structure = gst_structure_new("loop", "psrc", G_TYPE_POINTER, pcounter->source(), NULL);
			gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(pcounter->source()->bin()), structure));
			gst_object_unref(bus);

		}
	}
	return GST_PAD_PROBE_OK;
}
