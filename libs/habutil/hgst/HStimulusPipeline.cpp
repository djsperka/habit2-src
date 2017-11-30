/*
 * HStimulusPipeline.cpp
 *
 *  Created on: Nov 14, 2017
 *      Author: dan
 */

#include "HStimulusPipeline.h"
#include "HVideoWidget.h"
#include <QMutexLocker>
#include <gst/gst.h>
#include <gst/videotestsrc/gstvideotestsrc.h>
#include <gst/audiotestsrc/gstaudiotestsrc.h>

guint64 last_running_time=0;

HStimulusPipeline::BinData::BinData(HStimulusPipeline *pipeline, const HPlayerPositionType& pptype)
: stimulusPipeline(pipeline)
, ppt(pptype)
, decodebin(NULL)
, videoSink(NULL)
, ignoreAudio(false)
, ignoreVideo(false)
, isPrerolled(false)
, isLooping(false)
, probeID(0)
, probeElement(NULL)
, volume(0)
{
	if (ppt != HPlayerPositionType::Sound)
	{
		videoSink = makeElement("qwidget5videosink", ppt, stimulusPipeline->id());
		Q_ASSERT(videoSink);
		gst_bin_add_many (GST_BIN (pipeline->pipeline()), videoSink, NULL);
	}
};

void HStimulusPipeline::addStimulusInfo(BinData *pdata, const Habit::StimulusInfo& info, const QDir& stimRoot, const HPlayerPositionType& ppt)
{
	qDebug() << "addStimulusInfo: id " << id() << " - " << ppt.name();
	qDebug() << info;
	if (ppt == HPlayerPositionType::Sound)
	{
		// set up filesrc ! decodebin and padAdded callback
		// create alternate bins to attach for video and image caps
		GstElement *src = makeElement("filesrc", ppt, id());
		g_object_set(G_OBJECT(src), "location", C_STR(info.getAbsoluteFileName(stimRoot)), NULL);
		pdata->decodebin = makeElement("decodebin", ppt, id());
		gst_bin_add_many(GST_BIN(pipeline()), src, pdata->decodebin, NULL);
		gst_element_link(src, pdata->decodebin);

		pdata->isLooping = info.isLoopPlayBack();
		pdata->ignoreVideo = true;
		pdata->ignoreAudio = false;
		pdata->volume = info.getVolume();
		m_mapBinData.insert(ppt, pdata);	// NOTE: this map now owns the storage associated with pdata -see destructor

		g_signal_connect(pdata->decodebin, "pad-added", G_CALLBACK(&HStimulusPipeline::padAdded), pdata);

	}
	else
	{
		// solid color background are completely specified here - no callbacks needed. BinData needed to hold videoSink, however.
		if (info.isColor() || info.isBackground())
		{
			GstElement *src = makeElement("videotestsrc", ppt, id());
			Q_ASSERT(src);
			g_object_set(G_OBJECT(src), "pattern", GST_VIDEO_TEST_SRC_SOLID, "foreground-color", info.getColor().rgba(), NULL);

			gst_bin_add_many (GST_BIN (pipeline()), src, NULL);
			gst_element_link (src, pdata->videoSink);
			m_mapBinData.insert(ppt, pdata);	// NOTE: this map now owns the storage associated with pdata -see destructor
		}
		else
		{
			// set up filesrc ! decodebin and padAdded callback
			// create alternate bins to attach for video and image caps
			GstElement *src = makeElement("filesrc", ppt, id());
			g_object_set(G_OBJECT(src), "location", C_STR(info.getAbsoluteFileName(stimRoot)), NULL);
			pdata->decodebin = makeElement("decodebin", ppt, id());
			gst_bin_add_many(GST_BIN(pipeline()), src, pdata->decodebin, NULL);
			gst_element_link(src, pdata->decodebin);

			pdata->isLooping = info.isLoopPlayBack();
			pdata->ignoreVideo = false;					// accept video stream
			pdata->volume = info.getVolume();
			pdata->ignoreAudio = (info.getVolume() < 1);	// ignore audio if muted, otherwise accept if present
			if (pdata->ignoreAudio)
				qDebug() << "audio is muted for this src, will ignore any audio streams if present.";
			m_mapBinData.insert(ppt, pdata);	// NOTE: this map now owns the storage associated with pdata -see destructor

			g_signal_connect(pdata->decodebin, "pad-added", G_CALLBACK(&HStimulusPipeline::padAdded), pdata);

		}
	}
}

void HStimulusPipeline::setSizeOnWidget(HVideoWidget *w, const HPlayerPositionType& ppt)
{
	if (m_mapBinData.contains(ppt))
	{
		if (w)
		{
			w->setStimulusSize(m_mapBinData[ppt]->size);
		}
	}
	else
	{
		qCritical() << "Cannot set size on widget for pipleline " << QString(GST_ELEMENT_NAME(pipeline())) << ": no bin data for position " << ppt.name() << ".";
	}
}

void HStimulusPipeline::setWidgetPropertyOnSink(HVideoWidget *w, const HPlayerPositionType& ppt)
{
	GValue v = G_VALUE_INIT;
	g_value_init(&v, G_TYPE_POINTER);
	g_value_set_pointer(&v, w);

	if (m_mapBinData.contains(ppt))
	{
		g_object_set_property(G_OBJECT(m_mapBinData[ppt]->videoSink), "widget", &v);
	}
	else
	{
		qCritical() << "Cannot set widget property on pipleline " << QString(GST_ELEMENT_NAME(pipeline())) << ": no bin data for position " << ppt.name() << ".";
	}
}

void HStimulusPipeline::lazyAudioInitializeAndAddToPipeline()
{
	if (!m_bAudioElementsAddedToPipeline)
	{

		//GstElement *src = makeElement("audiotestsrc", HPlayerPositionType::Control, this->id());
		//Q_ASSERT(src);
		//g_object_set(G_OBJECT(src), "wave", GST_AUDIO_TEST_SRC_WAVE_SILENCE, NULL);
		m_audioSink = makeElement("osxaudiosink", HPlayerPositionType::Control, this->id());
		Q_ASSERT(m_audioSink);
		m_audioMixer = makeElement("audiomixer", HPlayerPositionType::Control, this->id());
		Q_ASSERT(m_audioMixer);

		gst_bin_add_many(GST_BIN(pipeline()), m_audioMixer, m_audioSink, NULL);
		gst_element_link(m_audioMixer, m_audioSink);
		if (!gst_element_sync_state_with_parent(m_audioMixer))
			qCritical() << "Cannot sync audiomixer with pipeline state";
		if (!gst_element_sync_state_with_parent(m_audioSink))
			qCritical() << "Cannot sync audiosink with pipeline state";

		m_bAudioElementsAddedToPipeline = true;
	}
}


HStimulusPipeline::HStimulusPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bISS, QObject *parent)
: HPipeline(id, parent)
, m_bInitialized(false)
, m_stimulusSettings(stimulusSettings)
, m_dirStimRoot(stimRoot)
, m_stimulusLayoutType(layoutType)
, m_bISS(bISS)
, m_pipeline(NULL)
, m_audioMixer(NULL)
, m_audioSink(NULL)
, m_bAudioElementsAddedToPipeline(false)
{
}

void HStimulusPipeline::initialize()
{
	if (m_bInitialized) return;

	m_pipeline = gst_pipeline_new(C_STR(makeElementName("pipeline", HPlayerPositionType::Control, this->id())));

	// now depending on the layout type, and whether ISS/sound is to be used, create sub-pipelines for each
	// position Left/Right/Center/Sound as needed

	if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		qDebug() << "Generate pipeline elements for single layout.";
		BinData *pdata = new BinData(this, HPlayerPositionType::Center);
		pdata->stimulusPipeline = this;
		addStimulusInfo(pdata, m_stimulusSettings.getCenterStimulusInfo(), m_dirStimRoot, HPlayerPositionType::Center);
		m_mapBinData.insert(HPlayerPositionType::Center, pdata);	// NOTE: this map now owns the storage associated with pdata -see destructor

		// set up bus callback for all sources.
		// The calback will handle state changed messages (emitting appropriate SIGNALs), and ASYNC_DONE messages
		// (will set up looping for sources that require it).
		qDebug() << "Install bus callback";
		GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
		gst_bus_add_watch(bus, &HStimulusPipeline::busCallback, this);
		gst_object_unref(bus);
	}
	else if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		qDebug() << "Generate pipeline elements for l/r layout.";

		BinData *pdataLeft = new BinData(this, HPlayerPositionType::Left);
		addStimulusInfo(pdataLeft, m_stimulusSettings.getLeftStimulusInfo(), m_dirStimRoot, HPlayerPositionType::Left);
		m_mapBinData.insert(HPlayerPositionType::Left, pdataLeft);	// NOTE: this map now owns the storage associated with pdata -see destructor

		BinData *pdataRight = new BinData(this, HPlayerPositionType::Right);
		addStimulusInfo(pdataRight, m_stimulusSettings.getRightStimulusInfo(), m_dirStimRoot, HPlayerPositionType::Right);
		m_mapBinData.insert(HPlayerPositionType::Right, pdataRight);	// NOTE: this map now owns the storage associated with pdata -see destructor

		qDebug() << "Install bus callback";
		GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
		gst_bus_add_watch(bus, &HStimulusPipeline::busCallback, this);
		gst_object_unref(bus);
	}
	if (iss() && !m_stimulusSettings.getIndependentSoundInfo().getFileName().trimmed().isEmpty())
	{
		BinData *pdataAudio = new BinData(this, HPlayerPositionType::Sound);
		addStimulusInfo(pdataAudio, m_stimulusSettings.getIndependentSoundInfo(), m_dirStimRoot, HPlayerPositionType::Sound);
		m_mapBinData.insert(HPlayerPositionType::Sound, pdataAudio);	// NOTE: this map now owns the storage associated with pdata -see destructor


		// if this source is looping, set up a bus listener
		// add bus listener. Handler for pad-added signal was added (if needed) in addMedia
		if (pdataAudio->isLooping)
		{
			qDebug() << "Install bus callback for sound";
			GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
			gst_bus_add_watch(bus, &HStimulusPipeline::busCallback, this);
			gst_object_unref(bus);
		}
		else
			qDebug() << "Not looping (audio), no bus callback.";


	}
	m_bInitialized = true;
}

HStimulusPipeline::~HStimulusPipeline()
{
	cleanup();
}

void HStimulusPipeline::cleanup()
{
	if (!m_bInitialized) return;

	// drop bus watch
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
	gst_bus_remove_watch(bus);
	gst_object_unref(bus);

	// if any of the sources were looped, remove their probes
	QMapIterator<HPlayerPositionType, BinData* > it(m_mapBinData);
	while (it.hasNext())
	{
		it.next();
		if (it.value()->isLooping)
		{
			qDebug() << "Remove probe ID " << it.value()->probeID << " from element " << GST_ELEMENT_NAME(it.value()->probeElement);
			GstPad *sinkPad = gst_element_get_static_pad(it.value()->probeElement, "sink");
			Q_ASSERT(sinkPad);
			gst_pad_remove_probe(sinkPad, it.value()->probeID);
			gst_object_unref(sinkPad);
		}
	}


	gst_element_set_state(pipeline(), GST_STATE_NULL);
	qDeleteAll(m_mapBinData);
	m_mapBinData.clear();
	gst_object_unref(m_pipeline);
	m_bInitialized = false;
	m_bAudioElementsAddedToPipeline = false;
}

//void HStimulusPipeline::null()
//{
//	qDebug() << "HStimulusPipeline::null(" << id() << ")";
//	gst_element_set_state(pipeline(), GST_STATE_NULL);
//}
//
void HStimulusPipeline::ready()
{
	qDebug() << "HStimulusPipeline::ready(" << id() << ")";
	initialize();
	gst_element_set_state(pipeline(), GST_STATE_READY);
}

void HStimulusPipeline::pause()
{
	qDebug() << "HStimulusPipeline::pause(" << id() << ")";
	initialize();
	gst_element_set_state(pipeline(), GST_STATE_PAUSED);
}

void HStimulusPipeline::play()
{
	qDebug() << "HStimulusPipeline::play(" << id() << ")";
	initialize();
	gst_element_set_state(pipeline(), GST_STATE_PLAYING);
}

void HStimulusPipeline::preroll()
{
	// TODO - prepare for prerolling
	qDebug() << "HStimulusPipeline::preroll(" << id() << ")";
	initialize();
	gst_element_set_state(pipeline(), GST_STATE_PAUSED);
}

void HStimulusPipeline::rewind()
{
	if (!m_bInitialized)
	{
		qCritical() << "Cannot rewind - initialize() first!";
		return;
	}

	// flushing seek on each of the decodebin elements (if any).
	QMapIterator<HPlayerPositionType, BinData* > it(m_mapBinData);
	while (it.hasNext())
	{
		it.next();
		if (it.value()->decodebin)
		{
			qDebug() << "Send flushing seek to " << GST_ELEMENT_NAME(it.value()->decodebin);
			if (!gst_element_seek(it.value()->decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << "rewind() - SEEK FAILED";
			}
			else
			{
				qDebug() << "rewind() - flush seek done " << GST_ELEMENT_NAME(it.value()->decodebin);
				it.value()->isPrerolled = true;
			}
		}
	}
}

void HStimulusPipeline::dump()
{
	if (!m_bInitialized)
	{
		qCritical() << "Cannot dump - initialize() first!";
		return;
	}

	qDebug() << "Dump dot file base name " << GST_ELEMENT_NAME(pipeline());
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline()), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME(pipeline()));

}

void HStimulusPipeline::attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1)
{
	if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		setWidgetPropertyOnSink(w0, HPlayerPositionType::Center);
		if (w0)
			setSizeOnWidget(w0, HPlayerPositionType::Center);
	}
	else if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		setWidgetPropertyOnSink(w0, HPlayerPositionType::Left);
		if (w0)
			setSizeOnWidget(w0, HPlayerPositionType::Left);
		setWidgetPropertyOnSink(w1, HPlayerPositionType::Right);
		if (w1)
			setSizeOnWidget(w1, HPlayerPositionType::Right);
	}
}

void HStimulusPipeline::detachWidgetsFromSinks()
{
	attachWidgetsToSinks(NULL, NULL);
}


void HStimulusPipeline::write(std::ostream& os) const
{
	os << "Pipeline Key: " << id() << std::endl;
	os << "Name: " << GST_ELEMENT_NAME(pipeline()) << std::endl;
}

void HStimulusPipeline::emitNowPlaying()
{
	emit nowPlaying();
}

void HStimulusPipeline::parseCaps(GstCaps* caps, bool& isVideo, bool& isImage, int& width, int& height, bool& isAudio)
{
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;
	gchar *s_new_pad_caps = NULL;
	const GValue *v = NULL;
	new_pad_struct = gst_caps_get_structure(caps, 0);
	new_pad_type = gst_structure_get_name(new_pad_struct);
	s_new_pad_caps = gst_caps_to_string(caps);
	qDebug() << "parseCaps: " << s_new_pad_caps;

	isAudio = g_str_has_prefix(s_new_pad_caps, "audio/x-raw");
	if (g_str_has_prefix(s_new_pad_caps, "video/x-raw"))
	{
		// get width/height
		v = gst_structure_get_value(new_pad_struct, "width");
		if (v && G_VALUE_HOLDS_INT(v))
			width = g_value_get_int(v);
		else
			qCritical() << "Width is not an int!?!";

		v = gst_structure_get_value(new_pad_struct, "height");
		if (v && G_VALUE_HOLDS_INT(v))
			height = g_value_get_int(v);
		else
			qCritical() << "Height is not an int!?!";

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
			else qCritical() << "negative numerator?";
		}
		else qCritical() << "framerate not in caps or unexpected value type";
	}
	qDebug() << "Video? " << isVideo << " Image? " << isImage << " Audio? :" << isAudio << " Resolution: " << width << "x" << height;
}

void HStimulusPipeline::padAdded(GstElement *src, GstPad *newPad, gpointer p)
{
	BinData *pdata = (BinData *)p;

	// get lock on the HStimulusPipeline object
	QMutexLocker locker(pdata->stimulusPipeline->mutex());
	qDebug() << "padAdded: Got new pad '" << GST_PAD_NAME(newPad) << " from " << GST_ELEMENT_NAME(src);


	// get the caps and parse them. That will tell us whether its audio or video. If its video, then
	// we get the frame rate - that tells us whether its an image (rate=0) or video. We also get the
	// resolution.

	GstCaps *new_pad_caps = NULL;
	new_pad_caps = gst_pad_get_current_caps(newPad);
	bool isVideo=false, isImage=false, isAudio=false;
	int width=-1, height=-1;
	parseCaps(new_pad_caps, isVideo, isImage, width, height, isAudio);
	gst_caps_unref(new_pad_caps);

	if (isImage)
	{
		qDebug() << "padAdded: image - " << GST_ELEMENT_NAME(src) << " - link into pipeline";
		GstElement *videoConvert = makeElement("videoconvert", pdata->ppt, pdata->stimulusPipeline->id());
		GstElement *freeze = makeElement("imagefreeze", pdata->ppt, pdata->stimulusPipeline->id());
		gst_bin_add_many(GST_BIN(pdata->stimulusPipeline->pipeline()), videoConvert, freeze, NULL);

		GstPad *sinkPad = gst_element_get_static_pad(videoConvert, "sink");
		gst_pad_link(newPad, sinkPad);
		gst_object_unref(sinkPad);
		gst_element_link (videoConvert, freeze);
		gst_element_link (freeze, pdata->videoSink);

		// set states of all newly-added elements.
		if (!gst_element_sync_state_with_parent(videoConvert))
			qCritical() << "Cannot sync videoConvert with pipeline state";
		if (!gst_element_sync_state_with_parent(freeze))
			qCritical() << "Cannot sync freeze with pipeline state";
		if (!gst_element_sync_state_with_parent(pdata->videoSink))
			qCritical() << "Cannot sync sink with pipeline state";

		pdata->size.setHeight(height);
		pdata->size.setWidth(width);
		qDebug() << "padAdded: image - " << GST_ELEMENT_NAME(src) << " - done.";
	}
	else if (isVideo)
	{
		//qDebug() << "padAdded: video - start";
		if (pdata->ignoreVideo)
		{
			qDebug() << "padAdded: video - " << GST_ELEMENT_NAME(src) << " -  ignoring";
#ifdef LINK_IGNORED_SRC_PADS
			GstElement *sink = makeElement("fakesink", pdata->ppt, pdata->stimulusPipeline->id());
			gst_bin_add(GST_BIN(pdata->stimulusPipeline->pipeline()), sink);
			GstPad *sinkPad = gst_element_get_static_pad(sink, "sink");
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);
			qDebug() << "padAdded: video - ignored src pad is linked to a fake sink";
			if (!gst_element_sync_state_with_parent(sink))
				qCritical() << "Cannot sync fakesink with pipeline state";
#else
			qDebug() << "padAdded: video - ignored src pad is unlinked";
#endif
		}
		else
		{
			qDebug() << "padAdded: video - " << GST_ELEMENT_NAME(src) << " - link into pipeline";
			GstElement *videoConvert = makeElement("videoconvert", pdata->ppt, pdata->stimulusPipeline->id());
			gst_bin_add_many(GST_BIN(pdata->stimulusPipeline->pipeline()), videoConvert, NULL);

			GstPad *sinkPad = gst_element_get_static_pad(videoConvert, "sink");
			gst_pad_link(newPad, sinkPad);
			gst_element_link (videoConvert, pdata->videoSink);

			// set states of all newly-added elements.
			if (!gst_element_sync_state_with_parent(videoConvert))
				qCritical() << "Cannot sync videoConvert with pipeline state";
			if (!gst_element_sync_state_with_parent(pdata->videoSink))
				qCritical() << "Cannot sync sink with pipeline state";

			pdata->size.setHeight(height);
			pdata->size.setWidth(width);

			// if the BinData canLoop and isLooping, install an event probe on the sink pad coming out of the decodebin
			if (pdata->isLooping)
			{
				qDebug() << "padAdded: video will loop, install event probe on sink pad of " << GST_ELEMENT_NAME(videoConvert);
				pdata->probeID = gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimulusPipeline::eventProbeCB, (gpointer)pdata, NULL);
				pdata->probeElement = videoConvert;	// the element that sinkPad came from
			}
			gst_object_unref(sinkPad);
		}
		qDebug() << "padAdded: video - " << GST_ELEMENT_NAME(src) << " -  done";
	}
	else if (isAudio)
	{
		//qDebug() << "padAdded: audio - start";
		if (pdata->ignoreAudio)
		{
			qDebug() << "padAdded: audio - " << GST_ELEMENT_NAME(src) << " - ignoring";
#ifdef LINK_IGNORED_SRC_PADS
			GstElement *sink = makeElement("fakesink", pdata->ppt, pdata->stimulusPipeline->id());
			gst_bin_add(GST_BIN(pdata->stimulusPipeline->pipeline()), sink);
			GstPad *sinkPad = gst_element_get_static_pad(sink, "sink");
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);
			qDebug() << "padAdded: audio - ignored src pad is linked to a fake sink";
			if (!gst_element_sync_state_with_parent(sink))
							qCritical() << "Cannot sync sink with pipeline state";
#else
			qDebug() << "padAdded: audio - ignored src pad is unlinked";
#endif
		}
		else
		{
			qDebug() << "padAdded: audio - " << GST_ELEMENT_NAME(src) << " - link into pipeline";
			GstElement *convert = makeElement("audioconvert", pdata->ppt, pdata->stimulusPipeline->id());
			Q_ASSERT(convert);
			GstElement *resample = makeElement("audioresample", pdata->ppt, pdata->stimulusPipeline->id());
			Q_ASSERT(resample);
			GstElement *volume = makeElement("volume", pdata->ppt, pdata->stimulusPipeline->id());
			Q_ASSERT(volume);
			g_object_set(G_OBJECT(volume), "volume", (double)pdata->volume/100.0, NULL);
			qDebug() << "padAdded: set audio volume to [0-1] " << pdata->volume;
			gst_bin_add_many(GST_BIN(pdata->stimulusPipeline->pipeline()), convert, resample, volume, NULL);

			// link srcPad to audioconvert ! volume
			GstPad *sinkPad = gst_element_get_static_pad(convert, "sink");
			GstPadLinkReturn padLinkReturn = gst_pad_link(newPad, sinkPad);
			if (GST_PAD_LINK_FAILED(padLinkReturn))
			{
				qCritical() << "Cannot link new pad to audioconvert sink pad, GstPadLinkReturn=" << padLinkReturn;
			}
			if (!gst_element_link(convert, resample))
			{
				qCritical() << "Cannot link audioconvert to resample element.";
			}
			if (!gst_element_link(resample, volume))
			{
				qCritical() << "Cannot link resample to volume element.";
			}

			// link to audiomixer element -- need reference up to owning object.
			pdata->stimulusPipeline->lazyAudioInitializeAndAddToPipeline();
			if (!gst_element_link(volume, pdata->stimulusPipeline->m_audioMixer))
			{
				qCritical() << "Cannot link volume to audiomixer element.";
			}

			if (pdata->isLooping)
			{
				pdata->probeID = gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimulusPipeline::eventProbeCB, (gpointer)pdata, NULL);
				pdata->probeElement = convert;
				qDebug() << "padAdded: audio will loop, install event probe on sink pad of " << GST_ELEMENT_NAME(convert);
			}
			gst_object_unref(sinkPad);

			// set states of all newly-added elements.
			if (!gst_element_sync_state_with_parent(convert))
				qCritical() << "Cannot sync convert with pipeline state";
			if (!gst_element_sync_state_with_parent(resample))
				qCritical() << "Cannot sync resample with pipeline state";
			if (!gst_element_sync_state_with_parent(volume))
				qCritical() << "Cannot sync volume with pipeline state";

		}
		qDebug() << "padAdded: audio - " << GST_ELEMENT_NAME(src) << " -  done";
	}
}

gboolean HStimulusPipeline::busCallback(GstBus *, GstMessage *msg, gpointer pdata)
{
	HStimulusPipeline *pSP = (HStimulusPipeline *)pdata;

	//qDebug() << "busCallback: " << GST_MESSAGE_TYPE_NAME(msg) << " from " << GST_MESSAGE_SRC_NAME(msg);
	if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ASYNC_DONE)
	{
		// if any of the pieces of the pipeline need looping, then do a flushing seek now
		// Note that the ASYNC_DONE is sent from the pipeline and is received on the bus.
		// We send the flushing seek to the decodebin element for the portion(s) of the pipeline
		// that require looping
		GstClockTime runningTime;
		gst_message_parse_async_done(msg, &runningTime);
		qDebug() << "busCallback(" << GST_MESSAGE_SRC_NAME(msg) << "): Got ASYNC_DONE, running time " << runningTime;
		QMapIterator<HPlayerPositionType, BinData* > it(pSP->getBinDataMap());
		while (it.hasNext())
		{
			it.next();
			if (it.value()->isLooping)
			{
				if (!it.value()->isPrerolled)
				{
					qDebug() << "busCallback(" << GST_MESSAGE_SRC_NAME(msg) << "): Src for " << it.key().name() << " requires looping. Send flushing seek.";
					if (!gst_element_seek(it.value()->decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
					{
						qCritical() << "SEEK FAILED";
					}
					else
					{
						qDebug() << "busCallback(" << GST_MESSAGE_SRC_NAME(msg) << "): flush seek done";
						it.value()->isPrerolled = true;
					}
				}
				else
					qDebug() << "busCallback(" << GST_MESSAGE_SRC_NAME(msg) << "): Src for " << it.key().name() << " already prerolled";
			}
		}
	}
	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_STATE_CHANGED)
	{
		GstState old_state, new_state;
		QString factoryName, prefix;
		int id;
		const HPlayerPositionType* pppt;
		gst_message_parse_state_changed(msg, &old_state, &new_state, NULL);
		//qDebug() << "STATE CHANGED "<< GST_MESSAGE_SRC_NAME(msg) << " - " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(new_state);
		if (	parseElementName(GST_MESSAGE_SRC_NAME(msg), factoryName, pppt, id, prefix) &&
			factoryName == "pipeline")
		{
			if (new_state == GST_STATE_PLAYING)
			{
				qDebug() << "busCallback(" << GST_MESSAGE_SRC_NAME(msg) << "): got STATE_CHANGED " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(new_state);
				pSP->emitNowPlaying();
			}
			else if (new_state == GST_STATE_PAUSED && old_state == GST_STATE_PLAYING)
			{
				qDebug() << "busCallback(" << GST_MESSAGE_SRC_NAME(msg) << "): got STATE_CHANGED " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(new_state);
			}
		}
	}
	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR)
	{
		GError *err = NULL;
		gchar *dbg_info = NULL;

		gst_message_parse_error(msg, &err, &dbg_info);
		qCritical() << "ERROR from element " << GST_OBJECT_NAME(msg->src) << ": " << err->message;
		qCritical() << "Debugging info: " << ((dbg_info) ? dbg_info : "none");
		g_error_free (err);
		g_free (dbg_info);
	}
	return TRUE;
}

GstPadProbeReturn HStimulusPipeline::eventProbeCB (GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	BinData *pdata = (BinData *)p;

	// Note: GST_PAD_PROBE_INFO_EVENT(d) returns a GstEvent* or NULL if info->data does not have an event.
	// In this routine, because its an event probe, it should always contain an event?

	GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
	if (event)
	{
		GstElement* parent = GST_PAD_PARENT(pad);
		if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT)
		{
			gchar *parentName = gst_element_get_name(parent);
			qDebug() << "eventProbeCB(" << parentName << "): got SEGMENT event";
			// segment info
			const GstSegment* segment;
			gst_event_parse_segment(event, &segment);
			qDebug() << "eventProbeCB(" << parentName << "): segment base: " << segment->base << " offset: " << segment->offset << " start: " << segment->start << " stop: " << segment->stop << " time: " << segment->time << " position: " << segment->position << " duration: " << segment->duration;
			g_free(parentName);

		}
		else if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT_DONE)
		{
			gchar *parentName = gst_element_get_name(parent);
			qDebug() << "eventProbeCB(" << parentName << ") got SEGMENT_DONE event";
			// do segment seek
			qDebug() << "eventProbeCB(" << parentName << "): Running time is " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
			qDebug() << "eventProbeCB(" << parentName << "): diff " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent))-last_running_time;
			last_running_time = (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
			qDebug() << "eventProbeCB(" << parentName << "): do segment seek";
			if (!gst_element_seek(pdata->decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << "SEEK FAILED";
			}
			g_free(parentName);
		}
	}
	return GST_PAD_PROBE_OK;
}

HPipeline* HStimulusPipelineFactory(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool, bool bISS, QObject *parent)
{
	HStimulusPipeline *p = new HStimulusPipeline(id, stimulusSettings, stimRoot, layoutType, bISS, parent);
	return p;
}





