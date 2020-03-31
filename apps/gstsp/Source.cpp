/*
 * HMMSource.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#include "Source.h"
#include "Counter.h"
#include <sstream>
using namespace hmm;

Source::Source(HMMSourceType t, Stim *parent)
: m_sourceType(t)
, m_parent(parent)
, m_bSeeking(false)
{};

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



ColorSource::ColorSource(HMMSourceType t, Stim *parent, unsigned int argb)
: Source(t, parent)
, m_argb(argb)
{
	std::ostringstream oss;
	GError *gerror = NULL;
	oss << "videotestsrc pattern=solid-color foreground-color=" << m_argb;
	m_ele = gst_element_factory_make("videotestsrc", NULL);
	if (m_ele == NULL)
	{
		throw std::runtime_error("Cannot create color source");
	}
	g_object_set (m_ele, "pattern", 17, "foreground-color", m_argb, NULL);
	GstPad *srcpad = gst_element_get_static_pad(m_ele, "src");
	addStream(HMMStreamType::VIDEO, srcpad, NULL);
}

GstElement *ColorSource::bin()
{
	return m_ele;
}

void ColorSource::preroll(GstElement *pipeline, Counter *pStimCounter)
{
	g_print("prerolling color %x\n", m_argb);
}

void ColorSource::stop()
{

}

void ColorSource::rewind()
{

}

FileSource::FileSource(HMMSourceType t, Stim *parent, const std::string& filename, bool bloop, unsigned int volume)
: Source(t, parent)
, m_filename(filename)
, m_bloop(bloop)
, m_volume(volume)
{
	std::string uri("file://");
	uri.append(m_filename);
	m_ele = gst_element_factory_make("uridecodebin", NULL);
	if (!m_ele)
		throw std::runtime_error("gst_element_factory_make(uridecodebin) returned NULL");
	g_object_set (m_ele, "uri", uri.c_str(), NULL);
}

GstElement *FileSource::bin()
{
	return m_ele;
}

void FileSource::preroll(GstElement *pipeline, Counter *pStimCounter)
{
	g_print("FileSource prerolling %s\n", m_filename.c_str());

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
void FileSource::noMorePadsCallback(GstElement *, SourcePrerollCounter *pspc)
{
	// decrement here, negates the initial setting of (1) of counter.
	pspc->decrement();
}


// need access to counter, Source* (must add Streams)
// padAdded called once for each stream, so incrementing counter here is un-done
// when the blocking probe callback is called.
void FileSource::padAddedCallback(GstElement *, GstPad * pad, SourcePrerollCounter *pspc)
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


	g_print("padAddedCallback: %s\n", name);
	if (strcmp (name, "video/x-raw") == 0)
	{
		if (pspc->source()->sourceType() == HMMSourceType::VIDEO_ONLY || pspc->source()->sourceType() == HMMSourceType::AUDIO_VIDEO)
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
				pspc->source()->addStream(HMMStreamType::VIDEO, pad, sink, probeid);
				gst_object_ref(pad);
			}
			else if (isImage)
			{
				// for images, append an imagefreeze element and use its src pad for probe and connection
				// add fake sink
				GstElement *freeze = gst_element_factory_make("imagefreeze", NULL);
				GstElement *sink = gst_element_factory_make("fakesink", NULL);
				gst_bin_add_many(GST_BIN(pspc->pipeline()), freeze, sink, NULL);
				GstPad *freezepad = gst_element_get_static_pad(freeze, "sink");
				GstPadLinkReturn r = gst_pad_link(pad, freezepad);
				gboolean blink = gst_element_link(freeze, sink);
				gst_object_unref(freezepad);
				if (r != GST_PAD_LINK_OK || !blink)
				{
					g_print("fake sink pad/imagefreeze link error %d\n", (int)r);
					throw std::runtime_error("Cannot link image pads.");
				}

				// The freeze element is part of the head, not the tail. Use the 'src' pad
				// at the end of the head (the src pad of the imagefreeze element here) for the
				// probe
				freezepad = gst_element_get_static_pad(freeze, "src");
				gulong probeid = gst_pad_add_probe(freezepad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &FileSource::padProbeBlockCallback, pspc, NULL);

				// save this stream. Note the pad is ref'd because of the call to get_static_pad
				// TODO MAKE SURE THIS GETS UNREF'D
				pspc->source()->addStream(HMMStreamType::VIDEO, freezepad, sink, probeid);
				gst_object_unref(freezepad);

			}
		}
	}
	else if (strcmp (name, "audio/x-raw") == 0)
	{
		if (pspc->source()->sourceType() == HMMSourceType::VIDEO_ONLY || pspc->source()->sourceType() == HMMSourceType::AUDIO_VIDEO)
		{
			// increment counter
			pspc->increment();

			// blocking probe
			gulong probeid = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &FileSource::padProbeBlockCallback, pspc, NULL);

			// save this stream. Note the pad is ref'd
			pspc->source()->addStream(HMMStreamType::VIDEO, pad, NULL, probeid);
			gst_object_ref(pad);
		}
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
	if (!pcounter->source()->isSeeking() && pcounter->decrement())
	{
		g_print("HMM::padProbeBlockCallback: decrement() returned TRUE, DO NOT delete counter\n");
		//delete pcounter;
	}
	return GST_PAD_PROBE_OK;
}
