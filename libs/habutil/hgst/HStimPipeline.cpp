/*
 * HStimPipeline.cpp
 *
 *  Created on: Dec 14, 2017
 *      Author: dan
 */

#include "HVideoWidget.h"
#include <QMutexLocker>
#include <gst/videotestsrc/gstvideotestsrc.h>
#include <hgst/HStimPipeline.h>
#include <hgst/HStaticStimPipeline.h>

HStimPipelineSource::HStimPipelineSource(HStimPipeline *pipe)
: m_pStimPipeline(pipe)
, bWaitingForPreroll(false)
, bPrerolled(false)
, bWaitingForSegment(false)
, bAudio(false)
, bVideo(false)
{
}

HStimPipelineSource::~HStimPipelineSource()
{

}

HStimPipeline::HStimPipeline(int id, const Habit::StimulusSettings& ss, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bISS, QObject *parent)
: HPipeline(id, ss, parent)
, m_bInitialized(false)
, m_dirStimRoot(stimRoot)
, m_stimulusLayoutType(layoutType)
, m_bISS(bISS)
#if !PREROLL_SOME_SOURCES
, bInitialFlushingSeekDone(false)
#endif
{
}

HStimPipeline::~HStimPipeline()
{
	// TODO Auto-generated destructor stub
}

GstElement *HStimPipelineSource::pipeline()
{
	return stimPipeline()->pipeline();
}

void HStimPipeline::emitNowPlaying()
{
	Q_EMIT nowPlaying();
}

void HStimPipeline::pause()
{
	qDebug() << "HStimPipeline::pause(" << id() << ")";
	initialize();
	gst_element_set_state(pipeline(), GST_STATE_PAUSED);
}

void HStimPipeline::play()
{
	qDebug() << "HStimPipeline::play(" << id() << ")";
	initialize();
	gst_element_set_state(pipeline(), GST_STATE_PLAYING);
}

void HStimPipeline::setWidgetPropertyOnSink(HVideoWidget *w, const HPlayerPositionType& ppt)
{
	GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline()), C_STR(makeElementName("qwidget5videosink", ppt, id())));
	Q_ASSERT(sink);

	GValue v = G_VALUE_INIT;
	g_value_init(&v, G_TYPE_POINTER);
	g_value_set_pointer(&v, w);

	g_object_set_property(G_OBJECT(sink), "widget", &v);
}

void HStimPipeline::attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1)
{
	if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		setWidgetPropertyOnSink(w0, HPlayerPositionType::Center);
		if (w0)
			w0->setStimulusSize(m_mapPipelineSources[HPlayerPositionType::Center]->size);
	}
	else if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		setWidgetPropertyOnSink(w0, HPlayerPositionType::Left);
		if (w0)
			w0->setStimulusSize(m_mapPipelineSources[HPlayerPositionType::Left]->size);
		setWidgetPropertyOnSink(w1, HPlayerPositionType::Right);
		if (w1)
			w1->setStimulusSize(m_mapPipelineSources[HPlayerPositionType::Right]->size);
	}
}

void HStimPipeline::detachWidgetsFromSinks()
{
	attachWidgetsToSinks(NULL, NULL);
}


void HStimPipeline::cleanup()
{
	// cleanup
	if (m_bInitialized)
	{
		qDebug() << "HStimPipeline::cleanup id " << id();
		GstBus *	bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
		gst_bus_remove_watch(bus);
		gst_object_unref(bus);
		gst_element_set_state(m_pipeline, GST_STATE_NULL);
		gst_object_unref(m_pipeline);
		qDeleteAll(m_mapPipelineSources);
		m_mapPipelineSources.clear();
		m_bInitialized = false;
	}
	else
	{
		qDebug() << "HStimPipeline::cleanup id " << id() << " not intialized.";
	}
}

void HStimPipeline::initialize()
{
	if (m_bInitialized) return;

	m_pipeline = gst_pipeline_new(C_STR(makeElementName("pipeline", HPlayerPositionType::Control, this->id())));
	Q_ASSERT(m_pipeline);

	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	gst_bus_add_watch(bus, &HStimPipeline::busCallback, this);
	gst_object_unref(bus);

	// now depending on the layout type, and whether ISS/sound is to be used, create sub-pipelines for each
	// position Left/Right/Center/Sound as needed

	if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		qDebug() << "Generate pipeline elements for single screen layout.";
		m_mapPipelineSources.insert(HPlayerPositionType::Center, addStimulusInfo(HPlayerPositionType::Center, stimulusSettings().getCenterStimulusInfo(), !m_bISS));
	}
	else if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		qDebug() << "Generate pipeline elements for l/r layout.";
		m_mapPipelineSources.insert(HPlayerPositionType::Left, addStimulusInfo(HPlayerPositionType::Left, stimulusSettings().getLeftStimulusInfo(), !m_bISS));
		m_mapPipelineSources.insert(HPlayerPositionType::Right, addStimulusInfo(HPlayerPositionType::Right, stimulusSettings().getRightStimulusInfo(), !m_bISS));
	}
	if (iss() && !stimulusSettings().getIndependentSoundInfo().getFileName().trimmed().isEmpty())
	{
		m_mapPipelineSources.insert(HPlayerPositionType::Sound, addStimulusInfo(HPlayerPositionType::Sound, stimulusSettings().getIndependentSoundInfo(), true));
	}
	m_bInitialized = true;
}

HStimPipelineSource *HStimPipeline::addStimulusInfo(const HPlayerPositionType& ppt, const Habit::StimulusInfo& info, bool bAudio)
{
	QMutexLocker locker(mutex());

	HStimPipelineSource *pStimPipelineSource = new HStimPipelineSource(this);

	if (ppt == HPlayerPositionType::Sound)
	{
		GstElement *src, *decodebin;

		/* Create the elements */
		src = makeElement("filesrc", ppt, id());
		Q_ASSERT(src);
		g_object_set(G_OBJECT(src), "location", C_STR(info.getAbsoluteFileName(m_dirStimRoot)), NULL);

		decodebin = makeElement("decodebin", ppt, id());
		Q_ASSERT(decodebin);
		g_signal_connect(decodebin, "pad-added", G_CALLBACK(&padAdded), pStimPipelineSource);

		/*
		 * Build the partial pipeline
		 */

		gst_bin_add_many (GST_BIN(m_pipeline), src, decodebin, NULL);
		if (gst_element_link(src, decodebin) != TRUE)
		{
			qCritical() << "src-decodebin could not be linked.";
			gst_object_unref (m_pipeline);
			delete pStimPipelineSource;
			return NULL;
		}

		pStimPipelineSource->bVideo = false;
		pStimPipelineSource->bAudio = true;
		pStimPipelineSource->bWaitingForPreroll = false;
		pStimPipelineSource->bPrerolled = false;
		pStimPipelineSource->bLoop = info.isLoopPlayBack();
		pStimPipelineSource->volume = (float)info.getVolume()/100.0;

	}
	else
	{
		// solid color background are completely specified here - no callbacks needed. BinData needed to hold videoSink, however.
		if (info.isColor() || info.isBackground())
		{
			GstElement *src, *sink, *convert;

			/* Create the elements */
			src = makeElement("videotestsrc", ppt, id());
			Q_ASSERT(src);

			g_object_set(G_OBJECT(src), "pattern", GST_VIDEO_TEST_SRC_SOLID, "foreground-color", info.getColor().rgba(), NULL);

			sink = makeElement("qwidget5videosink", ppt, id());
			Q_ASSERT(sink);
			convert = makeElement("videoconvert", ppt, id());
			Q_ASSERT(convert);

			/* Build the pipeline */
			gst_bin_add_many (GST_BIN(m_pipeline), src, convert, sink, NULL);
			if (gst_element_link(src, convert) != TRUE)
			{
				qCritical() << "src-convert could not be linked.";
				gst_object_unref (m_pipeline);
				delete pStimPipelineSource;
				return NULL;
			}
			if (gst_element_link(convert, sink) != TRUE)
			{
				qCritical() << "convert-sink could not be linked.";
				gst_object_unref (m_pipeline);
				delete pStimPipelineSource;
				return NULL;
			}

			pStimPipelineSource->bVideo = true;
			pStimPipelineSource->bAudio = false;
			pStimPipelineSource->bWaitingForPreroll = false;
			pStimPipelineSource->bPrerolled = false;
			pStimPipelineSource->bLoop = false;
			pStimPipelineSource->volume = 0;

		}
		else
		{
			// all other stim types -- image or movie -- handled here. The determination
			// of whether its an image or movie is done when caps are handled in padAdded.
			// Same is true of audio handling.

			GstElement *src, *decodebin, *sink;

			/* Create the elements */
			src = makeElement("filesrc", ppt, id());
			Q_ASSERT(src);
			g_object_set(G_OBJECT(src), "location", C_STR(info.getAbsoluteFileName(m_dirStimRoot)), NULL);

			decodebin = makeElement("decodebin", ppt, id());
			Q_ASSERT(decodebin);
			g_signal_connect(decodebin, "pad-added", G_CALLBACK(&padAdded), pStimPipelineSource);

			sink = makeElement("qwidget5videosink", ppt, id());
			Q_ASSERT(sink);

			/*
			 * Build the partial pipeline
			 * Note "sink" is left unconnected in the pipeline.
			 * padAdded method will link it. Must be created here or threading-related-errors occur when trying to
			 * connect widget to sink. Name of the sink can be deduced from the name of the decodebin element,
			 * e.g. 'decodebin-Center-4' -> 'qwidget5videosink-Center-4'
			 */

			gst_bin_add_many (GST_BIN(m_pipeline), src, decodebin, sink, NULL);
			if (gst_element_link(src, decodebin) != TRUE)
			{
				qCritical() << "src-decodebin could not be linked.";
				gst_object_unref (m_pipeline);
				delete pStimPipelineSource;
				return NULL;
			}

			pStimPipelineSource->bVideo = true;
			pStimPipelineSource->bAudio = bAudio;
			pStimPipelineSource->bWaitingForPreroll = false;
			pStimPipelineSource->bPrerolled = false;
			pStimPipelineSource->bLoop = info.isLoopPlayBack();
			pStimPipelineSource->volume = (float)info.getVolume()/100.0;

		}
	}

	return pStimPipelineSource;
}


void HStimPipeline::padAdded(GstElement *src, GstPad *newPad, gpointer p)
{
	GstElement *audioMixer=NULL, *audioSink=NULL;
	HStimPipelineSource *pSource = (HStimPipelineSource *)p;
	QString factory, prefix;
	const HPlayerPositionType *pppt=&HPlayerPositionType::UnknownPlayerPositionType;
	int id=0;

	QMutexLocker(pSource->stimPipeline()->mutex());

	qDebug() << "padAdded: Got new pad " << GST_PAD_NAME(newPad) << " from " << GST_ELEMENT_NAME(src) << " accept video? " << pSource->bVideo << " accept audio? " << pSource->bAudio;
	bool b = parseElementName(QString(GST_ELEMENT_NAME(src)), factory, pppt, id, prefix);
	Q_ASSERT(b);
	qDebug() << "padAdded: parsed element name, factory " << factory << " ppt " << pppt->name() << " id " << id;

	// get the caps and parse them. That will tell us whether its audio or video. If its video, then
	// we get the frame rate - that tells us whether its an image (rate=0) or video. We also get the
	// resolution.

	GstCaps *new_pad_caps = NULL;
	new_pad_caps = gst_pad_get_current_caps(newPad);
	bool isVideo=false, isImage=false, isAudio=false;
	int width=-1, height=-1;
	parseCaps(new_pad_caps, isVideo, isImage, width, height, isAudio);
	pSource->size = QSize(width, height);
	gst_caps_unref(new_pad_caps);

	if (isImage)
	{
		qDebug() << "padAdded - got image stream";
		if (!pSource->bVideo)
		{
			qDebug() << "padAdded - ignoring video for this source";
		}
		else
		{
			qDebug() << "padAdded - link image into pipeline";
			GstElement *videoConvert = makeElement("videoconvert", *pppt, id);
			Q_ASSERT(videoConvert);

			GstElement *freeze = makeElement("imagefreeze", *pppt, id);
			Q_ASSERT(freeze);

			gst_bin_add_many(GST_BIN(pSource->pipeline()), videoConvert, freeze, NULL);

			GstPad *sinkPad = gst_element_get_static_pad(videoConvert, "sink");
			gst_pad_add_probe (newPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeDoNothingCB, (gpointer)pSource, NULL);
			//gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeDoNothingCB, (gpointer)pSource, NULL);
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);
			gst_element_link (videoConvert, freeze);

			GstElement *sink = gst_bin_get_by_name(GST_BIN(pSource->pipeline()), C_STR(makeElementName("qwidget5videosink", *pppt, id)));
			Q_ASSERT(sink);
			gst_element_link (freeze, sink);

			// set states of all newly-added elements.
			if (!gst_element_sync_state_with_parent(videoConvert))
				qCritical() << "Cannot sync videoConvert with pipeline state";
			if (!gst_element_sync_state_with_parent(freeze))
				qCritical() << "Cannot sync freeze with pipeline state";
			if (!gst_element_sync_state_with_parent(sink))
				qCritical() << "Cannot sync sink with pipeline state";

			qDebug() << "padAdded: image - " << GST_ELEMENT_NAME(src) << " - done.";
		}
	}
	else if (isVideo)
	{
		qDebug() << "padAdded - got video stream";
		if (!pSource->bVideo)
		{
			qDebug() << "padAdded - ignoring video for this source";
		}
		else
		{
			qDebug() << "padAdded - link video into pipeline. Loop? " << pSource->bLoop;
			GstElement *videoConvert = makeElement("videoconvert", *pppt, id);
			gst_bin_add_many(GST_BIN(pSource->pipeline()), videoConvert, NULL);

			GstPad *sinkPad = gst_element_get_static_pad(videoConvert, "sink");
			gst_pad_add_probe (newPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeDoNothingCB, (gpointer)pSource, NULL);
			//gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeDoNothingCB, (gpointer)pSource, NULL);
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);

			GstElement *sink = gst_bin_get_by_name(GST_BIN(pSource->pipeline()), C_STR(makeElementName("qwidget5videosink", *pppt, id)));
			Q_ASSERT(sink);
			gst_element_link (videoConvert, sink);

			// set states of all newly-added elements.
			if (!gst_element_sync_state_with_parent(videoConvert))
				qCritical() << "Cannot sync videoConvert with pipeline state";
			if (!gst_element_sync_state_with_parent(sink))
				qCritical() << "Cannot sync sink with pipeline state";

			gst_object_unref(sink);

			//gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &FDDialog::eventProbeDoNothingCB, (gpointer)pdata, NULL);

		}
		qDebug() << "padAdded: video - " << GST_ELEMENT_NAME(src) << " - done.";
	}
	else if (isAudio)
	{
		if (!pSource->bAudio)
		{
			qDebug() << "padAdded - ignoring audio for this source";
			return;
		}
		else if (pSource->volume == 0)
		{
			qDebug() << "padAdded - audio is muted for this source";
			return;
		}
		else
		{
			qDebug() << "padAdded - link audio into pipeline. Loop? " << pSource->bLoop;

			// check for existing audio sink
			// If not found, then create it and the audiomixer->audioconvert
			// After this block is done, we will have a reference to the audiomixer which will be linked
			// to the end of the audio elements created below. We MUST RELEASE the reference!
			audioMixer = gst_bin_get_by_name(GST_BIN(pSource->pipeline()), C_STR(makeElementName("audiomixer", HPlayerPositionType::Control, id)));
			if (!audioMixer)
			{
				GstElement *audioSink = makeElement("osxaudiosink", HPlayerPositionType::Control, id);
				Q_ASSERT(audioSink);

				g_object_set(G_OBJECT(audioSink), "provide-clock", false, NULL);

				audioMixer = makeElement("audiomixer", HPlayerPositionType::Control, id);
				Q_ASSERT(audioMixer);
				gst_bin_add_many(GST_BIN(pSource->pipeline()), audioMixer, audioSink, NULL);
				gst_element_link(audioMixer, audioSink);
			}
			else
				gst_object_unref(audioMixer);


			// create elements for audio stream
			GstElement *audioresample = makeElement("audioresample", *pppt, id);
			Q_ASSERT(audioresample);
			GstElement *audioconvert = makeElement("audioconvert", *pppt, id);
			Q_ASSERT(audioconvert);
			GstElement *volume = makeElement("volume", *pppt, id);
			Q_ASSERT(volume);
			qDebug() << "padAdded - audio volume level [0.0-1.0]: " << pSource->volume;
			g_object_set(G_OBJECT(volume), "volume", pSource->volume, NULL);

			gst_bin_add_many(GST_BIN(pSource->pipeline()), audioresample, audioconvert, volume, NULL);

			GstPad *pad = gst_element_get_static_pad(audioconvert, "sink");
			Q_ASSERT(pad);
			gst_pad_add_probe (newPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeDoNothingCB, (gpointer)pSource, NULL);
			//gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeDoNothingCB, (gpointer)pSource, NULL);
			//gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &FDDialog::eventProbeAudioDoNothingCB, (gpointer)pdata, NULL);
			gst_pad_link(newPad, pad);
			gst_object_unref(pad);

			gst_element_link(audioconvert, audioresample);
			gst_element_link(audioresample, volume);

			audioMixer = gst_bin_get_by_name(GST_BIN(pSource->pipeline()), C_STR(makeElementName("audiomixer", HPlayerPositionType::Control, id)));
			Q_ASSERT(audioMixer);
			gst_element_link(volume, audioMixer);	// audioMixer already linked with audioSink
			if (!gst_element_sync_state_with_parent(audioMixer))
				qCritical() << "Cannot sync audioMixer with pipeline state";

			audioSink = gst_bin_get_by_name(GST_BIN(pSource->pipeline()), C_STR(makeElementName("osxaudiosink", HPlayerPositionType::Control, id)));
			Q_ASSERT(audioSink);
			if (!gst_element_sync_state_with_parent(audioSink))
				qCritical() << "Cannot sync audioSink with pipeline state";

			if (!gst_element_sync_state_with_parent(audioresample))
				qCritical() << "Cannot sync audioresample with pipeline state";
			if (!gst_element_sync_state_with_parent(audioconvert))
				qCritical() << "Cannot sync audioconvert with pipeline state";
			if (!gst_element_sync_state_with_parent(volume))
				qCritical() << "Cannot sync volume with pipeline state";

			gst_object_unref(audioMixer);
			gst_object_unref(audioSink);
		}
	}

}

gboolean HStimPipeline::busCallback(GstBus *, GstMessage *msg, gpointer p)
{

	HStimPipeline *pStimPipeline = (HStimPipeline *)p;

	//qDebug() << "busCallback: " << GST_MESSAGE_TYPE_NAME(msg) << " from " << GST_MESSAGE_SRC_NAME(msg);
	if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ASYNC_DONE)
	{
		qDebug() << "busCallback: Got ASYNC_DONE from " << GST_MESSAGE_SRC_NAME(msg);

		// Loop over each stim source on this bus.
		// The ASYNC_DONE message doesn't come until all sources have prerolled, or entered PAUSED state.
		// If any of the sources are looping sources, and if they are not already prerolled, then we
		// must issue a flushing seek here on that portion of the pipeline.
		QMutexLocker locker(pStimPipeline->mutex());
#if PREROLL_SOME_SOURCES
		QMapIterator<HPlayerPositionType, HStimPipelineSource* >it(pStimPipeline->getPipelineSourceMap());
		while (it.hasNext())
		{
			it.next();
			HPlayerPositionType ppt = it.key();
			HStimPipelineSource* pSource = it.value();
			qDebug() << "busCallback: Source " << ppt.name();
			if (pSource->bPrerolled)
			{
				qDebug() << "busCallback: Source " << ppt.name() << " already prerolled.";
			}
			else
			{

				if (!pSource->bLoop)
				{
					// If not looping, then this is the initial preroll.
					qDebug() << "busCallback: Source " << ppt.name() << " initial preroll done - non-looping source.";
					pSource->bPrerolled = true;
				}
				else
				{
					if (pSource->bWaitingForPreroll)
					{
						// We should not get here.
						qCritical() << "WHAT??? ASYNC_DONE while waiting for preroll!?! Should be handled in event probe.";
						//pSource->bWaitingForPreroll = false;
						//pSource->bPrerolled = true;
						qDebug() << "busCallback: Source " << ppt.name() << " flushing seek done for looping source. Prerolled.";
					}
					else
					{
						// flushing seek on decodebin element.
						qDebug() << "busCallback: Source " << ppt.name() << " will loop, issue flushing seek to start segment looping.";
						GstElement *decodebin = gst_bin_get_by_name(GST_BIN(pStimPipeline->pipeline()), C_STR(makeElementName("decodebin", ppt, pStimPipeline->id())));
						Q_ASSERT(decodebin);
						if (!gst_element_seek(decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
						{
							qCritical() << "busCallback: SEEK FAILED";
						}
						else
						{
							qDebug() << "busCallback: flush seek done";
							pSource->bWaitingForPreroll = true;
						}
						gst_object_unref(decodebin);
					}
				}
			}
		}
#else
		// issue flushing seek to entire pipeline. Catch segment events for each source
		if (!pStimPipeline->bInitialFlushingSeekDone)
		{
			qDebug() << "busCallback: Issue initial flushing seek to pipeline.";
			if (!gst_element_seek(pStimPipeline->pipeline(), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << "busCallback: SEEK FAILED";
			}
			else
			{
				qDebug() << "busCallback: initial pipeline flush seek done";
				pStimPipeline->bInitialFlushingSeekDone = true;
				QMapIterator<HPlayerPositionType, HStimPipelineSource* >it(pStimPipeline->getPipelineSourceMap());
				while (it.hasNext())
				{
					it.next();
					HPlayerPositionType ppt = it.key();
					HStimPipelineSource* pSource = it.value();
					qDebug() << "busCallback: Source " << ppt.name() << " is now waiting for preroll...";
					pSource->bWaitingForPreroll = true;
				}
			}
		}
		else
		{
			qDebug() << "busCallback: entire pipeline is now prerolled.";
		}
#endif
	}
	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_STATE_CHANGED)
	{
		if (GST_ELEMENT(msg->src) == pStimPipeline->pipeline())
		{
			GstState old_state, new_state;
			gst_message_parse_state_changed(msg, &old_state, &new_state, NULL);
			qDebug() << "busCallback: got STATE_CHANGED " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(new_state) << " from " << GST_MESSAGE_SRC_NAME(msg);
			if (old_state == GST_STATE_PAUSED && new_state == GST_STATE_PLAYING)
			{
				pStimPipeline->emitNowPlaying();
			}
		}
	}
	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_SEGMENT_DONE)
	{
		qDebug() << "busCallback: SEGMENT_DONE  Running time is " << (gst_clock_get_time(gst_element_get_clock(pStimPipeline->pipeline())) - gst_element_get_base_time(pStimPipeline->pipeline()));

/*
		QMutexLocker(&pdata->mutex);
		qDebug() << "busCallback: SEGMENT DONE from "<< GST_MESSAGE_SRC_NAME(msg);
		GstElement *decodebin = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "decodebin");
		Q_ASSERT(decodebin);
		if (!gst_element_seek(decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
		{
			qCritical() << "busCallback: SEEK FAILED";
		}
		else
			qCritical() << "busCallback: SEEK OK";
		gst_object_unref(decodebin);
		*/
	}
#if !NON_FLUSHING_SEEK_IN_EVENT_PROBE
	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_APPLICATION)
	{
		if (gst_message_has_name (msg, "DoSegmentSeek"))
		{
			const gchar *elementName = gst_structure_get_string(gst_message_get_structure(msg), "element");
			qDebug() << "busCallback: DoSegmentSeek on element  " << (elementName ? elementName : "???");
			GstElement *element = gst_bin_get_by_name(GST_BIN(pStimPipeline->pipeline()), elementName);
			Q_ASSERT(element);
			if (!gst_element_seek(element, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_SEGMENT, GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << "busCallback: non-flushing segment seek on element " << elementName << " FAILED";
			}
			else
			{
				qDebug() << "busCallback: non-flushing segment seek on element " << elementName << " SUCCESS";
			}
			gst_object_unref(element);
		}
	}
#endif
	return TRUE;
}

GstPadProbeReturn HStimPipeline::eventProbeDoNothingCB(GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	HStimPipelineSource *pSource = (HStimPipelineSource *)p;

	// Note: GST_PAD_PROBE_INFO_EVENT(d) returns a GstEvent* or NULL if info->data does not have an event.
	// In this routine, because its an event probe, it should always contain an event?

	GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
	if (event)
	{
		GstElement* parent = GST_PAD_PARENT(pad);
		//qDebug() << "eventProbeDoNothingCB: Event type " << GST_EVENT_TYPE_NAME(event) << " from " << GST_ELEMENT_NAME(parent);

		if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT_DONE)
		{
			QMutexLocker(pSource->stimPipeline()->mutex());
			if (pSource->bLoop)
			{
				qDebug() << "eventProbeDoNothingCB: Looping source, got segment-done event on pad " << GST_PAD_NAME(pad) << ". Running time is " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
				if (pSource->bWaitingForSegment)
				{
					qDebug() << "eventProbeDoNothingCB: Looping source, waiting for segment. No seek issued.";
				}
				else
				{
//					qDebug() << "NOT DOING SEGMENT SEEK FOR LOOPING!";
					// post message on bus
				    qDebug() << "eventProbeDoNothingCB: Looping source, post bus message.";
					GstBus *	bus = gst_pipeline_get_bus(GST_PIPELINE(pSource->stimPipeline()->pipeline()));
					GstStructure *structure = gst_structure_new("DoSegmentSeek", "element", G_TYPE_STRING, GST_ELEMENT_NAME(parent), NULL);
				    gst_bus_post (bus, gst_message_new_application (
				          GST_OBJECT_CAST (pSource->stimPipeline()->pipeline()),
				          structure));
				    gst_object_unref(bus);
#if NON_FLUSHING_SEEK_IN_EVENT_PROBE
				    qDebug() << "eventProbeDoNothingCB: Looping source, issue non-flushing segment seek.";
					if (!gst_element_seek(parent, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_SEGMENT, GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
					{
						qCritical() << "eventProbeDoNothingCB: non-flushing segment seek FAILED";
					}
					else
					{
						qDebug() << "eventProbeDoNothingCB: non-flushing segment seek done";
						pSource->bWaitingForSegment = true;
					}
#else
					pSource->bWaitingForSegment = true;
#endif
				}
			}
		}
		else if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT)
		{
			QMutexLocker(pSource->stimPipeline()->mutex());
			if (pSource->bLoop)
			{
				const GstSegment *segment;
				gst_event_parse_segment(event, &segment);
				qDebug() << "eventProbeDoNothingCB: Looping source, got segment event on pad "  << GST_PAD_NAME(pad) << ". Running time is " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
				qDebug() << "eventProbeDoNothingCB: base " << segment->base << " offset " << segment->offset << " start " << segment->start << " stop " << segment->stop << " time " << segment->time << " position " << segment->position << " duration " << segment->duration;
				if (pSource->bWaitingForPreroll)
				{
					qDebug() << "eventProbeDoNothingCB: Waiting for looping source to preroll after initial flushing seek, expected segment. This source is prerolled and ready to loop.";
					pSource->bWaitingForPreroll = false;
					pSource->bPrerolled = true;
				}
				else if (pSource->bWaitingForSegment)
				{
					qDebug() << "eventProbeDoNothingCB: Looping source, expected segment. Cool.";
					pSource->bWaitingForSegment = false;
				}
				else
				{
					qDebug() << "eventProbeDoNothingCB: Looping source, unexpected segment.";
				}
			}
		}
	}
	return GST_PAD_PROBE_OK;
}

void HStimPipeline::dump()
{
	if (!m_bInitialized)
	{
		qCritical() << "Cannot dump - initialize() first!";
		return;
	}

	qDebug() << "Dump dot file base name " << GST_ELEMENT_NAME(pipeline());
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline()), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME(pipeline()));

}

HPipeline* HStimPipelineFactory(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool, bool bISS, bool bStatic, QObject *parent)
{
	HPipeline *p;
	if (bStatic)
		p = new HStaticStimPipeline(id, stimulusSettings, stimRoot, layoutType, bISS, parent);
	else
		p = new HStimPipeline(id, stimulusSettings, stimRoot, layoutType, bISS, parent);
	return p;
}
