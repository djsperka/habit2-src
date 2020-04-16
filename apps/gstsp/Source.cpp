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
{};

Source::~Source()
{
	g_print("Source destructor\n");

	// delete the streams
	for (std::pair<HMMStreamType, StreamP> namestream : m_streamMap)
	{
		g_print("delete stream\n");
		delete namestream.second;
	}
	m_streamMap.clear();
	g_print("Source destructor done\n");
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



ColorSource::ColorSource(HMMSourceType stype, const std::string& prefix, GstElement *pipeline)
: Source(stype)
{
	std::ostringstream oss;
	oss << prefix << "-videotestsrc";
	m_ele = gst_element_factory_make("videotestsrc", oss.str().c_str());
	gst_object_ref(m_ele);
	if (m_ele == NULL)
	{
		throw std::runtime_error("Cannot create color source");
	}
	oss.clear();
	oss << prefix << "-fakesink";
	GstElement *sink = gst_element_factory_make("fakesink", oss.str().c_str());
	gst_bin_add_many(GST_BIN(pipeline), m_ele, sink, NULL);
	gst_element_link(m_ele, sink);
	//g_object_set (ele, "pattern", 17, "foreground-color", info.getColor().rgba(), NULL);

	GstPad *srcpad = gst_element_get_static_pad(m_ele, "src");
	gst_object_ref(srcpad);
	addStream(HMMStreamType::VIDEO, srcpad, NULL);
}

ColorSource::~ColorSource()
{
	g_print("ColorSource destructor\n");
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

FileSource::FileSource(HMMSourceType stype, const std::string& filename, const std::string& prefix, GstElement *pipeline, bool bloop, unsigned int volume)
: Source(stype)
, m_bloop(bloop)
, m_volume(volume)
, m_ele(NULL)
, m_bIsImage(false)
, m_freeze(NULL)
{

	std::string uri("file://");
	uri.append(filename);
	std::ostringstream oss;
	oss << prefix << "-uridecodebin";
	m_ele = gst_element_factory_make("uridecodebin", oss.str().c_str());
	gst_object_ref(m_ele);
	gst_bin_add(GST_BIN(pipeline), m_ele);
	g_print("FileSource %s done m_ele %p\n", filename.c_str(), m_ele);
	if (!m_ele)
		throw std::runtime_error("gst_element_factory_make(uridecodebin) returned NULL");
	g_object_set(m_ele, "uri", uri.c_str(), NULL);
}

FileSource::~FileSource()
{
	g_print("FileSource destructor\n");

	if (!m_bIsImage)
	{
		g_print("!image, set NULL %s\n", GST_ELEMENT_NAME(m_ele));
		gst_element_set_state(m_ele, GST_STATE_NULL);
		gst_object_unref(m_ele);
		gst_bin_remove(GST_BIN(this->parentStim()->pipeline()), m_ele);

	}
	else
	{
		// Here we have
		/*
		 *
		 *       +---------------+  +-------------+
		 *       | uridecodebin  |--| imagefreeze |---
		 *       +---------------+  +-------------+
		 *
		 *                         |               |
		 *                    unlink here      this is srcpad, has probe
		 *
		 *                    remove ele from bin - that unlinks
		 */
		g_print("image, ele %s, unlink %s\n", GST_ELEMENT_NAME(m_ele), GST_ELEMENT_NAME(m_freeze));
		gst_element_set_state(m_ele, GST_STATE_NULL);
		gst_object_unref(m_ele);
		gst_bin_remove(GST_BIN(this->parentStim()->pipeline()), m_ele);

		// remove probe on src pad of freeze element
		Stream* str = getStream(HMMStreamType::VIDEO);
		if (str && str->getProbeID())
		{
			g_print("video stream has probe %lu\n", str->getProbeID());
			gst_pad_remove_probe(str->srcpad(), str->getProbeID());
		}

		gst_object_unref(m_freeze);
		g_print("set state...");
		gst_element_set_state(m_freeze, GST_STATE_NULL);
		g_print("...done\n");
		gst_bin_remove(GST_BIN(this->parentStim()->pipeline()), m_freeze);

#if 0
		// set event probe in same place - the callback will remove from pipeline.
		// then  send EOS
		FileSourceDisposalHelper *phelper = new FileSourceDisposalHelper();
		phelper->filter = m_freeze;
		phelper->pipeline = parentStim()->pipeline();
		gst_object_ref(m_freeze);
		gst_object_ref(parentStim()->pipeline());
		gulong pt = gst_pad_add_probe (str->srcpad(), (GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM), &FileSource::padProbeEventCallback, phelper, NULL);
		g_print("added event probe %lu\n", pt);

		GstPad *sinkpad = gst_element_get_static_pad(m_freeze, "sink");
		gst_pad_send_event(sinkpad, gst_event_new_eos());
		gst_object_unref(sinkpad);

		// unref our copy of the freeze element
		gst_object_unref(m_freeze);
#endif
	}
}

GstPadProbeReturn FileSource::padProbeEventCallback(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	FileSourceDisposalHelper *phelper = (FileSourceDisposalHelper *)user_data;
	g_print("FileSource::poadProbeEventCallback() - start\n");

	if (GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info)) != GST_EVENT_EOS)
	    return GST_PAD_PROBE_PASS;

	g_print("FileSource::poadProbeEventCallback() - have EOS\n");
	gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));
	gst_element_set_state(phelper->filter, GST_STATE_NULL);
	gst_bin_remove(GST_BIN(phelper->pipeline), phelper->filter);
	gst_object_unref(phelper->pipeline);
	gst_object_unref(phelper->filter);
	delete phelper;
	g_print("FileSource::poadProbeEventCallback() - done\n");

	return GST_PAD_PROBE_OK;
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
	FileSourcePrerollCounter *pspc = new FileSourcePrerollCounter(this, pipeline, 1, pStimCounter);
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
void FileSource::noMorePadsCallback(GstElement *src, FileSourcePrerollCounter *pspc)
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

void FileSource::padAddedCallback(GstElement *src, GstPad * pad, FileSourcePrerollCounter *pspc)
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
			pspc->fileSource()->addStream(HMMStreamType::VIDEO, pad, NULL, probeid);
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
			pspc->fileSource()->addImageElement(freeze);
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
			pspc->fileSource()->addStream(HMMStreamType::VIDEO, freezesrc, NULL, probeid);

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
		pspc->fileSource()->addStream(HMMStreamType::AUDIO, pad, NULL, probeid);
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
	FileSourcePrerollCounter *pcounter = (FileSourcePrerollCounter *)user_data;
	g_print("In pad probe callback - seeking %s\n", (pcounter->fileSource()->isSeeking() ? "TRUE" : "FALSE"));
	pcounter->decrement();
	return GST_PAD_PROBE_OK;
}
