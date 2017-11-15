/*
 * HGMMPipeline.cpp
 *
 *  Created on: Oct 23, 2017
 *      Author: dan
 */


#include "HGMMPipeline.h"
#include "HGMMException.h"
#include <QString>
#include <gst/videotestsrc/gstvideotestsrc.h>

#define C_STR(x) x.toStdString().c_str()

// default constructor
HGMMPipeline::HGMMPipeline(QObject *parent)
: HPipeline(-999, parent)
, m_stimulusLayoutType(HStimulusLayoutType::HStimulusLayoutUnknown)
, m_root("/")
, m_bISS(false)
, m_pipeline()
{}

HGMMPipeline::HGMMPipeline(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, HVideoWidget *pCenter, bool bISS, QObject *parent)
: HPipeline(id, parent)
, m_stimulusLayoutType(HStimulusLayoutType::HStimulusLayoutSingle)
, m_stimulus(stimulus)
, m_root(stimRoot)
, m_bISS(bISS)
{
	// create the pipeline itself
	m_pipeline = gst_pipeline_new(C_STR(makeElementName("pipeline", HPlayerPositionType::Control, this->id())));
	m_mapPipelineData[HPlayerPositionType::Center].pipeline = m_pipeline;

	// create PipelineData struct. Referencing it in the map will create it.
	// no longer used m_mapPipelineData[HPlayerPositionType::Center].videoWidget = pCenter;

	// add media elements to pipeline
	addMedia(stimulus.getCenterStimulusInfo(), HPlayerPositionType::Center);

	if (bISS)
	{
		m_mapPipelineData[HPlayerPositionType::Sound].pipeline = m_pipeline;
		addMedia(stimulus.getIndependentSoundInfo(), HPlayerPositionType::Sound);
	}

	// add bus listener. Handler for pad-added signal was added (if needed) in addMedia
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	gst_bus_add_watch(bus, &HGMMPipeline::busCallback, this);
	gst_object_unref(bus);

	// preroll...
	//gst_element_set_state (m_pipeline, GST_STATE_PAUSED);
	ready();
}

HGMMPipeline::HGMMPipeline(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, HVideoWidget *pLeft, HVideoWidget *pRight, bool bISS, QObject *parent)
: HPipeline(id, parent)
, m_stimulusLayoutType(HStimulusLayoutType::HStimulusLayoutLeftRight)
, m_stimulus(stimulus)
, m_root(stimRoot)
, m_bISS(bISS)
{
	// create the pipeline itself
	m_pipeline = gst_pipeline_new(C_STR(makeElementName("pipeline", HPlayerPositionType::Control, this->id())));

	// create PipelineData structs. Referencing it in the map will create them.
	// no longer used m_mapPipelineData[HPlayerPositionType::Left].videoWidget = pLeft;
	m_mapPipelineData[HPlayerPositionType::Left].pipeline = m_pipeline;
	// no longer used m_mapPipelineData[HPlayerPositionType::Right].videoWidget = pRight;
	m_mapPipelineData[HPlayerPositionType::Right].pipeline = m_pipeline;

	// add media elements to pipeline
	addMedia(stimulus.getLeftStimulusInfo(), HPlayerPositionType::Left);
	addMedia(stimulus.getRightStimulusInfo(), HPlayerPositionType::Right);

	if (bISS)
	{
		m_mapPipelineData[HPlayerPositionType::Sound].pipeline = m_pipeline;
		addMedia(stimulus.getIndependentSoundInfo(), HPlayerPositionType::Sound);
	}

	// add bus listeners. Handler for pad-added signal was added (if needed) in addMedia
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	gst_bus_add_watch(bus, &HGMMPipeline::busCallback, this);
	gst_object_unref(bus);


	// set to ready state
	ready();
}

HPipeline *HGMMPipeline::createHGMMPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bSound, bool bISS, QObject *parent)
{
	if (layoutType == HStimulusLayoutType::HStimulusLayoutSingle)
		return new HGMMPipeline(id, stimulusSettings, stimRoot, NULL, bISS, parent);
	else
		return new HGMMPipeline(id, stimulusSettings, stimRoot, NULL, NULL, bISS, parent);

}

HGMMPipeline::~HGMMPipeline()
{
	gst_element_set_state (m_pipeline, GST_STATE_NULL);
	gst_object_unref(m_pipeline);
}

void HGMMPipeline::addMedia(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt)
{
	m_mapPipelineData[ppt].isLoop = info.isLoopPlayBack();

	if (ppt == HPlayerPositionType::Sound)
	{
		m_mapPipelineData[ppt].isAudio = true;
		setSoundStimulus(info, ppt);
	}
	else if (info.isColor() || info.isBackground())
	{
		setSolidColorStimulus(info.getColor(), ppt);
		m_mapPipelineData[ppt].needPad = false;
	}
	else
	{
		setUnknownStimulus(info, ppt);
		m_mapPipelineData[ppt].needPad = true;
	}
}

void HGMMPipeline::setSolidColorStimulus(const QColor& color, const HPlayerPositionType& ppt)
{
	GstElement *src = makeElement("videotestsrc", ppt, id());
	Q_ASSERT(src);
	g_object_set(G_OBJECT(src), "pattern", GST_VIDEO_TEST_SRC_SOLID, "foreground-color", color.rgba(), NULL);

	m_mapPipelineData[ppt].sink = makeElement("qwidget5videosink", ppt, id());
	Q_ASSERT(m_mapPipelineData[ppt].sink);

	gst_bin_add_many (GST_BIN (m_mapPipelineData[ppt].pipeline), src, m_mapPipelineData[ppt].sink, NULL);
	gst_element_link (src, m_mapPipelineData[ppt].sink);
}

void HGMMPipeline::setUnknownStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt)
{
	QString filename = info.getAbsoluteFileName(m_root);
	qDebug() << "HGMMHelper::setUnknownStimulus " << info.getFileName() << "   filename " << filename;

	GstElement *src = makeElement("filesrc", ppt, id());
	g_object_set(G_OBJECT(src), "location", C_STR(filename), NULL);
	GstElement *decodebin = makeElement("decodebin", ppt, id());
	m_mapPipelineData[ppt].convert = makeElement("autovideoconvert", ppt, id());
	m_mapPipelineData[ppt].freeze = makeElement("imagefreeze", ppt, id());
	m_mapPipelineData[ppt].identity = makeElement("identity", ppt, id());
	m_mapPipelineData[ppt].sink = makeElement("qwidget5videosink", ppt, id());
	gst_bin_add_many(GST_BIN(m_mapPipelineData[ppt].pipeline), src, decodebin, m_mapPipelineData[ppt].identity, m_mapPipelineData[ppt].convert, m_mapPipelineData[ppt].freeze, m_mapPipelineData[ppt].sink, NULL);
	gst_element_link(src, decodebin);
	g_signal_connect(decodebin, "pad-added", G_CALLBACK(&HGMMPipeline::padAdded), &m_mapPipelineData[ppt]);
}

void HGMMPipeline::setSoundStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt)
{
	// may have ISS set but still have no sound media selected
	if (info.getFileName().isEmpty())
	{
		qDebug() << "HGMMHelper::setSoundStimulus - no filename set.";
		return;
	}

	QString filename = info.getAbsoluteFileName(m_root);
	qDebug() << "HGMMHelper::setSoundStimulus " << info.getFileName() << "   filename " << filename;

	GstElement *src = makeElement("filesrc", ppt, id());
	g_object_set(G_OBJECT(src), "location", C_STR(filename), NULL);
	GstElement *decodebin = makeElement("decodebin", ppt, id());

	m_mapPipelineData[ppt].convert = makeElement("audioconvert", ppt, id());
	Q_ASSERT(m_mapPipelineData[ppt].convert);
	m_mapPipelineData[ppt].volume = makeElement("volume", ppt, id());
	Q_ASSERT(m_mapPipelineData[ppt].volume);
	g_object_set(G_OBJECT(m_mapPipelineData[ppt].volume), "volume", (double)info.getVolume()/100, NULL);
	m_mapPipelineData[ppt].identity = makeElement("identity", ppt, id());
	Q_ASSERT(m_mapPipelineData[ppt].identity);
	m_mapPipelineData[ppt].sink = makeElement("osxaudiosink", ppt, id());
	Q_ASSERT(m_mapPipelineData[ppt].sink);
	gst_bin_add_many(GST_BIN(m_mapPipelineData[ppt].pipeline), src, decodebin, m_mapPipelineData[ppt].identity, m_mapPipelineData[ppt].convert, m_mapPipelineData[ppt].volume, m_mapPipelineData[ppt].sink, NULL);
	gst_element_link(src, decodebin);
	g_signal_connect(decodebin, "pad-added", G_CALLBACK(&HGMMPipeline::padAdded), &m_mapPipelineData[ppt]);
}

void HGMMPipeline::ready()
{
	qDebug() << "HGMMPipeline::ready(" << id() << ")";
	gst_element_set_state (m_pipeline, GST_STATE_READY);
}

void HGMMPipeline::preroll()
{
	qDebug() << "HGMMPipeline::preroll(" << id() << ")";
	gst_element_set_state (m_pipeline, GST_STATE_PAUSED);
}

void HGMMPipeline::play()
{
	qDebug() << "HGMMPipeline::play(" << id() << ")";
	gst_element_set_state (m_pipeline, GST_STATE_PLAYING);
}

void HGMMPipeline::pause()
{
	qDebug() << "HGMMPipeline::pause(" << id() << ")";
	gst_element_set_state (m_pipeline, GST_STATE_PAUSED);
}

void HGMMPipeline::rewind()
{
	qDebug() << "HGMMPipeline::rewind(" << id() << ") - same as pause()";
	pause();
}
void HGMMPipeline::detachWidgetsFromSinks()
{
	GValue v = G_VALUE_INIT;
	g_value_init(&v, G_TYPE_POINTER);
	g_value_set_pointer(&v, NULL);
	QMapIterator<HPlayerPositionType, PipelineData> it(m_mapPipelineData);
	while (it.hasNext())
	{
	    it.next();
	    if (it.key() != HPlayerPositionType::Sound) g_object_set_property(G_OBJECT(it.value().sink), "widget", &v);
	}
}


// set the "widget" property for each sink to its respective widget value
void HGMMPipeline::attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1)
{
	GValue v = G_VALUE_INIT;
	g_value_init(&v, G_TYPE_POINTER);
	if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		if (m_mapPipelineData.contains(HPlayerPositionType::Center))
		{
			g_value_set_pointer(&v, w0);
			g_object_set_property(G_OBJECT(m_mapPipelineData[HPlayerPositionType::Center].sink), "widget", &v);
			if (w0)
				w0->setStimulusSize(m_mapPipelineData[HPlayerPositionType::Center].size);
		}
	}
	else if (m_stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		if (m_mapPipelineData.contains(HPlayerPositionType::Left))
		{
			g_value_set_pointer(&v, w0);
			g_object_set_property(G_OBJECT(m_mapPipelineData[HPlayerPositionType::Left].sink), "widget", &v);
			if (w0)
				w0->setStimulusSize(m_mapPipelineData[HPlayerPositionType::Left].size);
		}
		if (m_mapPipelineData.contains(HPlayerPositionType::Right))
		{
			g_value_set_pointer(&v, w1);
			g_object_set_property(G_OBJECT(m_mapPipelineData[HPlayerPositionType::Right].sink), "widget", &v);
			if (w1)
				w1->setStimulusSize(m_mapPipelineData[HPlayerPositionType::Right].size);
		}
	}
	else
		throw HGMMException("Unhandled stimulus layout type!");
}


bool HGMMPipeline::hasPads() const
{
	bool b = true;
	QMapIterator<HPlayerPositionType, PipelineData> it(m_mapPipelineData);
	while (it.hasNext())
	{
	    it.next();
	    b = b && !it.value().needPad;
	}
	return b;
}


QString HGMMPipeline::makeElementName(const char *factoryName, const HPlayerPositionType& ppt, int number)
{
	QString format("%1-%2-%3");
	return format.arg(QString(factoryName)).arg(ppt.name()).arg(number);
}

GstElement *HGMMPipeline::makeElement(const char *factoryName, const HPlayerPositionType& ppt, int number)
{
	return gst_element_factory_make(factoryName, makeElementName(factoryName, ppt, number).toStdString().c_str());
}

QString HGMMPipeline::stateName(GstState s)
{
	QString name("UNKNOWN");
	switch (s)
	{
	case GST_STATE_VOID_PENDING:
		name = QString("VoidPending");
		break;
	case GST_STATE_NULL:
		name = QString("Null");
		break;
	case GST_STATE_READY:
		name = QString("Ready");
		break;
	case GST_STATE_PAUSED:
		name = QString("Paused");
		break;
	case GST_STATE_PLAYING:
		name = QString("Playing");
		break;
	}
	return name;
}

bool HGMMPipeline::parseElementName(const QString& elementName, QString& factoryName, const HPlayerPositionType*& pppt, int& id)
{
	bool b = false;
	QStringList l = elementName.toLower().split('-');
	if (l.size() != 3) return false;

	factoryName = l[0];

	if (l[1] == QString("left"))
		pppt = &HPlayerPositionType::Left;
	else if (l[1] == QString("right"))
		pppt = &HPlayerPositionType::Right;
	else if (l[1] == QString("center"))
		pppt = &HPlayerPositionType::Center;
	else if (l[1] == QString("control"))
		pppt = &HPlayerPositionType::Control;
	else
		return false;

	id = l[2].toInt(&b);
	return b;
}

void HGMMPipeline::emitNowPlaying()
{
	emit nowPlaying();
}

gboolean HGMMPipeline::busCallback(GstBus *, GstMessage *msg, gpointer pdata)
{
	GstState state, old_state;
	QString factoryName;
	int id;
	const HPlayerPositionType* pppt;
	HGMMPipeline *pPipeline = (HGMMPipeline *)pdata;

	switch(GST_MESSAGE_TYPE(msg))
	{
	case GST_MESSAGE_ASYNC_DONE:
		qDebug() << "Got ASYNC_DONE on " << GST_MESSAGE_SRC_NAME(msg);
		if (!pPipeline->isPrerolled)
		{
			// do flushing seek GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT
			qDebug() <<"flush seek " << GST_MESSAGE_SRC_NAME(msg);
			if (!gst_element_seek(GST_ELEMENT(pPipeline->pipeline()), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << "SEEK FAILED";
			}
			else
			{
				qDebug() << "flush seek done " << GST_MESSAGE_SRC_NAME(msg);
				pPipeline->isPrerolled = true;	// so we know we've done this already
			}
		}
		else
		{
			qDebug() << "already prerolled " << GST_MESSAGE_SRC_NAME(msg);
		}
		break;
	case GST_MESSAGE_SEGMENT_DONE:
		qDebug() << "Got SEGMENT_DONE on " << GST_MESSAGE_SRC_NAME(msg);
		if (pPipeline->getPipelineDataMap().count() == 1)
		{
			PipelineData& pPipelineData = pPipeline->getPipelineDataMap().first();
			if (pPipelineData.isVideo && pPipelineData.isLoop)
			{
				// do non-flushing seek GST_SEEK_FLAG_SEGMENT
				qDebug() << "Got SEGMENT_DONE on " << GST_MESSAGE_SRC_NAME(msg) << " do flushing segment seek on pipeline";
				if (!gst_element_seek(pPipelineData.pipeline, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
				{
					g_print("SEEK FAILED\n");
				}
			}
			else
				qDebug() << "Got SEGMENT_DONE on " << GST_MESSAGE_SRC_NAME(msg) << " not looped - do nothing";

		}
		else if (pPipeline->getPipelineDataMap().contains(HPlayerPositionType::Sound))
		{
			if (!gst_element_seek(GST_ELEMENT(pPipeline->pipeline()), 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				g_print("SEEK FAILED\n");
			}
			else
				qDebug() << "sound seek ok?";
		}
		break;
	case GST_MESSAGE_STATE_CHANGED:
		gst_message_parse_state_changed(msg, &old_state, &state, NULL);
		if (state == GST_STATE_PLAYING &&
				parseElementName(GST_MESSAGE_SRC_NAME(msg), factoryName, pppt, id) &&
				factoryName == "pipeline")
		{
			qDebug() << "STATE CHANGED "<< GST_MESSAGE_SRC_NAME(msg) << factoryName << "/" << pppt->name() << "/" << id << " - " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(state);
			pPipeline->emitNowPlaying();
		}
		break;
	default:
		break;
	}
	return TRUE;
}

void HGMMPipeline::padAdded(GstElement *src, GstPad *new_pad, PipelineData *pdata)
{
	PipelineData *pPipelineData = (PipelineData *)pdata;
	GstCaps *new_pad_caps = NULL;
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;
	gchar *s_new_pad_caps = NULL;
	const GValue *v = NULL;
	new_pad_caps = gst_pad_get_current_caps(new_pad);
	new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
	new_pad_type = gst_structure_get_name(new_pad_struct);
	s_new_pad_caps = gst_caps_to_string(new_pad_caps);
	qDebug() << "Got new pad '" << GST_PAD_NAME(new_pad) << " from " << GST_ELEMENT_NAME(src);
	qDebug() << "Caps: " << s_new_pad_caps;

	if (g_str_has_prefix(s_new_pad_caps, "video/x-raw"))
	{
		int width=0, height=0;

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
		pdata->size = QSize(width, height);

		// check framerate
		v = gst_structure_get_value(new_pad_struct, "framerate");
		if (v && GST_VALUE_HOLDS_FRACTION(v))
		{
			gint num = gst_value_get_fraction_numerator(v);
			if (num == 0)
			{
				GstPad *sink_pad = gst_element_get_static_pad(pPipelineData->convert, "sink");
				gst_pad_link(new_pad, sink_pad);
				gst_object_unref(sink_pad);
				gst_element_link (pPipelineData->convert, pPipelineData->freeze);
				gst_element_link (pPipelineData->freeze, pPipelineData->sink);
				pdata->needPad = false;
			}
			else if (num > 0)
			{
				GstPad *sink_pad = gst_element_get_static_pad(pPipelineData->identity, "sink");
				gst_pad_link(new_pad, sink_pad);
				gst_object_unref(sink_pad);
				if (!gst_element_link (pPipelineData->identity, pPipelineData->convert)) g_print("IDENITY-CONVERT LINK FAILED\n");
				if (!gst_element_link (pPipelineData->convert, pPipelineData->sink)) g_print("CONVERT-SINK LINK FAILED\n");
				pdata->needPad = false;
				pdata->isVideo = true;

				// Create pad probe on convert src pad
				GstPad *convert_srcpad = gst_element_get_static_pad (pPipelineData->convert, "src");
				gst_pad_add_probe (convert_srcpad,
				      GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HGMMPipeline::eventProbeCB, (gpointer)pPipelineData, NULL);
				gst_object_unref (convert_srcpad);


				//GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(pPipelineData->pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "movie");
			}
			else qCritical() << "negative numerator?";
		}
		else qCritical() << "framerate not in caps or unexpected value type";
	}
	else if (g_str_has_prefix(s_new_pad_caps, "audio/x-raw"))
	{
		// check if we're expecting sound
		if (pPipelineData->isAudio)
		{
			qDebug() << "linking audio source...";
			GstPad *sink_pad = gst_element_get_static_pad(pPipelineData->identity, "sink");
			gst_pad_link(new_pad, sink_pad);
			gst_object_unref(sink_pad);
			if (!gst_element_link (pPipelineData->identity, pPipelineData->convert)) g_print("IDENTITY-CONVERT LINK FAILED\n");
			if (!gst_element_link (pPipelineData->convert, pPipelineData->volume)) g_print("CONVERT-VOLUME LINK FAILED\n");
			if (!gst_element_link (pPipelineData->volume, pPipelineData->sink)) g_print("VOLUME-SINK LINK FAILED\n");
		}
		else
		{
			qDebug() << "Audio from video src - TODO";
		}
	}
	// cleanup
	gst_caps_unref(new_pad_caps);
	return;
}


GstPadProbeReturn HGMMPipeline::eventProbeCB (GstPad * pad, GstPadProbeInfo * info, gpointer)
{
	GstElement* parent = GST_PAD_PARENT(pad);
	gchar *parentName = gst_element_get_name(parent);
	g_print("got event %s from %s\n", GST_EVENT_TYPE_NAME(GST_PAD_PROBE_INFO_EVENT(info)), parentName);
	g_free(parentName);
	if (GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info)) == GST_EVENT_SEGMENT)
	{
		// get current state of element
		GstState currentState, pendingState;
		GstStateChangeReturn ret;
		ret = gst_element_get_state(parent, &currentState, &pendingState, 0);
		if (ret == GST_STATE_CHANGE_SUCCESS)
		{
			g_print("element is in state %s\n", gst_element_state_get_name(currentState));
		}
		else
		{
			g_print("element state change return ???\n");
		}

		// get element base time
		g_print("element base time is %" G_GUINT64_FORMAT "\n", gst_element_get_base_time(parent));

		//GST_ELEMENT_START_TIME() = running time when element went to PAUSED



		const GstSegment* segment;
		gst_event_parse_segment(GST_EVENT(GST_PAD_PROBE_INFO_DATA (info)), &segment);
		g_print("running time (0) of this segment: %" G_GUINT64_FORMAT "\n", gst_segment_to_running_time(segment, GST_FORMAT_TIME, 0));
		g_print("running time (-1) of this segment: %" G_GUINT64_FORMAT "\n", gst_segment_to_running_time(segment, GST_FORMAT_TIME, -1));
		// get element base time
		g_print("element base time is %" G_GUINT64_FORMAT "\n", gst_element_get_base_time(parent));
		g_print("clock time is        %" G_GUINT64_FORMAT "\n", gst_clock_get_time(gst_element_get_clock(parent)));
		g_print("running time is      %" G_GUINT64_FORMAT "\n", gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));

	}
	else if (GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info)) == GST_EVENT_SEGMENT_DONE)
	{

		// get current state of element
		GstState currentState, pendingState;
		GstStateChangeReturn ret;
		ret = gst_element_get_state(parent, &currentState, &pendingState, 0);
		if (ret == GST_STATE_CHANGE_SUCCESS)
		{
			g_print("element is in state %s\n", gst_element_state_get_name(currentState));
		}
		else
		{
			g_print("element state change return ???\n");
		}

		// get element base time
		g_print("element base time is %" G_GUINT64_FORMAT "\n", gst_element_get_base_time(parent));
		g_print("clock time is        %" G_GUINT64_FORMAT "\n", gst_clock_get_time(gst_element_get_clock(parent)));
		g_print("running time is      %" G_GUINT64_FORMAT "\n", gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
	}
	//if (GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info)) == GST_EVENT_EOS)
	//g_print("Got event\n");
    return GST_PAD_PROBE_PASS;
}
