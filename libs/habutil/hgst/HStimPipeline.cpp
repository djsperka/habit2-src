/*
 * HStimPipeline.cpp
 *
 *  Created on: Dec 14, 2017
 *      Author: dan
 */


// About sound....
//
// if iss==false it means that sound can only come from stim source files. Can have two sources l/r, will be mixed. Can have only one, or none.
// if iss==true it means that sound can ONLY come from a dedicated iss sound file. Video sources l/r/c that have sound are ok, but sound from those sources is ignored.

#include "HVideoWidget.h"
#include <QMutexLocker>
#include <hgst/HStimPipeline.h>
#include <hgst/HStaticStimPipeline.h>

HStimPipelineSource::HStimPipelineSource(HStimPipeline *pipe)
: m_pStimPipeline(pipe)
, bWaitingForPreroll(false)
, bPrerolled(false)
, bWaitingForSegment(false)
, bWaitingForSegment2(false)
, sWaitingForSegment2Pad("")
, bAudio(false)
, bVideo(false)
, bLoop(false)
, volume(0)
, nPadsLinked(0)
{
}

HStimPipelineSource::~HStimPipelineSource()
{

}

HStimPipeline::HStimPipeline(int id, const Habit::StimulusSettings& ss, const Habit::StimulusDisplayInfo& info, const QDir& stimRoot, QObject *parent)
: HPipeline(id, ss, info, parent)
, m_bInitialized(false)
, m_dirStimRoot(stimRoot)
, m_bRewindPending(false)
, m_pipeline(NULL)
, bInitialFlushingSeekDone(false)
, m_iAsyncPause(0)
{
	m_bUsingMixer = !info.getUseISS() &&
			(
					info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight ||
					info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple
			);
}

HStimPipeline::~HStimPipeline()
{
}

GstElement *HStimPipelineSource::pipeline()
{
	return stimPipeline()->pipeline();
}

void HStimPipeline::emitNowPlaying()
{
	Q_EMIT nowPlaying();
}

void HStimPipeline::emitPrerolling()
{
	Q_EMIT prerolling(this->id());
}

void HStimPipeline::emitPrerolled()
{
	Q_EMIT prerolled(this->id());
}

void HStimPipeline::rewind()
{
	// flushing seek. gsgtreamer will handle state, as long as we're paused or playing, which
	// we always are in habit, this will flush the pipeline and preroll, and original state is resumed.
	if (!gst_element_seek(pipeline(), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
	{
		qCritical() << "SEEK FAILED";
	}
	else
	{
		qDebug() << "HStimPipeline::rewind(), seek issued, waiting for async_done";
		m_bRewindPending = true;
	}
}

void HStimPipeline::dumpGstStateChangeReturn(const QString& s, GstStateChangeReturn r)
{
	switch(r)
	{
	case GST_STATE_CHANGE_FAILURE:
		qDebug() << s << "( " << id() << " ) GST_STATE_CHANGE_FAILURE";
		break;
	case GST_STATE_CHANGE_SUCCESS:
		qDebug() << s << "( " << id() << " ) GST_STATE_CHANGE_SUCCESS";
		break;
	case GST_STATE_CHANGE_ASYNC:
		qDebug() << s << "( " << id() << " ) GST_STATE_CHANGE_ASYNC";
		break;
	case GST_STATE_CHANGE_NO_PREROLL:
		qDebug() << s << "( " << id() << " ) GST_STATE_CHANGE_NO_PREROLL";
		break;
	default:
		qDebug() << s << "( " << id() << " )  unknown return value from state change";
		break;
	}
}

void HStimPipeline::pause()
{
	initialize();
	GstStateChangeReturn r = gst_element_set_state(pipeline(), GST_STATE_PAUSED);
	if (GST_STATE_CHANGE_ASYNC == r)
	{
		m_iAsyncPause++;
	}
}

void HStimPipeline::play()
{
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

void HStimPipeline::attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1, HVideoWidget *w2)
{
	if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		setWidgetPropertyOnSink(w0, HPlayerPositionType::Center);
		if (w0)
		{
			w0->setStimulusSize(m_mapPipelineSources[HPlayerPositionType::Center]->size);
			qDebug() << "Setting stimulus size " << m_mapPipelineSources[HPlayerPositionType::Center]->size
					<< " prerolled? " << m_mapPipelineSources[HPlayerPositionType::Center]->bPrerolled;
		}
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
	else if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		setWidgetPropertyOnSink(w0, HPlayerPositionType::Left);
		if (w0)
			w0->setStimulusSize(m_mapPipelineSources[HPlayerPositionType::Left]->size);
		setWidgetPropertyOnSink(w1, HPlayerPositionType::Center);
		if (w1)
			w1->setStimulusSize(m_mapPipelineSources[HPlayerPositionType::Center]->size);
		setWidgetPropertyOnSink(w2, HPlayerPositionType::Right);
		if (w2)
			w2->setStimulusSize(m_mapPipelineSources[HPlayerPositionType::Right]->size);
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
		m_bRewindPending = false;
		bInitialFlushingSeekDone = false;
		m_iAsyncPause = 0;
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

	// Create HStimPipelineSource objects for each position Left/Right/Center/Sound as needed.
	// Save the objects in m_mapPipelineSources, with the HPlayerPositionType as the key.
	if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		m_mapPipelineSources.insert(HPlayerPositionType::Center, addStimulusInfo(HPlayerPositionType::Center, stimulusSettings().getCenterStimulusInfo(), !iss()));
	}
	else if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		m_mapPipelineSources.insert(HPlayerPositionType::Left, addStimulusInfo(HPlayerPositionType::Left, stimulusSettings().getLeftStimulusInfo(), !iss()));
		m_mapPipelineSources.insert(HPlayerPositionType::Right, addStimulusInfo(HPlayerPositionType::Right, stimulusSettings().getRightStimulusInfo(), !iss()));
	}
	else if (stimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		m_mapPipelineSources.insert(HPlayerPositionType::Left, addStimulusInfo(HPlayerPositionType::Left, stimulusSettings().getLeftStimulusInfo(), !iss()));
		m_mapPipelineSources.insert(HPlayerPositionType::Center, addStimulusInfo(HPlayerPositionType::Center, stimulusSettings().getCenterStimulusInfo(), !iss()));
		m_mapPipelineSources.insert(HPlayerPositionType::Right, addStimulusInfo(HPlayerPositionType::Right, stimulusSettings().getRightStimulusInfo(), !iss()));
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

	qDebug() << "HStimPipeline::addStimulusInfo ppt " << ppt.name() << " info " << info;

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
		 * Build the partial pipeline: filesrc ! decodebin
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

#if 0
			src = makeElement("videotestsrc", ppt, id());
			Q_ASSERT(src);

			g_object_set(G_OBJECT(src), "pattern", GST_VIDEO_TEST_SRC_SOLID, "foreground-color", info.getColor().rgba(), NULL);
#else
			GError *gerror = NULL;
			QString s = QString("videotestsrc pattern=solid-color foreground-color=%1").arg(info.getColor().rgba());
			src = gst_parse_bin_from_description(s.toStdString().c_str(), true, &gerror);
			if (src == NULL || gerror != NULL)
			{
				qCritical() << "cannot create videotestsrc!";
				return NULL;
			}
			Q_ASSERT(src);
#endif
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
	GstElement *audioMixer=NULL;
	HStimPipelineSource *pSource = (HStimPipelineSource *)p;
	QString factory, prefix;
	const HPlayerPositionType *pppt=&HPlayerPositionType::UnknownPlayerPositionType;
	int id=0;
	QString sDebugPrefix;
	sDebugPrefix = QString("padAdded(%1): ").arg(GST_ELEMENT_NAME(src));

	// lock the entire HStimPipeline object
	QMutexLocker locker(pSource->stimPipeline()->mutex());

	qDebug() << sDebugPrefix << "got pad " << GST_PAD_NAME(newPad) << " accept video? " << pSource->bVideo << " accept audio? " << pSource->bAudio;
	bool b = parseElementName(QString(GST_ELEMENT_NAME(src)), factory, pppt, id, prefix);
	Q_ASSERT(b);
	qDebug() << sDebugPrefix << "parsed element name, factory " << factory << " ppt " << pppt->name() << " id " << id << " prefix " << prefix;

	// get the caps and parse them. That will tell us whether its audio or video. If its video, then
	// we get the frame rate - that tells us whether its an image (rate=0) or video. We also get the
	// resolution.

	bool isVideo=false, isImage=false, isAudio=false;
	int width=-1, height=-1;
	GstCaps *new_pad_caps = gst_pad_get_current_caps(newPad);
	parseCaps(new_pad_caps, isVideo, isImage, width, height, isAudio);
	gst_caps_unref(new_pad_caps);

	// set size of source, but only if this is an image or video stream. Audio streams have no size.
	// Container that has video and audio - this prevents us from wiping out the size if audio pad appears second.
	if (isImage || isVideo) pSource->size = QSize(width, height);

	if (isImage)
	{
		qDebug() << sDebugPrefix << "got image stream";
		if (!pSource->bVideo)
		{
			qDebug() << sDebugPrefix << "ignoring video for this source";
		}
		else
		{

			// pipeline here will be
			// --> queue --> videoconvert -->imagefreeze --> sink

			qDebug() << sDebugPrefix << "link image into pipeline";

			GstElement *queue = makeElement("queue", *pppt, id);
			Q_ASSERT(queue);

			GstElement *videoConvert = makeElement("videoconvert", *pppt, id);
			Q_ASSERT(videoConvert);

			GstElement *freeze = makeElement("imagefreeze", *pppt, id);
			Q_ASSERT(freeze);

			gst_bin_add_many(GST_BIN(pSource->pipeline()), queue, videoConvert, freeze, NULL);

			GstPad *sinkPad = gst_element_get_static_pad(queue, "sink");
			gst_pad_add_probe (newPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeCB, (gpointer)pSource, NULL);
			//gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeCB, (gpointer)pSource, NULL);
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);
			gst_element_link(queue, videoConvert);
			gst_element_link(videoConvert, freeze);

			GstElement *sink = gst_bin_get_by_name(GST_BIN(pSource->pipeline()), C_STR(makeElementName("qwidget5videosink", *pppt, id)));
			Q_ASSERT(sink);
			gst_element_link (freeze, sink);

			// set states of all newly-added elements.
			if (!gst_element_sync_state_with_parent(queue))
				qCritical() << "Cannot queue freeze with pipeline state";
			if (!gst_element_sync_state_with_parent(videoConvert))
				qCritical() << "Cannot sync videoConvert with pipeline state";
			if (!gst_element_sync_state_with_parent(freeze))
				qCritical() << "Cannot sync freeze with pipeline state";
			if (!gst_element_sync_state_with_parent(sink))
				qCritical() << "Cannot sync sink with pipeline state";

			qDebug() << sDebugPrefix << "image - " << GST_ELEMENT_NAME(src) << " - done.";
			pSource->nPadsLinked++;
		}
	}
	else if (isVideo)
	{
		qDebug() << sDebugPrefix << "got video stream";
		if (!pSource->bVideo)
		{
			qDebug() << sDebugPrefix << "ignoring video for this source";
		}
		else
		{
			qDebug() << sDebugPrefix << "link video into pipeline. Loop? " << pSource->bLoop;

			GstElement *queue = makeElement("queue", *pppt, id, "v-");
			Q_ASSERT(queue);

			GstElement *videoConvert = makeElement("videoconvert", *pppt, id);
			gst_bin_add_many(GST_BIN(pSource->pipeline()), queue, videoConvert, NULL);

			GstPad *sinkPad = gst_element_get_static_pad(queue, "sink");
			gst_pad_add_probe (newPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeCB, (gpointer)pSource, NULL);
			//gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeCB, (gpointer)pSource, NULL);
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);

			GstElement *sink = gst_bin_get_by_name(GST_BIN(pSource->pipeline()), C_STR(makeElementName("qwidget5videosink", *pppt, id)));
			Q_ASSERT(sink);
			gst_element_link(queue, videoConvert);
			gst_element_link (videoConvert, sink);

			// set states of all newly-added elements.
			if (!gst_element_sync_state_with_parent(queue))
				qCritical() << sDebugPrefix << "Cannot sync queue with pipeline state";
			if (!gst_element_sync_state_with_parent(videoConvert))
				qCritical() << sDebugPrefix << "Cannot sync videoConvert with pipeline state";
			if (!gst_element_sync_state_with_parent(sink))
				qCritical() << sDebugPrefix << "Cannot sync sink with pipeline state";

			gst_object_unref(sink);

			//gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &FDDialog::eventProbeDoNothingCB, (gpointer)pdata, NULL);

			pSource->nPadsLinked++;

		}
		qDebug() << sDebugPrefix << "video - " << GST_ELEMENT_NAME(src) << " - done.";
		GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pSource->pipeline()), GST_DEBUG_GRAPH_SHOW_ALL, "video");

	}
	else if (isAudio)
	{
		if (!pSource->bAudio)
		{
			qDebug() << sDebugPrefix << "ignoring audio for this source";
			return;
		}
		else if (pSource->volume == 0)
		{
			qDebug() << sDebugPrefix << "audio is muted for this source";
			return;
		}
		else
		{

			// When linking audio, put a prefix ("source=Left", use pppt->name()) on all element names,
			// except the mixer and downstream. So its really just the resample, convert, volume
			// to denote that the source is Center/Left/Right
			// If dual screen and both stim have audio, will get name clashes otherwise.

			qDebug() << sDebugPrefix << "link audio into pipeline. Loop? " << pSource->bLoop;


			// Build initial part of pipeline - audioconvert ! audioresample ! volume(*) ! queue
			// the received pad will connect to audioconvert
			// src pad on queue goes to _either_ audiosink
			//                          _or_ audiomixer ! audiosink (when multiple audio sources are possible)
			//

			// create elements for audio stream
			GstElement *audioconvert = makeElement("audioconvert", *pppt, id);
			Q_ASSERT(audioconvert);
			GstElement *audioresample = makeElement("audioresample", *pppt, id);
			Q_ASSERT(audioresample);
			GstElement *volume = makeElement("volume", *pppt, id);
			Q_ASSERT(volume);
			g_object_set(G_OBJECT(volume), "volume", pSource->volume, NULL);
			GstElement *queue = makeElement("queue", *pppt, id, "a-");
			Q_ASSERT(queue);

			// add them to pipeline
			gst_bin_add_many(GST_BIN(pSource->pipeline()), audioresample, audioconvert, volume, queue, NULL);

			// connect the pad we received with the audioconvert element
			GstPad *audioconvertPad = gst_element_get_static_pad(audioconvert, "sink");
			Q_ASSERT(audioconvertPad);
			gst_pad_add_probe (newPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HStimPipeline::eventProbeCB, (gpointer)pSource, NULL);
			gst_pad_link(newPad, audioconvertPad);
			gst_object_unref(audioconvertPad);

			gst_element_link(audioconvert, audioresample);
			gst_element_link(audioresample, volume);
			gst_element_link(volume, queue);
			if (!gst_element_sync_state_with_parent(audioconvert))
				qCritical() << sDebugPrefix << "Cannot sync audioconvert with pipeline state";
			if (!gst_element_sync_state_with_parent(audioresample))
				qCritical() << sDebugPrefix << "Cannot sync audioresample with pipeline state";
			if (!gst_element_sync_state_with_parent(volume))
				qCritical() << sDebugPrefix << "Cannot sync volume with pipeline state";
			if (!gst_element_sync_state_with_parent(queue))
				qCritical() << sDebugPrefix << "Cannot sync queue with pipeline state";

			// now the volume element will connect directly to an audiosink, or it'll connect to
			// an audiomixer. If an audio mixer is to be used, it may need to be created here.
			if (pSource->stimPipeline()->getUsingMixer())
			{
				// check for existing audio sink
				// If not found, then create it and the audiomixer->audiosink
				// After this block is done, we will have a reference to the audiomixer which will be linked
				// to the end of the audio elements created below. We MUST RELEASE the reference!

				QString sAudioMixerName = makeElementName("audiomixer", HPlayerPositionType::Control, id);
				audioMixer = gst_bin_get_by_name(GST_BIN(pSource->pipeline()), C_STR(makeElementName("audiomixer", HPlayerPositionType::Control, id)));
				if (!audioMixer)
				{
					GstElement *audioSink = NULL;

#if defined(Q_OS_MAC)
					qDebug() << "make osxaudiosink for mac";
					audioSink = makeElement("osxaudiosink", HPlayerPositionType::Control, id);
#elif defined(Q_OS_WIN)
					qDebug() << "make directsoundsink for win";
					audioSink = makeElement("directsoundsink", HPlayerPositionType::Control, id);
#else
					qDebug() << "Unsupported OS.";
#endif

					Q_ASSERT(audioSink);

					//g_object_set(G_OBJECT(audioSink), "provide-clock", false, NULL);
					//g_object_set(G_OBJECT(audioSink), "provide-clock", true, NULL);

					audioMixer = makeElement("audiomixer", HPlayerPositionType::Control, id);
					Q_ASSERT(audioMixer);
					gst_bin_add_many(GST_BIN(pSource->pipeline()), audioMixer, audioSink, NULL);

					// connect queue ! audioMixer ! audioSink
					gst_element_link(queue, audioMixer);
					gst_element_link(audioMixer, audioSink);

					if (!gst_element_sync_state_with_parent(audioMixer))
						qCritical() << sDebugPrefix << "Cannot sync audioMixer with pipeline state";
					if (!gst_element_sync_state_with_parent(audioSink))
						qCritical() << sDebugPrefix << "Cannot sync audioSink with pipeline state";
				}
				else
				{
					// connect queue element to mixer
					gst_element_link(queue, audioMixer);
					gst_object_unref(audioMixer);
				}
			}
			else
			{
				GstElement *audioSink = NULL;
#if defined(Q_OS_MAC)
				qDebug() << "make osxaudiosink for mac";
				audioSink = makeElement("osxaudiosink", HPlayerPositionType::Control, id);
#elif defined(Q_OS_WIN)
				qDebug() << "make directsoundsink for win";
				audioSink = makeElement("directsoundsink", HPlayerPositionType::Control, id);
#else
				qDebug() << "Unsupported OS.";
#endif
				Q_ASSERT(audioSink);

				//g_object_set(G_OBJECT(audioSink), "provide-clock", false, NULL);

				gst_bin_add(GST_BIN(pSource->pipeline()), audioSink);
				if (!gst_element_sync_state_with_parent(audioSink))
					qCritical() << sDebugPrefix << "Cannot sync audioSink with pipeline state";
				gst_element_link(queue, audioSink);
			}


			pSource->nPadsLinked++;
			//GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pSource->pipeline()), GST_DEBUG_GRAPH_SHOW_ALL, "audio");

		}
	}
	qDebug() << sDebugPrefix <<  "padAdded done - size " << pSource->size;

}


gboolean HStimPipeline::busCallback(GstBus *, GstMessage *msg, gpointer p)
{

	HStimPipeline *pStimPipeline = (HStimPipeline *)p;
	QString sDebugPrefix;
	sDebugPrefix = QString("busCallback(%1): ").arg(GST_MESSAGE_SRC_NAME(msg));
	if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ASYNC_DONE)
	{
		qDebug() << sDebugPrefix << "Got ASYNC_DONE";

		// Loop over each stim source on this bus.
		// The ASYNC_DONE message doesn't come until all sources have prerolled, or entered PAUSED state.
		// If any of the sources are looping sources, and if they are not already prerolled, then we
		// must issue a flushing seek here on that portion of the pipeline.
// move below?		QMutexLocker locker(pStimPipeline->mutex());
		// issue flushing seek to entire pipeline. Catch segment events for each source
		if (!pStimPipeline->bInitialFlushingSeekDone)
		{
			qDebug() << sDebugPrefix << "Issue initial flushing seek to pipeline.";
			if (!gst_element_seek(pStimPipeline->pipeline(), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << sDebugPrefix << "SEEK FAILED";
			}
			else
			{
				qDebug() << sDebugPrefix << "initial pipeline flush seek done";

				QMutexLocker locker(pStimPipeline->mutex());

				pStimPipeline->bInitialFlushingSeekDone = true;
				QMapIterator<HPlayerPositionType, HStimPipelineSource* >it(pStimPipeline->getPipelineSourceMap());
				while (it.hasNext())
				{
					it.next();
					HPlayerPositionType ppt = it.key();
					HStimPipelineSource* pSource = it.value();
					qDebug() << sDebugPrefix << "Source " << ppt.name() << " is now waiting for preroll...";
					pSource->bWaitingForPreroll = true;
				}
			}
		}
		else
		{
			if (pStimPipeline->m_iAsyncPause) pStimPipeline->m_iAsyncPause--;
			qDebug() << sDebugPrefix << "pipeline is now prerolled. m_iAsyncPause=" << pStimPipeline->m_iAsyncPause;
			pStimPipeline->emitPrerolled();
			GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pStimPipeline->pipeline()), GST_DEBUG_GRAPH_SHOW_ALL, GST_ELEMENT_NAME(pStimPipeline->pipeline()));
		}
		//#endif here see bottom
	}
	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_STATE_CHANGED)
	{
		if (GST_ELEMENT(msg->src) == pStimPipeline->pipeline())
		{
			GstState old_state, new_state;
			gst_message_parse_state_changed(msg, &old_state, &new_state, NULL);
			//qDebug() << "HStimPipeline::busCallback( " << pStimPipeline->id() << "): got STATE_CHANGED " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(new_state) << " from " << GST_MESSAGE_SRC_NAME(msg);
			if (old_state == GST_STATE_PAUSED && new_state == GST_STATE_PLAYING)
			{
				pStimPipeline->emitNowPlaying();
			}
		}
	}
//	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_SEGMENT_DONE)
//	{
//		qDebug() << "HStimPipeline::busCallback( " << pStimPipeline->id() << "): SEGMENT_DONE";
//	}
	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_APPLICATION)
	{
		if (gst_message_has_name (msg, "DoSegmentSeek"))
		{
			const gchar *elementName = gst_structure_get_string(gst_message_get_structure(msg), "element");
			qDebug() << sDebugPrefix << "DoSegmentSeek on element  " << (elementName ? elementName : "???");
			GstElement *element = gst_bin_get_by_name(GST_BIN(pStimPipeline->pipeline()), elementName);
			Q_ASSERT(element);
			if (!gst_element_seek(element, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_SEGMENT, GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << sDebugPrefix << "non-flushing segment seek on element " << elementName << " FAILED";
			}
			gst_object_unref(element);
		}
		else if (gst_message_has_name (msg, "DoFlushingSegmentSeek"))
		{
			const gchar *elementName = gst_structure_get_string(gst_message_get_structure(msg), "element");
			qDebug() << sDebugPrefix << "DoFlushingSegmentSeek on element  " << (elementName ? elementName : "???");
			GstElement *element = gst_bin_get_by_name(GST_BIN(pStimPipeline->pipeline()), elementName);
			Q_ASSERT(element);
			if (!gst_element_seek(element, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << sDebugPrefix << "flushing segment seek on element " << elementName << " FAILED";
			}
			gst_object_unref(element);
		}
		else
		{
			qWarning() << sDebugPrefix << "unhandled application message ";
		}
	}
	return TRUE;
}

GstPadProbeReturn HStimPipeline::eventProbeCB(GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	HStimPipelineSource *pSource = (HStimPipelineSource *)p;
	QString sDebugPrefix;
	sDebugPrefix = QString("eventProbeCB(%1/%2): ").arg(GST_ELEMENT_NAME(GST_PAD_PARENT(pad))).arg(GST_PAD_NAME(pad));

	// Note: GST_PAD_PROBE_INFO_EVENT(d) returns a GstEvent* or NULL if info->data does not have an event.
	// In this routine, because its an event probe, it should always contain an event?

	GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
	if (event)
	{
		//GstElement* parent = GST_PAD_PARENT(pad);
		//qDebug() << sDebugPrefix << "Event type " << GST_EVENT_TYPE_NAME(event) << " from " << GST_ELEMENT_NAME(GST_PAD_PARENT(pad));

		if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT_DONE)
		{
			QMutexLocker locker(pSource->stimPipeline()->mutex());
			if (pSource->bLoop)
			{
				//qDebug() << "eventProbeCB: Looping source, got segment-done event on pad " << GST_PAD_NAME(pad) << ". Running time is " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
				if (pSource->nPadsLinked == 2 && QString(GST_PAD_NAME(pad)) != pSource->sWaitingForSegment2Pad)
				{
					// skip the segment_done on this pad, waiting for the other pad
					qDebug() << sDebugPrefix << "segment_done on Looping source(2) pad=" << GST_PAD_NAME(pad) << " expecting" << pSource->sWaitingForSegment2Pad << "No seek issued.";
				}
				else
				{
					// post message on bus - tell the bus handler (in correct thread, apparently) to issue the seek
					// IIRc there was an issue getting it to work in all cases when issuing the FLUSHING seek from this thread,
					// hence the 'send message to the bus handler' trick employed here.
				    //qDebug() << "eventProbeCB: Looping source, post bus message.";

					// We always do 'segment seek' pipelines, which means for any source we play its full length, after which
					// a SEGMENT_DONE event comes, and later (??) the bus message hits.
					// It seems to work better to do the seeks in the bus callback, so do that via an application message.




					// I've found that when the pipeline uses a container file  with a&v, the
					//    AFter initialization, each source is prerolled so its ready to go immediately (or so).
					//    I wait for the ASYNC_DONE in the bus handler, which tells me the pipeline is paused (prerolled).
					//    Looping is easier (supposedly) when you do it with segment seeks. So, a flushing seek is issued from
					//    the bus handler. Why? We want to get the pipeline to play the entire video/audio file once thru in
					//    its entirety, and to treat that as a single "segment". When the
					//    decoder gets to the end of the video/audio stream it sends a SEGMENT_DONE event. We catch that event and
					//    rather than issuing the seek from here (can't recall why, but that didn't work out), send application
					//    message to bus.


					// When there's just a single stream in the source, do a segment seek and you're done.
					// (I don't think the tracking of the seek is important except for logging)
					// When there are two streams in the source, do a segment seek, but make sure to only do it for one
					// of the two streams, otherwise the source will play a short piece of the segment from the start, but
					// then will start over again, a little hitch, when the second seek hits.

					if (pSource->nPadsLinked == 1)
					{
						GstElement *parent = gst_pad_get_parent_element(pad);
						qDebug() << sDebugPrefix << "segment_done on looping source(1 pad linked), Post DoSegmentSeek message.";
						gst_object_unref(parent);

						//if (!strcmp(GST_PAD_NAME(pad), "src_0")) return GST_PAD_PROBE_OK;
						GstBus *	bus = gst_pipeline_get_bus(GST_PIPELINE(pSource->stimPipeline()->pipeline()));
						GstStructure *structure = gst_structure_new("DoSegmentSeek", "element", G_TYPE_STRING, GST_ELEMENT_NAME(parent), NULL);
						gst_bus_post (bus, gst_message_new_application (
							  GST_OBJECT_CAST (pSource->stimPipeline()->pipeline()),
							  structure));
						gst_object_unref(bus);

						//
						pSource->bWaitingForSegment = true;
						pSource->bWaitingForSegment2 = false;
						pSource->sWaitingForSegment2Pad = ""; //GST_PAD_NAME(pad);
					}
					else if (pSource->nPadsLinked == 2)
					{
						// in this case, we make the following assumption: the source will play two streams, and they
						// may not be exactly the same length. One of them hits the end of the segment, and the other
						// does a short time later.
						// My thinking is that by catching the segment_done events we find out at the head of the
						// pipeline, and we can reload the pipeline with no glitches by doing the seek then, rather than
						// waiting in the bus handler for the SEGMENT_DONE, which doesn't come until both segment events
						// have been issued.
						//
						GstElement *parent = gst_pad_get_parent_element(pad);
						qDebug() << sDebugPrefix << "segment_done on looping source(2 pads linked), Post DoSegmentSeek message.";
						gst_object_unref(parent);


						// If the first of the two segments finishes, we issue the seek message.
						// We don't issue any more seeks from here until we see a segment_done from the
						// same pad.
						//if (!strcmp(GST_PAD_NAME(pad), "src_0")) return GST_PAD_PROBE_OK;
						GstBus *	bus = gst_pipeline_get_bus(GST_PIPELINE(pSource->stimPipeline()->pipeline()));
						GstStructure *structure = gst_structure_new("DoSegmentSeek", "element", G_TYPE_STRING, GST_ELEMENT_NAME(parent), NULL);
						gst_bus_post (bus, gst_message_new_application (
							  GST_OBJECT_CAST (pSource->stimPipeline()->pipeline()),
							  structure));
						gst_object_unref(bus);

						pSource->bWaitingForSegment = false;
						pSource->bWaitingForSegment2 = true;
						pSource->sWaitingForSegment2Pad = GST_PAD_NAME(pad);

					}
				}
			}
		}
		else if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT)
		{
			//GstElement* parent = GST_PAD_PARENT(pad);
			qDebug() << sDebugPrefix << "got SEGMENT event";
			// << ". Running time is " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
			QMutexLocker locker(pSource->stimPipeline()->mutex());
			if (pSource->bLoop)
			{
				const GstSegment *segment;
				gst_event_parse_segment(event, &segment);
				if (pSource->bWaitingForPreroll)
				{
					pSource->bWaitingForPreroll = false;
					pSource->bPrerolled = true;
					if (pSource->nPadsLinked == 2)
					{
						pSource->sWaitingForSegment2Pad = GST_PAD_NAME(pad);
					}
				}
				else if (pSource->bWaitingForSegment)
				{
					// as expected
					pSource->bWaitingForSegment = false;
					qDebug() << sDebugPrefix << "Looping source(1), got expected segment";
				}
				else if (pSource->bWaitingForSegment2)
				{
					if (pSource->sWaitingForSegment2Pad == QString(GST_PAD_NAME(pad)))
					{
						pSource->bWaitingForSegment2 = false;
						qDebug() << sDebugPrefix << "Looping source(2), got expected segment";
					}
					else
					{
						qDebug() << sDebugPrefix << "Looping source(2), expected segment on pad " << pSource->sWaitingForSegment2Pad << ", continue waiting...";
					}
				}
				else
				{
					qWarning() << sDebugPrefix << "Looping source, unexpected segment.";
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

HPipeline* HStimPipelineFactory(int id, const Habit::StimulusSettings& stimulusSettings, const Habit::StimulusDisplayInfo& info, const QDir& stimRoot, bool bStatic, QObject *parent)
{
	HPipeline *p;
	if (bStatic)
		p = new HStaticStimPipeline(id, stimulusSettings, info, stimRoot, parent);
	else
		p = new HStimPipeline(id, stimulusSettings, info, stimRoot, parent);
	return p;
}



