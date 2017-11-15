/*
 * HStimulusPipeline.cpp
 *
 *  Created on: Nov 14, 2017
 *      Author: dan
 */

#include "HStimulusPipeline.h"
#include "HVideoWidget.h"
#include <QMutexLocker>
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
	qDebug() << "addStimulusInfo:";
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
, m_stimulusLayoutType(layoutType)
, m_bISS(bISS)
, m_pipeline(NULL)
, m_audioMixer(NULL)
, m_audioSink(NULL)
, m_bAudioElementsAddedToPipeline(false)
{
	m_pipeline = gst_pipeline_new(C_STR(makeElementName("pipeline", HPlayerPositionType::Control, this->id())));

	// now depending on the layout type, and whether ISS/sound is to be used, create sub-pipelines for each
	// position Left/Right/Center/Sound as needed

	if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		qDebug() << "Generate pipeline elements for single layout.";
		BinData *pdata = new BinData(this, HPlayerPositionType::Center);
		pdata->stimulusPipeline = this;
		addStimulusInfo(pdata, stimulusSettings.getCenterStimulusInfo(), stimRoot, HPlayerPositionType::Center);
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
		addStimulusInfo(pdataLeft, stimulusSettings.getLeftStimulusInfo(), stimRoot, HPlayerPositionType::Left);
		m_mapBinData.insert(HPlayerPositionType::Left, pdataLeft);	// NOTE: this map now owns the storage associated with pdata -see destructor

		BinData *pdataRight = new BinData(this, HPlayerPositionType::Right);
		addStimulusInfo(pdataRight, stimulusSettings.getRightStimulusInfo(), stimRoot, HPlayerPositionType::Right);
		m_mapBinData.insert(HPlayerPositionType::Right, pdataRight);	// NOTE: this map now owns the storage associated with pdata -see destructor

		qDebug() << "Install bus callback";
		GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
		gst_bus_add_watch(bus, &HStimulusPipeline::busCallback, this);
		gst_object_unref(bus);
	}
	if (iss() && !stimulusSettings.getIndependentSoundInfo().getFileName().trimmed().isEmpty())
	{
		BinData *pdataAudio = new BinData(this, HPlayerPositionType::Sound);
		addStimulusInfo(pdataAudio, stimulusSettings.getIndependentSoundInfo(), stimRoot, HPlayerPositionType::Sound);
		m_mapBinData.insert(HPlayerPositionType::Sound, pdataAudio);	// NOTE: this map now owns the storage associated with pdata -see destructor


		// if this source is looping, set up a bus listener
		// add bus listener. Handler for pad-added signal was added (if needed) in addMedia
		if (pdataAudio->isLooping)
		{
			qDebug() << "Install bus callback";
			GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
			gst_bus_add_watch(bus, &HStimulusPipeline::busCallback, this);
			gst_object_unref(bus);
		}
		else
			qDebug() << "Not looping (audio), no bus callback.";


	}
}

HStimulusPipeline::~HStimulusPipeline()
{
	// clean up all gstreamer stuff we've accumulated
	qDeleteAll(m_mapBinData);
	m_mapBinData.clear();
}

void HStimulusPipeline::ready()
{
	qDebug() << "HStimulusPipeline::ready(" << id() << ")";
	gst_element_set_state(pipeline(), GST_STATE_READY);
}

void HStimulusPipeline::pause()
{
	qDebug() << "HStimulusPipeline::pause(" << id() << ")";
	gst_element_set_state(pipeline(), GST_STATE_PAUSED);
}

void HStimulusPipeline::play()
{
	qDebug() << "HStimulusPipeline::play(" << id() << ")";
	gst_element_set_state(pipeline(), GST_STATE_PLAYING);
}

void HStimulusPipeline::preroll()
{
	// TODO - prepare for prerolling
	qDebug() << "HStimulusPipeline::preroll(" << id() << ")";
	gst_element_set_state(pipeline(), GST_STATE_PAUSED);
}

void HStimulusPipeline::rewind()
{
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
		qDebug() << "padAdded: image - start";
		qDebug() << "padAdded: image - link into pipeline";
		GstElement *videoConvert = makeElement("autovideoconvert", pdata->ppt, pdata->stimulusPipeline->id());
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
		qDebug() << "padAdded: image - done.";
	}
	else if (isVideo)
	{
		qDebug() << "padAdded: video - start";
		if (pdata->ignoreVideo)
		{
			qDebug() << "padAdded: video - ignoring";
			GstElement *sink = makeElement("fakesink", pdata->ppt, pdata->stimulusPipeline->id());
			GstPad *sinkPad = gst_element_get_static_pad(sink, "sink");
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);
			if (!gst_element_sync_state_with_parent(sink))
				qCritical() << "Cannot sync fakesink with pipeline state";
		}
		else
		{
			qDebug() << "padAdded: video - link into pipeline";
			GstElement *videoConvert = makeElement("autovideoconvert", pdata->ppt, pdata->stimulusPipeline->id());
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
				qDebug() << "Install event probe on sink pad";
				gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimulusPipeline::eventProbeCB, (gpointer)pdata, NULL);
			}
			gst_object_unref(sinkPad);
		}
		qDebug() << "padAdded: video - done";
	}
	else if (isAudio)
	{
		qDebug() << "padAdded: audio - start";
		if (pdata->ignoreAudio)
		{
			qDebug() << "padAdded: audio - ignoring";
			GstElement *sink = makeElement("fakesink", pdata->ppt, pdata->stimulusPipeline->id());
			GstPad *sinkPad = gst_element_get_static_pad(sink, "sink");
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);
			if (!gst_element_sync_state_with_parent(sink))
							qCritical() << "Cannot sync sink with pipeline state";
		}
		else
		{
			qDebug() << "padAdded: audio - link into pipeline";
			GstElement *convert = makeElement("audioconvert", pdata->ppt, pdata->stimulusPipeline->id());
			Q_ASSERT(convert);
			GstElement *resample = makeElement("audioresample", pdata->ppt, pdata->stimulusPipeline->id());
			Q_ASSERT(resample);
			GstElement *volume = makeElement("volume", pdata->ppt, pdata->stimulusPipeline->id());
			Q_ASSERT(volume);
			g_object_set(G_OBJECT(volume), "volume", (double)pdata->volume/100.0, NULL);
			qDebug() << "set volume " << pdata->volume;
			gst_bin_add_many(GST_BIN(pdata->stimulusPipeline->pipeline()), convert, resample, volume, NULL);

			// link srcPad to audioconvert ! volume
			GstPad *sinkPad = gst_element_get_static_pad(convert, "sink");
			gst_pad_link(newPad, sinkPad);
			gst_element_link(convert, resample);
			gst_element_link(resample, volume);

			// link to audiomixer element -- need reference up to owning object.
			pdata->stimulusPipeline->lazyAudioInitializeAndAddToPipeline();
			gst_element_link(volume, pdata->stimulusPipeline->m_audioMixer);

			if (pdata->isLooping)
			{
				gulong i;
				i = gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimulusPipeline::eventProbeCB, (gpointer)pdata, NULL);
				qDebug() << "Install event probe on (audio) sink pad: " << i;
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
		qDebug() << "padAdded: audio - done";
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
		qDebug() << "Got ASYNC_DONE from " << GST_MESSAGE_SRC_NAME(msg);
		QMapIterator<HPlayerPositionType, BinData* > it(pSP->getBinDataMap());
		while (it.hasNext())
		{
			it.next();
			if (it.value()->isLooping)
			{
				if (!it.value()->isPrerolled)
				{
					qDebug() << "Src for " << it.key().name() << " requires looping. Send flushing seek.";
					if (!gst_element_seek(it.value()->decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
					{
						qCritical() << "SEEK FAILED";
					}
					else
					{
						qDebug() << "flush seek done " << GST_MESSAGE_SRC_NAME(msg);
						it.value()->isPrerolled = true;
					}
				}
				else
					qDebug() << "Src for " << it.key().name() << " already prerolled";
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
		if (new_state == GST_STATE_PLAYING &&
				parseElementName(GST_MESSAGE_SRC_NAME(msg), factoryName, pppt, id, prefix) &&
				factoryName == "pipeline")
		{
			qDebug() << "STATE CHANGED "<< GST_MESSAGE_SRC_NAME(msg) << factoryName << "/" << pppt->name() << "/" << id << " - " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(new_state);
			pSP->emitNowPlaying();
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
			qDebug() << "Got SEGMENT event from " << parentName;
			g_free(parentName);
			qDebug() << "Running time is " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
			qDebug() << "diff " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent))-last_running_time;
			last_running_time = (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));

			// segment info
			const GstSegment* segment;
			gst_event_parse_segment(event, &segment);
			qDebug() << "Segment: base: " << segment->base << " offset: " << segment->offset << " start: " << segment->start << " stop: " << segment->stop << " time: " << segment->time << " position: " << segment->position << " duration: " << segment->duration;

		}
		else if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT_DONE)
		{
			gchar *parentName = gst_element_get_name(parent);
			qDebug() << "Got SEGMENT_DONE event from " << parentName;
			g_free(parentName);
			// do segment seek
			qDebug() << "Running time is " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
			qDebug() << "diff " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent))-last_running_time;
			last_running_time = (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
			qDebug() << "do segment seek";
			if (!gst_element_seek(pdata->decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << "SEEK FAILED";
			}
		}
	}
	return GST_PAD_PROBE_OK;
}

HPipeline* HStimulusPipelineFactory(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bSound, bool bISS, QObject *parent)
{
	HStimulusPipeline *p = new HStimulusPipeline(id, stimulusSettings, stimRoot, layoutType, bISS, parent);
	return p;
}





