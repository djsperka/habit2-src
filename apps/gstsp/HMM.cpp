/*
 * HMM.cpp
 *
 *  Created on: Sep 17, 2019
 *      Author: dan
 */

#include "HMM.h"

const HMMStimPosition VIDEO_POS(1);
bool f_looping = false;

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

void HMMCounter::increment()
{
	g_atomic_int_inc(&m_counter);
//	g_print("inc to %d\n", m_counter);
}

bool HMMCounter::decrement()
{
	if (g_atomic_int_dec_and_test(&m_counter))
	{
//		g_print("dec to %d\n", m_counter);
		this->operator()();

		// if a sub counter exists, decrement that one now
		if (m_psubCounter)
			m_psubCounter->decrement();
		return true;
	}
//	g_print("dec to %d\n", m_counter);
	return false;
}

void HMMStim::addSource(HMMStimPosition pos, HMMSource *psrc)
{
	m_sourceMap.insert(make_pair(pos, source_ptr(psrc)));
}

HMMSource *HMMStim::getSource(HMMStimPosition pos)
{
	HMMSource *psrc = NULL;
	if (m_sourceMap.count(pos) == 1)
	{
		psrc = m_sourceMap[pos].get();
	}
	return psrc;
}


HMM::HMM(const std::string& bkgd)
{
	GstElement *conv, *scale, *vsink;
	m_stimidBkgd = addStimInfo(bkgd, false);

	// create pipeline

	m_pipeline = gst_pipeline_new (NULL);
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	gst_bus_add_watch(bus, &HMM::busCallback, this);
	gst_object_unref(bus);

	// hardcode type of pipeline - just video for now
	conv = gst_element_factory_make ("videoconvert", NULL);
	scale = gst_element_factory_make ("videoscale", NULL);
	vsink = gst_element_factory_make("autovideosink", NULL);
	gst_object_ref(conv);
	gst_object_ref(scale);
	gst_object_ref(vsink);
	gst_bin_add_many(GST_BIN(m_pipeline), conv, scale, vsink, NULL);
	if (!gst_element_link_many (conv, scale, vsink, NULL))
	{
		throw std::runtime_error("Cannot link conv-scale-sink");
	}

	HMMVideoTail tail = {conv, scale, vsink};
	addVideoTail(VIDEO_POS, tail);

	// tail end of pipeline is ready.

	// set up background. This bypasses normal procedure for stim
	GError *err=NULL;
	GstElement *ele = gst_parse_bin_from_description(bkgd.c_str(), TRUE, &err);
	if (!ele)
	{
		throw std::runtime_error("bad bkgd description");
	}
	gst_element_set_name(ele, "background");
	gst_bin_add(GST_BIN(m_pipeline), ele);
	if (!gst_element_link(ele, conv))
	{
		throw std::runtime_error("Cannot link bkgd into pipeline");
	}

	GstPad *srcpad = gst_element_get_static_pad(ele, "src");
	if (!srcpad)
		throw std::runtime_error("Cannot get src pad from background bin");

	// create a src and add a stream
	HMMSource *psource = new HMMSource(HMMSourceType::VIDEO_ONLY, ele);
	psource->addStream(HMMStreamType::VIDEO, new HMMStream(srcpad));

	HMMStim *pstim = new HMMStim();
	pstim->addSource(VIDEO_POS, psource);
	m_stimMap.insert(std::make_pair(m_stimidBkgd, pstim));
	m_stimidCurrent = m_stimidBkgd;
	gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

}


HMM::~HMM()
{
}

HMMStimID HMM::addStimInfo(const std::string& filename_or_description, bool bIsFile)
{
	HMMStimID id = (HMMStimID)((unsigned long)m_stimInfoMap.size()+1);
	m_stimInfoMap.insert(make_pair(id, make_pair(bIsFile, filename_or_description)));
	return id;
}

HMMStim *HMM::getStim(HMMStimID id)
{
	HMMStim *pstim = NULL;
	if (m_stimMap.count(id) > 0)
	{
		pstim = m_stimMap[id].get();
	}
	return pstim;
}

HMMVideoTail* HMM::getVideoTail(HMMStimPosition pos)
{
	HMMVideoTail *ptail=NULL;
	if (m_stimTailMap.count(pos) == 1)
		ptail = &m_stimTailMap[pos];
	return ptail;
}


void HMM::dump(const char *c)
{
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline()), GST_DEBUG_GRAPH_SHOW_ALL, c);
}

void HMM::preroll(HMMStimID id)
{
	if (m_stimMap.count(id) > 0)
		throw std::runtime_error("id already prerolled, don't do it again.");

	if (m_stimInfoMap.count(id) != 1)
		throw std::runtime_error("id not found, cannot preroll");

	g_print("prerolling %lu\n", id);

	// at this point we would set in motion the preroll for _each_of_the_sources_, e.g. left,
	// right, ISS, or whatever topology is in use.
	//
	// Prerolling the stim means prerolling each individual source. Only when all sources are prerolled
	// is the stim fully prerolled.
	//
	// instead, assume a single source, video only

	HMMStim *pstim = NULL;
	const stim_info& info = m_stimInfoMap.at(id);

	// The calls to makeXXXforPreroll() will sync elements in the newly created bin(s) with the
	// pipeline state, which will initiatethe preroll. The preroll will NOT necessarily be complete
	// on return here, but will be started.
	if (info.first)
		pstim = makeStimFromFile(id, info.second);
	else
		pstim = makeStimFromDesc(id, info.second);

	m_stimMap.insert(std::make_pair(id, pstim));

	return;

}


void HMM::play(HMMStimID id)
{
	if (m_stimMap.count(id) != 1)
		throw std::runtime_error("id not in stimMap, cannot play.");

	if (m_stimMap[id]->getStimState() != HMMStimState::PREROLLED)
		throw std::runtime_error("id is not prerolled, cannot play");
	g_print("play %lu current is %lu\n", id, m_stimidCurrent);


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

	HMMStim *pstimCurrent = m_stimMap[m_stimidCurrent].get();
	HMMStim *pstimPending = m_stimMap[id].get();

	//HMMStimSwapCounter *pcounterSwap = new HMMStimSwapCounter(m_stimidCurrent, id, this, (int)pstimCurrent->sourceMap().size());
	HMMPlayStimCounter *pcounterPlay = new HMMPlayStimCounter(m_stimidCurrent, id, this, (int)pstimCurrent->sourceMap().size());
	for (std::pair<const HMMStimPosition, HMMStim::source_ptr>& p: pstimCurrent->sourceMap())
	{
		HMMSource* psrc = p.second.get();
		HMMSource* psrcPending = pstimPending->getSource(p.first);
		g_print("will set blocking (IDLE) probe for src pos %d\n", p.first);
		if (!psrcPending)
			throw std::runtime_error("Pending source not found corresponding to current src");

		HMMNoopCounter *pcounter = new HMMNoopCounter((int)(psrc->streamMap().size()), pcounterPlay);
		for (std::pair<const HMMStreamType, HMMSource::stream_ptr>& pp: psrc->streamMap())
		{
			// now put blocking probe on each stream, with a Noop counter holding a swap counter

			if (pp.first == HMMStreamType::VIDEO)
			{
				pp.second->setProbeID(
						//gst_pad_add_probe(pp.second->srcpad(), GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HMM::padProbeBlockCallback, pcounter, NULL)
						gst_pad_add_probe(pp.second->srcpad(), (GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM|GST_PAD_PROBE_TYPE_IDLE), &HMM::padProbeBlockCallback, pcounter, NULL)
						//gst_pad_add_probe(pp.second->srcpad(), (GstPadProbeType)(GST_PAD_PROBE_TYPE_IDLE/*|GST_PAD_PROBE_TYPE_BLOCK*/), &HMM::padProbeIdleCallback, pcounter, NULL)
						);
				g_print("added probe for stream type %d: block %lu\n", pp.first, pp.second->getProbeID());
			}
		}
	}
	// NOT YET m_stimidCurrent = id;
}


HMMStimID HMM::swap(HMMStimID id)
{
	HMMStimID current = m_stimidCurrent;
	g_print("HMM::swap(current %lu new %lu\n", current, id);
	for (std::pair<const HMMStimPosition, HMMStim::source_ptr>& p: getStim(m_stimidCurrent)->sourceMap())
	{
		if (p.first > 0)
		{
			HMMSource* psrc = p.second.get();
			HMMSource* psrcPending = getStim(id)->getSource(p.first);
			if (!psrcPending)
				throw std::runtime_error("Pending source not found corresponding to current src");
			// does current source have a video stream?
			HMMStream *pvideoCurrent = psrc->getStream(HMMStreamType::VIDEO);
			HMMStream *pvideoPending = psrcPending->getStream(HMMStreamType::VIDEO);

			if (pvideoCurrent)
			{
				if (!pvideoPending)
					throw std::runtime_error("Pending source missing video stream");

				// unlink
				HMMVideoTail* ptail = getVideoTail(p.first);
				if (!ptail)
					throw std::runtime_error("cannot find video tail");
				GstPad *psink = gst_element_get_static_pad(ptail->m_conv, "sink");
				gst_pad_unlink(pvideoCurrent->srcpad(), psink);

				// link
				gst_pad_link(pvideoPending->srcpad(), psink);

				// set offset on tail pad
				GstClockTime abs = gst_clock_get_time(gst_element_get_clock(ptail->m_conv));
				GstClockTime base = gst_element_get_base_time(ptail->m_conv);
				gst_pad_set_offset(psink, abs-base);
				gst_object_unref(psink);

				g_print("Set offset on tail pad to %lums\n", GST_TIME_AS_MSECONDS(abs-base));
				// remove probe on the pending video src pad
				g_print("remove probe on pad, probeid %d\n", pvideoPending->getProbeID());
				gst_pad_remove_probe(pvideoPending->srcpad(), pvideoPending->getProbeID());

				// finally, do flushing seek on removed source
				// TODO - the actual disposition of the source really controlled at the stim level

				GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
				GstStructure *structure = gst_structure_new("seek", "id", G_TYPE_ULONG, current, "pos", G_TYPE_INT, p.first,  NULL);
				gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(pipeline()), structure));
				gst_object_unref(bus);


			}
			else
				throw std::runtime_error("Current source has video pos but no video stream");
		}
	}
	m_stimidCurrent = id;
	g_print("HMM::swap - done\n");
	return current;
}

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
			if (FALSE == gst_structure_get(gst_message_get_structure(msg), "id", G_TYPE_ULONG, &id, "pos", G_TYPE_INT, &pos, NULL))
				throw std::runtime_error("Cannot get stuff from msg");

			g_print("id %lu pos %d\n", id, pos);
			if (!phmm)
				throw std::runtime_error("Cannot get source for stim to be flushed");

			// flushing seek
			if (!gst_element_seek(phmm->getStim(id)->getSource(pos)->bin(), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				throw std::runtime_error("seek failed");
			}
		}
		else if (gst_message_has_name (msg, "loop"))
		{
			g_print("Got loop msg\n");
			HMMStimID id;
			HMMStimPosition pos;
			if (FALSE == gst_structure_get(gst_message_get_structure(msg), "id", G_TYPE_ULONG, &id, "pos", G_TYPE_INT, &pos, NULL))
				throw std::runtime_error("Cannot get stuff from msg");

			g_print("id %lu pos %d\n", id, pos);
			if (!phmm)
				throw std::runtime_error("Cannot get source for stim to be flushed");

			// NON-flushing seek
			if (!gst_element_seek(phmm->getStim(id)->getSource(pos)->bin(), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				throw std::runtime_error("LOOPING seek failed");
			}
		}
		else if (gst_message_has_name (msg, "play"))
		{
			HMMStimID id;
			g_print("Got play msg\n");
			if (FALSE == gst_structure_get(gst_message_get_structure(msg), "id", G_TYPE_ULONG, &id, NULL))
				throw std::runtime_error("Cannot get stuff from play msg");
			g_print("play id %lu\n", id);
			HMMStimID previous = phmm->swap(id);
			g_print("current is %lu prev %lu\n", id, previous);
		}
		break;
	}
	default:
		break;
	}
	return TRUE;
}

HMMStim* HMM::makeStimFromFile(HMMStimID id, const std::string& filename)
{
	HMMStim *pstim=new HMMStim();
	pstim->setStimState(HMMStimState::PREROLLING);

	// Our stim have just one source, a file at pos 1
	HMMStimPrerollCounter *pstimCounter = new HMMStimPrerollCounter(id, pstim, 1);

	std::string uri("file://");
	uri.append(filename);
	GstElement *ele = gst_element_factory_make("uridecodebin", NULL);
	g_object_set (ele, "uri", uri.c_str(), NULL);
	gst_object_ref(ele);
	gst_bin_add(GST_BIN(m_pipeline), ele);
	HMMSource *psrc = new HMMSource(HMMSourceType::VIDEO_ONLY, ele, true);
	pstim->addSource(VIDEO_POS, psrc);

	// make a functor to manage the preroll process
	// TODO this is a mem leak!
	HMMSourcePrerollCounter* psourceCounter = new HMMSourcePrerollCounter(id, VIDEO_POS, psrc, m_pipeline, 1, pstimCounter);

	g_signal_connect (ele, "pad-added", G_CALLBACK(HMM::padAddedCallback), psourceCounter);
	g_signal_connect (ele, "no-more-pads", G_CALLBACK(HMM::noMorePadsCallback), psourceCounter);

	gst_element_sync_state_with_parent (ele);

	return pstim;
}

HMMStim* HMM::makeStimFromDesc(HMMStimID id, const std::string& description)
{
	HMMStim *pstim=NULL;
	throw std::runtime_error("cannot make stim from desc");	// todo, sync with parent()?
	return pstim;
}


void HMM::noMorePadsCallback(GstElement *, HMMSourcePrerollCounter *pcounter)
{
	pcounter->decrement();
}


void HMM::padAddedCallback(GstElement *, GstPad * pad, HMMSourcePrerollCounter *pcounter)
{
	GstCaps *caps;
	GstStructure *s;
	const gchar *name;
	HMMStream *pstream = NULL;

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
					gboolean gb;
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
					pstream = new HMMStream(srcpad);
				}
				else
				{
					gst_object_ref(pad);
					pstream = new HMMStream(pad);
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
	HMMCounter *pcounter = (HMMCounter *)user_data;
	if (pcounter->decrement())
	{
		g_print("HMM::padProbeBlockCallback: decrement() returned TRUE, DO NOT delete counter\n");
		//delete pcounter;
	}
	return GST_PAD_PROBE_OK;
}

GstPadProbeReturn HMM::padProbeIdleCallback(GstPad *pad, GstPadProbeInfo *, gpointer user_data)
{
	HMMCounter *pcounter = (HMMCounter *)user_data;
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


void HMMSourcePrerollCounter::operator()(void)
{
	g_print("HMMSourcePrerollCounter (%lu) triggered\n", m_id);
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
	GstStructure *structure = gst_structure_new("seek", "id", G_TYPE_ULONG, this->id(), "pos", G_TYPE_INT, this->pos(),  NULL);
	gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(this->source()->bin()), structure));
	gst_object_unref(bus);
}

void HMMStimPrerollCounter::operator()(void)
{
	//g_print("HMMStimPrerollCounter (%lu) triggered\n", m_id);
	stim()->setStimState(HMMStimState::PREROLLED);
}

void HMMNoopCounter::operator()(void)
{
	//g_print("HMMNoopCounter triggered\n");
}

void HMMPlayStimCounter::operator()(void)
{
	// post bus message
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(this->hmm()->pipeline()));
	GstStructure *structure = gst_structure_new("play", "id", G_TYPE_ULONG, this->pending(),  NULL);
	gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(hmm()->pipeline()), structure));
	gst_object_unref(bus);
}


void HMMStimSwapCounter::operator()(void)
{
	//g_print("HMMStimSwapCounter triggered\n");

	for (std::pair<const HMMStimPosition, HMMStim::source_ptr>& p: hmm()->getStim(current())->sourceMap())
	{
		if (p.first > 0)
		{
			HMMSource* psrc = p.second.get();
			HMMSource* psrcPending = hmm()->getStim(pending())->getSource(p.first);
			if (!psrcPending)
				throw std::runtime_error("Pending source not found corresponding to current src");
			// does current source have a video stream?
			HMMStream *pvideoCurrent = psrc->getStream(HMMStreamType::VIDEO);
			HMMStream *pvideoPending = psrcPending->getStream(HMMStreamType::VIDEO);

			if (pvideoCurrent)
			{
				if (!pvideoPending)
					throw std::runtime_error("Pending source missing video stream");

				// unlink
				HMMVideoTail* ptail = hmm()->getVideoTail(p.first);
				if (!ptail)
					throw std::runtime_error("cannot find video tail");
				GstPad *psink = gst_element_get_static_pad(ptail->m_conv, "sink");
				gst_pad_unlink(pvideoCurrent->srcpad(), psink);

				// link
				gst_pad_link(pvideoPending->srcpad(), psink);

				// set offset on tail pad
				GstClockTime abs = gst_clock_get_time(gst_element_get_clock(ptail->m_conv));
				GstClockTime base = gst_element_get_base_time(ptail->m_conv);
				gst_pad_set_offset(psink, abs-base);
				gst_object_unref(psink);

				g_print("Set offset on tail pad to %lums\n", GST_TIME_AS_MSECONDS(abs-base));
				// remove probe on the pending video src pad
				gst_pad_remove_probe(pvideoPending->srcpad(), pvideoPending->getProbeID());

//				// finally, do flushing seek on removed source
//				// TODO - the actual disposition of the source really controlled at the stim level
//
				GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(this->hmm()->pipeline()));
				GstStructure *structure = gst_structure_new("seek", "id", G_TYPE_ULONG, this->current(), "pos", G_TYPE_INT, p.first,  NULL);
				gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(this->hmm()->pipeline()), structure));
				gst_object_unref(bus);


			}
			else
				throw std::runtime_error("Current source has video pos but no video stream");
		}
	}
}


GstPadProbeReturn HMM::eventProbeCallback(GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	//HMMSource* psrcPending = (HMMSource *)p;
	HMMSourcePrerollCounter *pcounter = (HMMSourcePrerollCounter *)p;

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
			GstStructure *structure = gst_structure_new("loop", "id", G_TYPE_ULONG, pcounter->id(), "pos", G_TYPE_INT, pcounter->pos(),  NULL);
			gst_bus_post (bus, gst_message_new_application(GST_OBJECT_CAST(pcounter->source()->bin()), structure));
			gst_object_unref(bus);

		}
	}
	return GST_PAD_PROBE_OK;
}
