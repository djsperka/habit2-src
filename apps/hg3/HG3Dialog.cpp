/*
 * Q5PlayerTestDialog.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: dan
 */

#include <QDialogButtonBox>
#include <QDesktopWidget>
#include <QtDebug>

#include "stimulisettings.h"
#include "stimulusdisplayinfo.h"
#include "HG3Dialog.h"
#include "HPipeline.h"
#include "HStimulusPipeline.h"
#include <gst/gst.h>
#include <gst/videotestsrc/gstvideotestsrc.h>
#include <gst/audiotestsrc/gstaudiotestsrc.h>

#define MAX_INFO 5
Habit::StimulusInfo f_vinfo[MAX_INFO];
Habit::StimulusInfo f_ainfo[MAX_INFO];
Habit::StimuliSettings f_stimuli;

#if 0
guint64 last_running_time=0;

class HStimulusPipeline: public HPipeline
{
	const HStimulusLayoutType& m_stimulusLayoutType;
	bool m_bISS;
	GstElement *m_pipeline;
	GstElement *m_audioMixer;
	GstElement *m_audioSink;
	bool m_bAudioElementsAddedToPipeline;



	// This struct is used to configure a bin that handles a Habit::StimulusInfo.
	// There can be video, audio, or a/v. If there is video, then videoSink is the qwidget5videosink element
	//
	// The simplest bin is the solid color.
	// All other stimuli are partially created in the constuctor - filesrc ! decodebin.
	// The 'pad-added' signal is caught and the bin is completed in the callback (padAdded)
	// Image stimuli use imagefreeze to generate video streams from a single image.
	// In all other cases, the 'pad-added' may pass a video stream and/or an audio stream.
	// A video stream is connected to a videoSink, unless we are told to 'ignoreVideo'. That is the case for all files given as ISS.
	// An audio stream is connected to an audiomixer. All allowed audio streams are thus mixed (and clamped, I believe). An audio stream is
	// ignored if its volume is zero.
	//
	// If the stim is to loop, then things follow a 2-step process.
	// When the stim is prerolled, we catch the 'ASYNC_DONE' message on the bus. That message is posted only after all elements in the pipeline
	// have reached PAUSED. If there are multiple files - e.g. L/R stim, L.R&Sound, etc., each is loaded/decoded, in separate threads, so each is a
	// separate asynchronous task. Once we have the ASYNC_DONE, we issue a flushing seek. That causes the first frames of the video/audio to be re-sent,
	// but this time preceded by a SEGMENT event. Once the segment completes, a SEGMENT_DONE event is sent. We catch that event and send another seek
	// to the beginning of the segment. We do that every time the SEGMENT_DONE is found.
	//
	// When there are two or more streams in a pipeline, the seeking/prerolling is handled separately. The event probe, the callback that
	// catches the SEGMENT_DONE event and re-issues the seek, is installed on the first sink pad downstreanm of the decodebin. It sends
	// the seek to the decodebin element which feeds it. If a file is a container with video and audio, and it is to loop, then
	// we favor the video stream - when the video stream has a SEGMENT_DONE, the seek is issued to the decodebin element which feeds both streams.
	//
	//

	struct BinData
	{
		HStimulusPipeline *stimulusPipeline; // need to point to parent
		const HPlayerPositionType& ppt;		// which position this bin represents
		GstElement *decodebin;				// the decoder element, need this to do seeks
		GstElement *videoSink;				// if there is video, this is the sink element, qwidget5videosink
		bool ignoreAudio;					// advice to padAdded to ignore any audio streams. If any are found, they are connected to a fakesink
		bool ignoreVideo;					// advice to padAdded to ignore any video streams. If any are found, they are connected to a fakesink
		bool isPrerolled;					// set after the stimulus is prerolled (TODO - exactly where should this be done?)
		bool isLooping;						// if the StimulusInfo specified for this stim to loop.
		int volume;							// volume level, 0-100.
		QSize size;							// resolution of video, if any. Can be undefined -- will take any shape of target screen (i.e.full screen)

		BinData(HStimulusPipeline *pipeline, const HPlayerPositionType& pptype): stimulusPipeline(pipeline), ppt(pptype), decodebin(NULL), videoSink(NULL), ignoreAudio(false), ignoreVideo(false), isPrerolled(false), isLooping(false), volume(0)
		{
			if (ppt != HPlayerPositionType::Sound)
			{
				videoSink = makeElement("qwidget5videosink", ppt, stimulusPipeline->id());
				Q_ASSERT(videoSink);
				gst_bin_add_many (GST_BIN (pipeline->pipeline()), videoSink, NULL);
			}
		};
	};

	QMap<HPlayerPositionType, BinData* > m_mapBinData;


	void addStimulusInfo(BinData *pdata, const Habit::StimulusInfo& info, const QDir& stimRoot, const HPlayerPositionType& ppt)
	{
		qDebug() << "addStimulusInfo:";
		qDebug() << info;
		qDebug() << stimRoot;
		qDebug() << ppt.name();

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

	void setSizeOnWidget(HVideoWidget *w, const HPlayerPositionType& ppt)
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

	void setWidgetPropertyOnSink(HVideoWidget *w, const HPlayerPositionType& ppt)
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

	void lazyAudioInitializeAndAddToPipeline()
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
public:
	HStimulusPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bISS, QObject *parent)
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

			// if this source is looping, set up a bus listener
			// add bus listener. Handler for pad-added signal was added (if needed) in addMedia
			if (pdata->isLooping)
			{
				qDebug() << "Install bus callback";
				GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
				gst_bus_add_watch(bus, &HStimulusPipeline::busCallback, this);
				gst_object_unref(bus);
			}
			else
				qDebug() << "Not looping, no bus callback.";
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

			if (pdataLeft->isLooping || pdataRight->isLooping)
			{
				GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline()));
				gst_bus_add_watch(bus, &HStimulusPipeline::busCallback, this);
				gst_object_unref(bus);
			}
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

	virtual ~HStimulusPipeline()
	{
		// clean up all gstreamer stuff we've accumulated
		qDeleteAll(m_mapBinData);
		m_mapBinData.clear();
	}

	void ready()
	{
		qDebug() << "HStimulusPipeline::ready(" << id() << ")";
		gst_element_set_state(pipeline(), GST_STATE_READY);
	}

	void pause()
	{
		qDebug() << "HStimulusPipeline::pause(" << id() << ")";
		gst_element_set_state(pipeline(), GST_STATE_PAUSED);
	}

	void play()
	{
		qDebug() << "HStimulusPipeline::play(" << id() << ")";
		gst_element_set_state(pipeline(), GST_STATE_PLAYING);
	}

	void preroll()
	{
		// TODO - prepare for prerolling
		qDebug() << "HStimulusPipeline::preroll(" << id() << ")";
		gst_element_set_state(pipeline(), GST_STATE_PAUSED);
	}

	void rewind()
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

	void attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1)
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

	void detachWidgetsFromSinks()
	{
		attachWidgetsToSinks(NULL, NULL);
	}

	GstElement *pipeline() const { return m_pipeline; }
	bool iss() const { return m_bISS; }
	const HStimulusLayoutType& stimulusLayoutType() const { return m_stimulusLayoutType; }
	QMap<HPlayerPositionType, BinData* >& getBinDataMap() { return m_mapBinData; }

	virtual void write(std::ostream& os) const
	{
	    os << "Pipeline Key: " << id() << std::endl;
	    os << "Name: " << GST_ELEMENT_NAME(pipeline()) << std::endl;
	}

	static void parseCaps(GstCaps* caps, bool& isVideo, bool& isImage, int& width, int& height, bool& isAudio)
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

	static void noMorePads(GstElement *src, gpointer p)
	{
		qDebug() << "Got noMorePads for " << GST_ELEMENT_NAME(src);

		// clean up unused elements.

	}



	static void padAdded(GstElement *src, GstPad *newPad, gpointer p)
	{
		BinData *pdata = (BinData *)p;
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

	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata)
	{
		GstState state, old_state;
		QString factoryName;
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
			gst_message_parse_state_changed(msg, &old_state, &state, NULL);
			if (state == GST_STATE_PLAYING && old_state == GST_STATE_PAUSED)
			{
				qDebug() << GST_MESSAGE_SRC_NAME(msg) << " - " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(state);
				//pPipeline->emitNowPlaying();
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

	static GstPadProbeReturn eventProbeCB (GstPad * pad, GstPadProbeInfo * info, gpointer p)
	{
		BinData *pdata = (BinData *)p;

		// Note: GST_PAD_PROBE_INFO_EVENT(d) returns a GstEvent* or NULL if info->data does not have an event.
		// In this routine, because its an event probe, it should always contain an event?

		GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
		if (event)
		{
			GstElement* parent = GST_PAD_PARENT(pad);
			gchar *parentName = gst_element_get_name(parent);
			qDebug() << "Got event from " << parentName << " type: " << GST_EVENT_TYPE_NAME(event);
			g_free(parentName);
			if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT)
			{
//				g_print("element base time is %" G_GUINT64_FORMAT "\n", gst_element_get_base_time(parent));
//				g_print("clock time is        %" G_GUINT64_FORMAT "\n", gst_clock_get_time(gst_element_get_clock(parent)));
//				g_print("running time is      %" G_GUINT64_FORMAT "\n", gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
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


};

HPipeline* myFactory(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bSound, bool bISS, QObject *parent)
{
	qDebug() << "Calling myFactory for stimulus:";
	qDebug() << stimulusSettings;

	HStimulusPipeline *p = new HStimulusPipeline(id, stimulusSettings, stimRoot, layoutType, bISS, parent);
	return p;
}

#endif


HG3Dialog::HG3Dialog(const QDir& dirStimRoot, int screen, const QString& flag, QWidget *parent)
: QDialog(parent)
, m_pVideoWidgetLeft(NULL)
, m_pVideoWidgetRight(NULL)
, m_pVideoWidgetCenter(NULL)
, m_iCurrent(-1)
{
	QDialogButtonBox *buttonBox;
	QVBoxLayout *vbox;
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, parent);
	m_pbReady = new QPushButton("Ready");
	m_pbPreroll = new QPushButton("Preroll");
	m_pbPause = new QPushButton("Pause");
	m_pbRewind = new QPushButton("Rewind");
	m_pbPlay = new QPushButton("PLay");
	vbox = new QVBoxLayout;
	buttonBox->addButton(m_pbReady, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbPreroll, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbPause, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbRewind, QDialogButtonBox::ActionRole);
	buttonBox->addButton(m_pbPlay, QDialogButtonBox::ActionRole);
	connect(m_pbReady, SIGNAL(clicked()), this, SLOT(readyClicked()));
	connect(m_pbPreroll, SIGNAL(clicked()), this, SLOT(prerollClicked()));
	connect(m_pbRewind, SIGNAL(clicked()), this, SLOT(rewindClicked()));
	connect(m_pbPause, SIGNAL(clicked()), this, SLOT(pauseClicked()));
	connect(m_pbPlay, SIGNAL(clicked()), this, SLOT(playClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));

	if (screen < 1)
	{
		vbox->addWidget(new QLabel("HELLO"));
	}
	else if (screen == 1)
	{
		Habit::StimulusDisplayInfo sdi;
		sdi.setBackGroundColor(QColor(Qt::blue));
		sdi.setDisplayType(HDisplayType::HDisplayTypeOriginalSize);
		sdi.setMaintainOriginalAspectRatio(true);
		sdi.setPresentationStyle(HPresentationStyle::HPresentationStyleMonitorDefined);
		sdi.setStimulusLayoutType(HStimulusLayoutType::HStimulusLayoutSingle);
		sdi.setUseISS(true);
		vbox->addLayout(initSingleScreen(sdi, dirStimRoot, flag));
	}
	else
	{
		Habit::StimulusDisplayInfo sdi;
		sdi.setBackGroundColor(QColor(Qt::blue));
		sdi.setDisplayType(HDisplayType::HDisplayTypeOriginalSize);
		sdi.setMaintainOriginalAspectRatio(true);
		sdi.setPresentationStyle(HPresentationStyle::HPresentationStyleMonitorDefined);
		sdi.setStimulusLayoutType(HStimulusLayoutType::HStimulusLayoutLeftRight); // not used?
		sdi.setUseISS(true);
		vbox->addLayout(initLRScreen(sdi, dirStimRoot, flag));
	}

	m_sbWhich = new QSpinBox(this);
	m_sbWhich->setMinimum(0);
	m_sbWhich->setMaximum(m_pmm->nStimuli()-1);

	vbox->addWidget(m_sbWhich);
	vbox->addWidget(buttonBox);
	setLayout(vbox);


	//connect(m_paudioHGstPlayer, SIGNAL(started(int, const QString&)), this, SLOT(started(int, const QString& )));


}

HG3Dialog::~HG3Dialog()
{
	//delete m_paudioHGstPlayer;
	//m_pvideoHGstPlayer->stop();
	//delete m_pvideoHGstPlayer;
}

void HG3Dialog::stimStarted(int key)
{
	qDebug() << "HG3Dialog::stimStarted " << key;
}

void HG3Dialog::agStarted()
{
	qDebug() << "HG3Dialog::agStarted";
}

QHBoxLayout *HG3Dialog::initSingleScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& dirStimRoot, const QString& flag)
{
	QHBoxLayout *hbox = new QHBoxLayout;
	m_pVideoWidgetCenter = new HStimulusWidget(sdi, 1024, 768);
	//m_pVideoWidgetCenter->setMinimumSize(320, 240);
	hbox->addWidget(m_pVideoWidgetCenter);

	m_pmm = new HGMM(m_pVideoWidgetCenter, dirStimRoot, true, sdi.getBackGroundColor(), HStimulusPipelineFactory);
	//connect(m_pmm, SIGNAL(mmReady()), this, SLOT(mmReady()));
	//connect(m_pmm, SIGNAL(mmFail()), this, SLOT(mmFail()));
	connect(m_pmm, SIGNAL(agStarted()), this, SLOT(agStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	connect(m_pmm, SIGNAL(stimulusChanged()), m_pVideoWidgetCenter->getHVideoWidget(), SLOT(stimulusChanged()));

	// create background
//	m_pmm->addBackground(QColor(Qt::green));

	// pic
	if (flag.contains("all") || flag.contains("2"))
	{
		Habit::StimulusSettings s2;
		s2.setName("s2-ocean-nosound");
		Habit::StimulusInfo si2(QString("oceanz"), QString("images/posters/ocean.jpg"));
		s2.setCenterStimulusInfo(si2);
		f_stimuli.addStimulus(s2);
	}

	// movie - taxi hgas a sound stream
	if (flag.contains("all") || flag.contains("3"))
	{
		Habit::StimulusSettings s3;
		s3.setName("s3-taxi-nosound");
		Habit::StimulusInfo si3(QString("taxi"), QString("mov/taxi-480p.mov"), true);
		s3.setCenterStimulusInfo(si3);
		f_stimuli.addStimulus(s3);
	}

	if (flag.contains("all") || flag.contains("4"))
	{
		Habit::StimulusSettings s3;
		s3.setName("s4-drop-nosound");
		Habit::StimulusInfo si3(QString("drop"), QString("avi/toy_plane_liftoff.avi"), true);
		s3.setCenterStimulusInfo(si3);
		f_stimuli.addStimulus(s3);
	}

	if (flag.contains("all") || flag.contains("5"))
	{
		Habit::StimulusSettings s4;
		s4.setName("s5-excavator-nosound");
		Habit::StimulusInfo si4(QString("excavator"), QString("images/tools/excavator.png"));
		s4.setCenterStimulusInfo(si4);
		f_stimuli.addStimulus(s4);
	}

	if (flag.contains("all") || flag.contains("6"))
	{
		// color + sound no loop
		Habit::StimulusSettings s6;
		s6.setName("s6-tire-whistle");
		Habit::StimulusInfo si6(QString("tire"), QString("images/tools/tire.png"));
		s6.setCenterStimulusInfo(si6);
		Habit::StimulusInfo sis6("wav/slide_whistle_x.wav", true, 50);
		s6.setIndependentSoundInfo(sis6);
		f_stimuli.addStimulus(s6);
	}

	if (flag.contains("all") || flag.contains("7"))
	{
		// color + sound no loop
		Habit::StimulusSettings s5;
		s5.setName("s7-green-bowling");
		Habit::StimulusInfo si5(Qt::green);
		s5.setCenterStimulusInfo(si5);
		Habit::StimulusInfo sis5("wav/bowling.wav", true, 50);
		s5.setIndependentSoundInfo(sis5);
		f_stimuli.addStimulus(s5);
	}

	if (flag.contains("all") || flag.contains("8"))
	{
		// color + sound no loop
		Habit::StimulusSettings s8;
		s8.setName("s8-red-whistle-from-movie");
		Habit::StimulusInfo si8(Qt::red);
		s8.setCenterStimulusInfo(si8);
		Habit::StimulusInfo sis8("mov/Loop.mov", true, 50);
		s8.setIndependentSoundInfo(sis8);
		f_stimuli.addStimulus(s8);
	}

	m_pmm->addStimuli(f_stimuli, 1);

	//m_pmm->getReady(5000);

	m_pmm->preroll(0);
	m_pmm->preroll(1);
	//m_pmm->stim(0);
	m_iCurrent = 0;

	return hbox;
}


QHBoxLayout *HG3Dialog::initLRScreen(const Habit::StimulusDisplayInfo& sdi, const QDir& dirStimRoot, const QString& flag)
{
	QHBoxLayout *hbox = new QHBoxLayout;
	m_pVideoWidgetLeft = new HStimulusWidget(sdi, 800, 600);
	m_pVideoWidgetRight = new HStimulusWidget(sdi, 800, 600);
	hbox->addWidget(m_pVideoWidgetLeft);
	hbox->addWidget(m_pVideoWidgetRight);

	m_pmm = new HGMM(m_pVideoWidgetLeft, m_pVideoWidgetRight, dirStimRoot, true, sdi.getBackGroundColor(), HStimulusPipelineFactory);
	//connect(m_pmm, SIGNAL(mmReady()), this, SLOT(mmReady()));
	//connect(m_pmm, SIGNAL(mmFail()), this, SLOT(mmFail()));
	connect(m_pmm, SIGNAL(agStarted()), this, SLOT(agStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	connect(m_pmm, SIGNAL(stimulusChanged()), m_pVideoWidgetLeft->getHVideoWidget(), SLOT(stimulusChanged()));
	connect(m_pmm, SIGNAL(stimulusChanged()), m_pVideoWidgetRight->getHVideoWidget(), SLOT(stimulusChanged()));


	// l/r color b/g no sound
	if (flag.contains("all") || flag.contains("2"))
	{
		Habit::StimulusSettings s;
		s.setName("s1");
		s.setLeftStimulusInfo(Habit::StimulusInfo(Qt::blue));
		s.setRightStimulusInfo(Habit::StimulusInfo(Qt::green));
		f_stimuli.addStimulus(s);
	}

	// l/r color b/g iss
	if (flag.contains("all") || flag.contains("3"))
	{
		Habit::StimulusSettings s;
		s.setName("s2");
		s.setLeftStimulusInfo(Habit::StimulusInfo(Qt::red));
		s.setRightStimulusInfo(Habit::StimulusInfo(Qt::yellow));
		s.setIndependentSoundInfo(Habit::StimulusInfo(QString("sounds/ant-marching-band.mp3"), false, 50));
		f_stimuli.addStimulus(s);
	}

	// l/r color b/g iss
	if (flag.contains("all") || flag.contains("4"))
	{
		Habit::StimulusSettings s;
		s.setName("s4");
		s.setLeftStimulusInfo(Habit::StimulusInfo(Qt::cyan));
		s.setRightStimulusInfo(Habit::StimulusInfo(QString("anchovy"), QString("images/fish/anchovy.jpg")));
		f_stimuli.addStimulus(s);
	}

	// l/r color b/g iss
	if (flag.contains("all") || flag.contains("5"))
	{
		Habit::StimulusSettings s;
		s.setName("s5");
		s.setLeftStimulusInfo(Habit::StimulusInfo(QString("scad"), QString("images/fish/scad.jpg")));
		s.setRightStimulusInfo(Habit::StimulusInfo(Qt::magenta));
		s.setIndependentSoundInfo(Habit::StimulusInfo(QString("sounds/sad-trombone.wav"), true, 50));
		f_stimuli.addStimulus(s);
	}

	//
	if (flag.contains("all") || flag.contains("6"))
	{
		Habit::StimulusSettings s;
		s.setName("s6");
		s.setLeftStimulusInfo(Habit::StimulusInfo(QString("kami"), QString("mp4/KAMI2001_64kb.mp4"), false, 50));
		s.setRightStimulusInfo(Habit::StimulusInfo(QString("SahyCheese"), QString("mp4/SayChees2001_64kb.mp4"), false, 0));
		f_stimuli.addStimulus(s);
	}

	if (flag.contains("all") || flag.contains("7"))
	{
		Habit::StimulusSettings s;
		s.setName("s7");
		s.setLeftStimulusInfo(Habit::StimulusInfo(QString("kami"), QString("mp4/KAMI2001_64kb.mp4"), false, 0));
		s.setRightStimulusInfo(Habit::StimulusInfo(QString("SahyCheese"), QString("mp4/SayChees2001_64kb.mp4"), false, 50));
		f_stimuli.addStimulus(s);
	}

	if (flag.contains("all") || flag.contains("8"))
	{
		Habit::StimulusSettings s;
		s.setName("s8");
		s.setLeftStimulusInfo(Habit::StimulusInfo(QString("kami"), QString("mp4/KAMI2001_64kb.mp4"), false, 0));
		s.setRightStimulusInfo(Habit::StimulusInfo(QString("SahyCheese"), QString("mp4/SayChees2001_64kb.mp4"), false, 0));
		s.setIndependentSoundInfo(Habit::StimulusInfo(QString("sounds/welcome-to-the-internet.mp3"), true, 50));
		f_stimuli.addStimulus(s);
	}

//	//
//	{
//		Habit::StimulusSettings s;
//		s.setName("s");
//		s.setLeftStimulusInfo(Habit::StimulusInfo());
//		s.setRightStimulusInfo(Habit::StimulusInfo());
//		s.setIndependentSoundInfo(Habit::StimulusInfo());
//		f_stimuli.addStimulus(s);
//	}

#if 0
	// pic
	Habit::StimulusSettings s1;
	s1.setName("s1");
	Habit::StimulusInfo si1L(QString("ocean"), QString("images/posters/ocean.jpg"));
	s1.setLeftStimulusInfo(si1L);
	Habit::StimulusInfo si1R(QString("owl"), QString("images/posters/owl.jpg"));
	s1.setRightStimulusInfo(si1R);

	f_stimuli.addStimulus(s1);

	// movie
	Habit::StimulusSettings s2;
	s2.setName("s2");
	Habit::StimulusInfo si2L(QString("earthsun"), QString("mov/earthsun.mov"), false);
	s2.setLeftStimulusInfo(si2L);
	Habit::StimulusInfo si2R(QString("taxi"), QString("mov/taxi-480p.mov"), true);
	s2.setRightStimulusInfo(si2R);
	f_stimuli.addStimulus(s2);

	Habit::StimulusSettings s3;
	s3.setName("s3");
	Habit::StimulusInfo si3L(QString("small"), QString("mov/small.mp4"), true);
	s3.setLeftStimulusInfo(si3L);
	Habit::StimulusInfo si3R(QString("tire"), QString("images/tools/tire.png"));
	s3.setRightStimulusInfo(si3R);
	f_stimuli.addStimulus(s3);

	Habit::StimulusSettings s4;
	s4.setName("s4");
	Habit::StimulusInfo si4L(QString("hydrant"), QString("images/tools/hydrant.png"));
	s4.setLeftStimulusInfo(si4L);
	Habit::StimulusInfo si4R(QString("excavator"), QString("images/tools/excavator.png"));
	s4.setRightStimulusInfo(si4R);
	f_stimuli.addStimulus(s4);
#endif

	m_pmm->addStimuli(f_stimuli, 1);

//	m_pmm->preroll(0);
//	m_pmm->preroll(1);
	m_pmm->stim(0);
	m_iCurrent = 0;

	//m_pmm->getReady(5000);
	return hbox;
}


#if 0
void HG3Dialog::mediaChanged(libvlc_media_t *media)
{
	qInfo() << "media changed";
	// what can we find out about the media?

}
#endif

void HG3Dialog::started(int screenid, const QString& filename)
{
	qInfo() << "started " << screenid << ":" << filename;
}

void HG3Dialog::readyClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "ready " << i;
	m_pmm->ready(i);
}

void HG3Dialog::prerollClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "preroll " << i;
	m_pmm->preroll(i);
}

void HG3Dialog::rewindClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "rewind " << i;
	m_pmm->rewind(i);
}

void HG3Dialog::pauseClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "pause " << i;
	m_pmm->pause(i);
}

void HG3Dialog::playClicked()
{
	int i = m_sbWhich->value();
	qDebug() << "play " << i;
	m_pmm->stim(i);
}

void HG3Dialog::mmReady()
{
	qDebug() << "HG3Dialog::mmReady";
}

void HG3Dialog::mmFail()
{
	qDebug() << "HG3Dialog::mmFail";
}
