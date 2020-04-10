/*
 * HMMSource.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#include "Source.h"
#include "Counter.h"
#include "Stim.h"
#include <sstream>
using namespace hmm;

Source::Source(HMMSourceType t)
: m_sourceType(t)
, m_parent(NULL)
, m_bSeeking(false)
, m_extra(NULL)
{};

Source::~Source()
{
	// delete the streams
	for (std::pair<HMMStreamType, StreamP> namestream : m_streamMap)
	{
		delete namestream.second;
	}
	m_streamMap.clear();
	if (m_extra)
	{
		gst_bin_remove(GST_BIN(this->parentStim()->pipeline()), m_extra);
		gst_element_set_state(m_extra, GST_STATE_NULL);
		gst_object_unref(m_extra);
		m_extra = NULL;
	}
}

//void Source::addStream(HMMStreamType t, Stream *pstream)
//{
//	m_streamMap.insert(std::make_pair(t, pstream));
//}

Stream *Source::getStream(HMMStreamType t)
{
	Stream *pstream = NULL;
	if (m_streamMap.count(t) == 1)
		pstream = m_streamMap[t];
	return pstream;
}

void Source::addStream(HMMStreamType streamType, GstPad *srcpad, GstElement *sink, gulong probeid)
{
	m_streamMap[streamType] = new Stream(srcpad, this, sink, probeid);
}

void Source::parseCaps(GstCaps* caps, bool& isVideo, bool& isImage, int& width, int& height, bool& isAudio)
{
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;
	gchar *s_new_pad_caps = NULL;
	const GValue *v = NULL;
	new_pad_struct = gst_caps_get_structure(caps, 0);
	new_pad_type = gst_structure_get_name(new_pad_struct);
	s_new_pad_caps = gst_caps_to_string(caps);

	isAudio = g_str_has_prefix(s_new_pad_caps, "audio/x-raw");
	if (g_str_has_prefix(s_new_pad_caps, "video/x-raw"))
	{
		// get width/height
		v = gst_structure_get_value(new_pad_struct, "width");
		if (v && G_VALUE_HOLDS_INT(v))
			width = g_value_get_int(v);
		else
			width = 0;	// shouldn't happen
		v = gst_structure_get_value(new_pad_struct, "height");
		if (v && G_VALUE_HOLDS_INT(v))
			height = g_value_get_int(v);
		else
			height = 0;	// shouldn't happen
		// check framerate
		v = gst_structure_get_value(new_pad_struct, "framerate");
		if (v && GST_VALUE_HOLDS_FRACTION(v))
		{
			gint num = gst_value_get_fraction_numerator(v);
			if (num == 0)
			{
				isImage = true;
				isVideo = false;
			}
			else if (num > 0)
			{
				isImage = false;
				isVideo = true;
			}
			else
			{
				isImage = true;
				isVideo = false;	// shouldn't happen
			}
		}
		else
		{
			isImage = true;
			isVideo = false;
		}
	}
}



ColorSource::ColorSource(HMMSourceType stype, GstElement *ele)
: Source(stype)
, m_ele(ele)
{
	gst_object_ref(ele);
}

ColorSource::~ColorSource()
{
	gst_element_set_state(m_ele, GST_STATE_NULL);
	gst_object_unref(m_ele);
	gst_bin_remove(GST_BIN(this->parentStim()->pipeline()), m_ele);
}

GstElement *ColorSource::bin()
{
	return m_ele;
}

void ColorSource::preroll(GstElement *pipeline, Counter *pStimCounter)
{
	Stream *vs = nullptr;
	g_print("prerolling color\n");

	// must have a single video stream
	vs = getStream(HMMStreamType::VIDEO);
	if (streamCount() != 1 || !vs)
		throw std::runtime_error("cannot preroll color source, must have 1 video stream");

	// set blocking probe and a fake sink
	//GstElement *sink = gst_element_factory_make("fakesink", NULL);
	//gulong probeid = gst_pad_add_probe(vs->srcpad(), GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &ColorSource::padProbeBlockCallback, pspc, NULL);

}

void ColorSource::stop()
{

}

void ColorSource::rewind()
{

}

FileSource::FileSource(HMMSourceType stype, GstElement *ele, bool bloop, unsigned int volume)
: Source(stype)
, m_bloop(bloop)
, m_volume(volume)
, m_ele(ele)
, m_bIsImage(false)
, m_freeze(NULL)
{
	gst_object_ref(ele);
}

FileSource::~FileSource()
{
	gst_element_set_state(m_ele, GST_STATE_NULL);
	gst_object_unref(m_ele);
	gst_bin_remove(GST_BIN(this->parentStim()->pipeline()), m_ele);
	if (m_bIsImage)
	{
		gst_element_set_state(m_freeze, GST_STATE_NULL);
		gst_object_unref(m_ele);
		gst_bin_remove(GST_BIN(this->parentStim()->pipeline()), m_freeze);
	}
}

GstElement *FileSource::bin()
{
	return m_ele;
}

void FileSource::preroll(GstElement *pipeline, Counter *pStimCounter)
{
	g_print("FileSource prerolling\n");

	// a filesource is created with uridecodebin, so we set up the machinery to do this.
	// One thing we need is a counter specific to this source. When triggered it will
	// post a message on the bus
	SourcePrerollCounter *pspc = new SourcePrerollCounter(this, pipeline, 1, pStimCounter);
	//
	// no linking, just deal with pad-added
	g_signal_connect (m_ele, "pad-added", G_CALLBACK(FileSource::padAddedCallback), pspc);
	g_signal_connect (m_ele, "no-more-pads", G_CALLBACK(FileSource::noMorePadsCallback), pspc);

	gst_element_sync_state_with_parent(bin());
}

void FileSource::stop()
{

}

void FileSource::rewind()
{

}

// Need access to counter (decrement)
void FileSource::noMorePadsCallback(GstElement *src, SourcePrerollCounter *pspc)
{
	g_print("noMorePads(%s)\n", GST_ELEMENT_NAME(src));
	// decrement here, negates the initial setting of (1) of counter.
	pspc->decrement();
}


// need access to counter, Source* (must add Streams)
// padAdded called once for each stream, so incrementing counter here is un-done
// when the blocking probe callback is called.
// ALL streams are handled here, and a Stream* is added to the source object. The port will decide
// which stream(s) are needed.

void FileSource::padAddedCallback(GstElement *src, GstPad * pad, SourcePrerollCounter *pspc)
{
	GstCaps *caps;
	GstStructure *s;
	const gchar *name;
	bool isVideo=false, isImage=false, isAudio=false;
	int width=-1, height=-1;

	// get caps, check if its video or audio.
	caps = gst_pad_get_current_caps (pad);
	s = gst_caps_get_structure (caps, 0);
	name = gst_structure_get_name (s);

	GstCaps *new_pad_caps = gst_pad_get_current_caps(pad);
	parseCaps(new_pad_caps, isVideo, isImage, width, height, isAudio);
	gst_caps_unref(new_pad_caps);


	if (strcmp (name, "video/x-raw") == 0)
	{
		// increment counter
		pspc->increment();

		// handle images and video differently
		if (isVideo)
		{
			gulong probeid = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &FileSource::padProbeBlockCallback, pspc, NULL);

			// add fake sink
			GstElement *sink = gst_element_factory_make("fakesink", NULL);
			gst_bin_add(GST_BIN(pspc->pipeline()), sink);
			GstPad *sinkpad = gst_element_get_static_pad(sink, "sink");
			GstPadLinkReturn r = gst_pad_link(pad, sinkpad);
			gst_object_unref(sinkpad);
			if (r != GST_PAD_LINK_OK)
			{
				g_print("fake sink pad link error %d\n", (int)r);
				throw std::runtime_error("Cannot link pads.");
			}

			// save this stream. Note the pad is ref'd
			pspc->source()->addStream(HMMStreamType::VIDEO, pad, NULL, probeid);
			gst_object_ref(pad);

			// sync fakesink
			gst_element_sync_state_with_parent(sink);
			g_print("padAddedCallback(%s) - video stream %d x %d\n", GST_ELEMENT_NAME(src), width, height);
		}
		else if (isImage)
		{
			pspc->setIsImage(true);

			// for images, append an imagefreeze element and use its src pad for probe and connection
			// add fake sink
			//
			//              ---------------                      ------------
			//  (new pad)-->| imagefreeze |(stream.srcpad())---->| fakesink |
			//              ---------------                      ------------
			//
			GstElement *freeze = gst_element_factory_make("imagefreeze", NULL);
			gst_object_ref(freeze);
			pspc->source()->saveExtraElement(freeze);
			GstElement *sink = gst_element_factory_make("fakesink", NULL);
			gst_bin_add_many(GST_BIN(pspc->pipeline()), freeze, sink, NULL);
			GstPad *freezesink = gst_element_get_static_pad(freeze, "sink");
			GstPadLinkReturn r = gst_pad_link(pad, freezesink);
			gboolean blink = gst_element_link(freeze, sink);
			gst_object_unref(freezesink);
			if (r != GST_PAD_LINK_OK || !blink)
			{
				g_print("fake sink pad/imagefreeze link error %d\n", (int)r);
				throw std::runtime_error("Cannot link image pads.");
			}

			// The freeze element is part of the head, not the tail.
			// Using the freeze src pad for probe.
			// The srcpad on the freeze element is used in the stream as the connection point.
			GstPad *freezesrc = gst_element_get_static_pad(freeze, "src");
			gulong probeid = gst_pad_add_probe(freezesrc, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &FileSource::padProbeBlockCallback, pspc, NULL);
			// save this stream. Note the pad is ref'd because of the call to get_static_pad
			// TODO MAKE SURE THIS GETS UNREF'D
			pspc->source()->addStream(HMMStreamType::VIDEO, freezesrc, NULL, probeid);

			// sync fakesink and freeze
			gst_element_sync_state_with_parent(freeze);
			gst_element_sync_state_with_parent(sink);

			g_print("padAddedCallback(%s) - image stream %d x %d\n", GST_ELEMENT_NAME(src), width, height);
		}
	}
	else if (strcmp (name, "audio/x-raw") == 0)
	{
		// increment counter
		pspc->increment();

		// blocking probe
		gulong probeid = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &FileSource::padProbeBlockCallback, pspc, NULL);

		// add fake sink
		GstElement *sink = gst_element_factory_make("fakesink", NULL);
		gst_bin_add(GST_BIN(pspc->pipeline()), sink);
		GstPad *sinkpad = gst_element_get_static_pad(sink, "sink");
		GstPadLinkReturn r = gst_pad_link(pad, sinkpad);
		gst_object_unref(sinkpad);
		if (r != GST_PAD_LINK_OK)
		{
			g_print("fake sink pad link error %d\n", (int)r);
			throw std::runtime_error("Cannot link audio pads.");
		}

		// save this stream. Note the pad is ref'd
		pspc->source()->addStream(HMMStreamType::AUDIO, pad, NULL, probeid);
		gst_object_ref(pad);

		// sync fakesink
		gst_element_sync_state_with_parent(sink);

		g_print("padAddedCallback(%s) - audio stream\n", GST_ELEMENT_NAME(src));

	}

	// free caps
	gst_caps_unref (caps);
}

// probe is called once for each stream set up in padAdded. Decrement counter here.
// Its payload is likely to fire in a call here. Trivial if a single stream, a little less
// so if its a/v. Payload inside functor is a bus message, so needs pipeline, Source*
GstPadProbeReturn FileSource::padProbeBlockCallback(GstPad *, GstPadProbeInfo *, gpointer user_data)
{
	SourcePrerollCounter *pcounter = (SourcePrerollCounter *)user_data;
	g_print("In pad probe callback - seeking %s\n", (pcounter->source()->isSeeking() ? "TRUE" : "FALSE"));
	pcounter->decrement();
	return GST_PAD_PROBE_OK;
}
