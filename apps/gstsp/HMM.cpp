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


const HMMStimPosition HMM::STIMPOS_CONTROL = "control";
const HMMStimPosition HMM::STIMPOS_LEFT = "left";
const HMMStimPosition HMM::STIMPOS_CENTER = "center";
const HMMStimPosition HMM::STIMPOS_RIGHT = "right";
const HMMStimPosition HMM::STIMPOS_AUDIO = "speaker";


// the specific audio sink element used depends on platform
#if defined(Q_OS_MAC)
#define AUDIOSINK_ELEMENT "osxaudiosink"
#elif defined(Q_OS_LINUX)
#define AUDIOSINK_ELEMENT "alsaudiosink"
#elif defined(Q_OS_WIN)
#define AUDIOSINK_ELEMENT "directsoundsink"
#else
#define AUDIOSINK_ELEMENT "UNSUPPORTED_PLATFORM"
#endif



void HMMConfiguration::addVideoSink(HMMStimPosition pos, const std::string& sink)
{
	video[pos] = sink;
}

void HMMConfiguration::addAudioSink(HMMStimPosition pos, const std::string& sink)
{
	audio[pos] = sink;
}

HMM::HMM(const HMMConfiguration& config, StimFactory& factory)
: m_factory(factory)
, m_instanceCounter(0)
{
	g_print("HMM::HMM()\n");

	// launch main loop thread
	m_pgml = g_main_loop_new(NULL, FALSE);
	m_gthread = g_thread_new("HMM-main-loop", &HMM::threadFunc, m_pgml);

	// create pipeline
	m_pipeline = gst_pipeline_new ("gstsp-pipeline");
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	gst_bus_add_watch(bus, &HMM::busCallback, this);
	gst_object_unref(bus);

	// Configure port object. Video first.
	g_print("HMM::HMM(): configure video ports\n");

	for (StimPosTailMap::const_iterator it = config.video.begin(); it!= config.video.end(); it++)
	{

		// Configure the pipeline first.
		// TODO: must specify alternate sinks here e.g. gstqtvideosink, now its hard coded to do autovideosink
		// By adding a video element to the port - it will always search for a video stream from the source
		// with the same stim position (StimPosition is like "left", "right", "center", etc.

		g_print("HMM: Configuring pipeline, add video position %s using sink \"%s\"\n", it->first.c_str(), it->second.c_str());
		GstElement *conv, *scale, *vsink;
		conv = gst_element_factory_make ("videoconvert", NULL);
		scale = gst_element_factory_make ("videoscale", NULL);

		// create video sink. The sink factory name was passed in the config.
		vsink = gst_element_factory_make(it->second.c_str(), NULL);
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
	}

	// Now configure audio - note we may also configure background for audio

	g_print("HMM::HMM(): configure audio ports\n");
	for (StimPosTailMap::const_iterator it = config.audio.begin(); it!= config.audio.end(); it++)
	{
		g_print("HMM: Configuring pipeline, add audio position \"%s\" using sink element %s\n", it->first.c_str(), AUDIOSINK_ELEMENT);
		GstElement *audioSink = NULL, *audioMixer=NULL, *audioTestSrc=NULL;
		if (it->second.size() > 0)
			audioSink = gst_element_factory_make(it->second.c_str(), NULL);
		else
			audioSink = gst_element_factory_make(AUDIOSINK_ELEMENT, NULL);

		if (!audioSink)
		{
			std::ostringstream oss;
			oss << "Cannot create sink \"" << it->second << "\" at audio pos " << it->first;
			throw std::runtime_error(oss.str());
		}
		audioMixer = gst_element_factory_make("audiomixer", NULL);
		if (!audioMixer)
			throw std::runtime_error("Cannot create audio mixer");

		gst_object_ref(audioMixer);
		gst_object_ref(audioSink);
		gst_bin_add_many(GST_BIN(m_pipeline), audioMixer, audioSink, NULL);
		gst_element_link(audioMixer, audioSink);
		m_port.addAudioEle(it->first, audioMixer);
		m_port.addAudioSink(it->first, audioSink);

		// create a single source that will always live with the mixer
		audioTestSrc = gst_element_factory_make("audiotestsrc", NULL);
		if (!audioTestSrc)
			throw std::runtime_error("Cannot create audio test src");
		//g_object_set (audioTestSrc, "volume", (gdouble)0.1, NULL);
		g_object_set(audioTestSrc, "wave", 4);

		gst_bin_add(GST_BIN(m_pipeline), audioTestSrc);
		gst_element_link(audioTestSrc, audioMixer);
//		GstPad *aPad = gst_element_get_request_pad(audioMixer, "sink_%us");
//		if (!aPad)
//			throw std::runtime_error("Cannot get request pad from audio mixer");
//		GstPad *aSrcPad = gst_element_get_static_pad(audioTestSrc, "src");
//		if (!aSrcPad)
//			throw std::runtime_error("Cannot get src pad from audiotestsrc");
//		gst_pad_link(aSrcPad, aPad);
//		gst_object_unref(aSrcPad);
//		gst_object_unref(aPad);

	}

	g_print("HMM::HMM(): configure background default screen\n");

	m_iidBkgd = 99;
	m_instanceMap[m_iidBkgd] = m_factory.background(*this);
	m_instanceMap.insert(std::make_pair(m_iidBkgd, m_instanceMap[m_iidBkgd]));
	m_iidCurrent = m_iidBkgd;
	g_print("HMM(): m_port.connect( bkgd = %lu)\n", m_iidBkgd);
	m_port.connect(*m_instanceMap[m_iidBkgd]);
	g_print("set playing...\n");
	gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

	g_print("HMM::HMM() - done.\n");
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
	// destroy all Stim in instance map
	for (std::pair<HMMInstanceID, StimP> p : m_instanceMap)
	{
		delete p.second;
	}
	m_instanceMap.clear();
	gst_element_set_state(m_pipeline, GST_STATE_NULL);

	// exit main loop
	qDebug() << "HMM::~HMM: quit main loop";
	g_main_loop_quit(m_pgml);
	qDebug() << "HMM::~HMM: g_thread_join...";
	g_thread_join(m_gthread);
	qDebug() << "HMM::~HMM: g_thread_join done\n";

	g_main_loop_unref(m_pgml);
	g_thread_unref(m_gthread);

}

void HMM::removeStimInstance(HMMInstanceID id)
{
	InstanceMap::iterator it = m_instanceMap.find(id);
	if (it != m_instanceMap.end())
		m_instanceMap.erase(it);
	else
		throw std::runtime_error("Cannot remove stim instance.");
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
	g_print("dump to %s\n", c);
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
	//
	// When the sync_state call below happens, what happens depends on how the Sources which make up
	// the Stim are set up. If a Source is a multimedia file, then the file is opened and decoding
	// begins. As decoding progresses, new Src pads are created. Each such pad represents a stream that
	// is either VIDEO or AUDIO (other stream types are ignored). The new pads are blocked, and once blocked,
	//

	HMMInstanceID instanceID = (HMMInstanceID)(1000 + m_instanceCounter);
	m_instanceCounter++;
	std::ostringstream oss;
	oss << "iid-" << instanceID;
	m_instanceMap[instanceID] = m_factory(id, *this, oss.str().c_str());
	//dump("preroll");
	m_instanceMap[instanceID]->preroll(pipeline());

//	// Now sync all elements in the newly-created Stim with parent.
//	for (std::map<HMMStimPosition, Stim::SourceP>::iterator it = m_instanceMap[instanceID]->sourceMap().begin(); it != m_instanceMap[instanceID]->sourceMap().end(); it++)
//	{
//		gst_element_sync_state_with_parent(it->second->bin());
//	}
	return instanceID;
}


void HMM::play(const HMMInstanceID& iid)
{
	if (m_instanceMap.count(iid) != 1)
		throw std::runtime_error("instance id not found, cannot play.");

	Stim *pstimCurrent = m_instanceMap[m_iidCurrent];
	Stim *pstimPending = m_instanceMap[iid];

	if (pstimPending->getStimState() != HMMStimState::PREROLLED)
	{
		g_print("HMM::play(%lu) - not prerolled\n", iid);
		return;
	}

	g_print("HMM::play(%lu) - set IDLE probes on %d sources\n", iid, (int)pstimCurrent->sourceMap().size());
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

	PlayStimCounter *pcounterPlay = new PlayStimCounter(iid, this, (int)pstimCurrent->sourceMap().size());
	for (std::pair<HMMStimPosition, Stim::SourceP> p: pstimCurrent->sourceMap())
	{
		Stim::SourceP psrc = p.second;
		Stim::SourceP psrcPending = pstimPending->getSource(p.first);
		if (!psrcPending)
			throw std::runtime_error("Pending source not found corresponding to current src");

		g_print("create noop counter with count = %d\n", psrc->streamMap().size());
		NoopCounter *pcounter = new NoopCounter((int)(psrc->streamMap().size()), pcounterPlay);
		for (std::pair<HMMStreamType, Source::StreamP> pp: psrc->streamMap())
		{
			// now put blocking probe on each stream, with a Noop counter holding a swap counter
//
//			if (pp.first == HMMStreamType::VIDEO)
//			{
				g_print("HMM::play(%lu) - adding probe for stream type %d\n", iid, pp.first);
				pp.second->setProbeID(
						gst_pad_add_probe(pp.second->srcpad(), (GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM|GST_PAD_PROBE_TYPE_IDLE), &HMM::padProbeIdleCallback, pcounter, NULL)
						);
				g_print("HMM::play(%lu) - added probe for stream type %d: block %lu\n", iid, pp.first, pp.second->getProbeID());
//			}
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

gboolean HMM::busCallback(GstBus *, GstMessage *msg, gpointer user_data)
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
		g_print("MM1::busCallback(message type %s from ele %s)\n", GST_MESSAGE_TYPE_NAME(msg), GST_MESSAGE_SRC_NAME(msg));
		if (gst_message_has_name (msg, "seek"))
		{
			Source *psrc;
			if (FALSE == gst_structure_get(gst_message_get_structure(msg), "psrc", G_TYPE_POINTER, &psrc, NULL))
				throw std::runtime_error("Cannot get source ptr from msg");

			// flushing seek
			g_print("MM1::busCallback(seek message) - flushing seek on element %s\n", GST_ELEMENT_NAME(psrc->bin()));
			if (!gst_element_seek(psrc->bin(), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				throw std::runtime_error("seek failed");
			}
		}
		else if (gst_message_has_name (msg, "loop"))
		{
			Source *psrc;
			if (FALSE == gst_structure_get(gst_message_get_structure(msg), "psrc", G_TYPE_POINTER, &psrc, NULL))
				throw std::runtime_error("Cannot get source from msg");

			// NON-flushing seek
			g_print("MM1::busCallback(loop message) - non-flushing seek on element %s\n", GST_ELEMENT_NAME(psrc->bin()));
			if (!gst_element_seek(psrc->bin(), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				throw std::runtime_error("LOOPING seek failed");
			}
		}
		else if (gst_message_has_name (msg, "play"))
		{
			Stim *pCurrent;
			Stim *pPending;
			HMMInstanceID iid = 0;
			g_print("MM1::busCallback(play message)\n");
			if (FALSE == gst_structure_get(gst_message_get_structure(msg), "iid", G_TYPE_ULONG, &iid, NULL))
				throw std::runtime_error("Cannot get stuff from play msg");
			phmm->swap(iid);
//			phmm->port().disconnect();
//			phmm->port().connect(*pPending);
			g_print("MM1::busCallback(play message) - done\n");
		}
		break;
	}
	default:
		break;
	}
	return TRUE;
}


void HMM::swap(HMMInstanceID iid)
{
	port().disconnect();

	// now m_iidCurrent is done and dangling. Dispose.
	dispose(m_iidCurrent);

	// Get the instance 'iid', connect it (this also releases the blocking probe)
	Stim *pstim = getStimInstance(iid);
	if (!pstim)
		throw std::runtime_error("HMM::swap() cannot find instance.");
	port().connect(*pstim);
	m_iidCurrent = iid;
}

/*
 *
 * 12344567888890qwertyuiopasqersnfnu90wsa1
 *
 * bnnnnnnnnnnnnnnngfuo80xbgm, mmmkhg34
 *
 * bronte - 4/13/20
 *
 */

void HMM::dispose(HMMInstanceID iid)
{
	// Get the instance 'iid', connect it (this also releases the blocking probe)
	g_print("HMM::dispose(%lu)\n", iid);

	Stim *pstim = getStimInstance(iid);
	if (!pstim)
		throw std::runtime_error("HMM::dispose() cannot find instance.");
	removeStimInstance(iid);

	// Just delete the stim?
	g_print("delete pstim\n");
	delete pstim;
	g_print("HMM::dispose(%lu) - done\n", iid);

}

#if 0
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
	g_print("HMM::makeSourceFromFile(%s)\n", uri.c_str());
	GstElement *ele = gst_element_factory_make("uridecodebin", NULL);
	if (!ele)
		g_print("NULL ele!\n");
	g_object_set (ele, "uri", uri.c_str(), NULL);

	// add ele to pipeline. pipeline takes ownership, will unref when ele is removed.
	if (!gst_bin_add(GST_BIN(m_pipeline), ele))
		g_print("ERROR- cannot add uridecodebin\n");

	// Source does not ref the ele, assumes that its in a pipeline and ref'd there.
	// TODO: not sure I like how ownership seems ill-defined.
	Source *psrc = new Source(stype, ele, loop, volume);

	g_signal_connect (psrc->bin(), "pad-added", G_CALLBACK(HMM::padAddedCallback), userdata);
	g_signal_connect (psrc->bin(), "no-more-pads", G_CALLBACK(HMM::noMorePadsCallback), userdata);

	return psrc;
}
#endif

void HMM::noMorePadsCallback(GstElement *, FileSourcePrerollCounter *pcounter)
{
	pcounter->decrement();
}

//void HMM::padAddedCallback(GstElement *, GstPad * pad, SourcePrerollCounter *pcounter)
//{
//	GstCaps *caps;
//	GstStructure *s;
//	const gchar *name;
//	Stream *pstream = NULL;
//
//	caps = gst_pad_get_current_caps (pad);
//	s = gst_caps_get_structure (caps, 0);
//	name = gst_structure_get_name (s);
//
//	g_print("padAddedCallback: %s\n", name);
//	if (strcmp (name, "video/x-raw") == 0)
//	{
//		if (pcounter->source()->sourceType() == HMMSourceType::VIDEO_ONLY ||
//			pcounter->source()->sourceType() == HMMSourceType::AUDIO_VIDEO)
//		{
//			pcounter->increment();
//
//			// check if stream is a still image. If so, add imagefreeze and use its src pad
//			// in the Stream object. If its a video, use that pad. Have to ref it in the latter case.
//			const GValue *v = gst_structure_get_value(s, "framerate");
//			if (v && GST_VALUE_HOLDS_FRACTION(v))
//			{
//				gint num = gst_value_get_fraction_numerator(v);
//				if (num == 0)
//				{
//					GstElement *freeze = gst_element_factory_make("imagefreeze", NULL);
//					if (!freeze)
//						throw std::runtime_error("Cannot create freeze element");
//					GstPad *sinkpad = gst_element_get_static_pad(freeze, "sink");
//					if (!sinkpad)
//						throw std::runtime_error("Cannot get sinkpad from imagefreeze");
//					gst_bin_add(GST_BIN(pcounter->pipeline()), freeze);
//					gst_pad_link(pad, sinkpad);
//					gst_object_unref(sinkpad);
//					GstPad *srcpad = gst_element_get_static_pad(freeze, "src");
//					gst_object_ref(srcpad);
//					pstream = new Stream(srcpad);
//				}
//				else
//				{
//					gst_object_ref(pad);
//					pstream = new Stream(pad);
//				}
//			}
//			else
//				throw std::runtime_error("NO FRAMERATE IN CAPS");
//
//			// save this stream
//			pcounter->source()->addStream(HMMStreamType::VIDEO, pstream);
//
//			// set blocking probe on the src pad
//			pstream->setProbeID(gst_pad_add_probe(pstream->srcpad(), GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HMM::padProbeBlockCallback, pcounter, NULL));
//
//			// set event probe on src pad.
//			pstream->setEventProbeID(gst_pad_add_probe(pstream->srcpad(), GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HMM::eventProbeCallback, pcounter, NULL));
//		}
//	}
//
//	gst_caps_unref (caps);
//}

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
	g_print("HMM::padProbeIdleCallback, from element %s - start.\n", GST_ELEMENT_NAME(GST_PAD_PARENT(pad)));
	//g_print("HMM::padProbeIdleCallback, from element %s pad %s ghost? %s\n", GST_ELEMENT_NAME(GST_PAD_PARENT(pad)), GST_PAD_NAME(pad), (GST_IS_GHOST_PAD(pad) ? "YES" : "NO"));
	pcounter->decrement();
	g_print("HMM::padProbeIdleCallback, from element %s - done.\n", GST_ELEMENT_NAME(GST_PAD_PARENT(pad)));
	return GST_PAD_PROBE_OK;
}


GstPadProbeReturn HMM::eventProbeCallback(GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	//HMMSource* psrcPending = (HMMSource *)p;
	FileSourcePrerollCounter *pcounter = (FileSourcePrerollCounter *)p;

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
			GstStructure *structure = gst_structure_new("loop", "psrc", G_TYPE_POINTER, pcounter->fileSource(), NULL);
			gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(pcounter->fileSource()->bin()), structure));
			gst_object_unref(bus);

		}
	}
	return GST_PAD_PROBE_OK;
}
