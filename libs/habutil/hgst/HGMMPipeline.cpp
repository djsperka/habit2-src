/*
 * HGMMPipeline.cpp
 *
 *  Created on: Oct 23, 2017
 *      Author: dan
 */


#include "HGMMPipeline.h"
#include <QString>
#include <gst/videotestsrc/gstvideotestsrc.h>

#define C_STR(x) x.toStdString().c_str()

// default constructor
HGMMPipeline::HGMMPipeline(QObject *parent)
: QObject(parent)
, m_id(-999)
, m_pStimulusLayoutType(&HStimulusLayoutType::HStimulusLayoutUnknown)
, m_root("/")
, m_bISS(false)
, m_pipeline()
{}

HGMMPipeline::HGMMPipeline(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, HVideoWidget *pCenter, bool bISS, QObject *parent)
: QObject(parent)
, m_id(id)
, m_pStimulusLayoutType(&HStimulusLayoutType::HStimulusLayoutSingle)
, m_stimulus(stimulus)
, m_root(stimRoot)
, m_bISS(bISS)
{
	// create the pipeline itself
	m_pipeline = gst_pipeline_new(C_STR(makeElementName("pipeline", HPlayerPositionType::Control, m_id)));
	m_mapPipelineData[HPlayerPositionType::Center].pipeline = m_pipeline;

	// create PipelineData struct. Referencing it in the map will create it.
	m_mapPipelineData[HPlayerPositionType::Center].videoWidget = pCenter;

	// add media elements to pipeline
	addMedia(stimulus.getCenterStimulusInfo(), HPlayerPositionType::Center);

	// add bus listener. Handler for pad-added signal was added (if needed) in addMedia
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	gst_bus_add_watch(bus, &HGMMPipeline::busCallback, this);
	gst_object_unref(bus);

	// preroll...
	gst_element_set_state (m_pipeline, GST_STATE_PAUSED);
}

HGMMPipeline::HGMMPipeline(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, HVideoWidget *pLeft, HVideoWidget *pRight, bool bISS, QObject *parent)
: QObject(parent)
, m_id(id)
, m_pStimulusLayoutType(&HStimulusLayoutType::HStimulusLayoutLeftRight)
, m_stimulus(stimulus)
, m_root(stimRoot)
, m_bISS(bISS)
{
	// create the pipeline itself
	m_pipeline = gst_pipeline_new(C_STR(makeElementName("pipeline", HPlayerPositionType::Control, m_id)));

	// create PipelineData structs. Referencing it in the map will create them.
	m_mapPipelineData[HPlayerPositionType::Left].videoWidget = pLeft;
	m_mapPipelineData[HPlayerPositionType::Left].pipeline = m_pipeline;
	m_mapPipelineData[HPlayerPositionType::Right].videoWidget = pRight;
	m_mapPipelineData[HPlayerPositionType::Right].pipeline = m_pipeline;

	// add media elements to pipeline
	addMedia(stimulus.getLeftStimulusInfo(), HPlayerPositionType::Left);
	addMedia(stimulus.getRightStimulusInfo(), HPlayerPositionType::Right);

	// add bus listeners. Handler for pad-added signal was added (if needed) in addMedia
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
	gst_bus_add_watch(bus, &HGMMPipeline::busCallback, this);
	gst_object_unref(bus);

	// preroll...
	gst_element_set_state (m_pipeline, GST_STATE_PAUSED);
}

HGMMPipeline::~HGMMPipeline()
{
	gst_element_set_state (m_pipeline, GST_STATE_NULL);
	gst_object_unref(m_pipeline);
}

void HGMMPipeline::addMedia(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt)
{
	m_mapPipelineData[ppt].isLoop = info.isLoopPlayBack();

	if (info.isColor())
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

void HGMMPipeline::play()
{
	gst_element_set_state (m_pipeline, GST_STATE_PLAYING);

	//GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS("pipeline", GST_DEBUG_GRAPH_SHOW_ALL, "pipeline")
}

void HGMMPipeline::pause()
{
	if (m_mapPipelineData.count() == 1)
	{
		m_mapPipelineData.first().isLoopingStarted = false;
	}
	gst_element_set_state (m_pipeline, GST_STATE_PAUSED);
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
	    g_object_set_property(G_OBJECT(it.value().sink), "widget", &v);
	}
}


// set the "widget" property for each sink to its respective widget value
void HGMMPipeline::attachWidgetsToSinks()
{
	GValue v = G_VALUE_INIT;
	g_value_init(&v, G_TYPE_POINTER);
	QMapIterator<HPlayerPositionType, PipelineData> it(m_mapPipelineData);
	while (it.hasNext())
	{
	    it.next();
	    g_value_set_pointer(&v, it.value().videoWidget);
	    g_object_set_property(G_OBJECT(it.value().sink), "widget", &v);
	    it.value().videoWidget->setStimulusSize(it.value().size);
	}
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

gboolean HGMMPipeline::busCallback(GstBus *bus, GstMessage *msg, gpointer pdata)
{
	bool bLoop = false;
	GstState state, old_state;
	QString factoryName;
	int id;
	const HPlayerPositionType* pppt;
	HGMMPipeline *pPipeline = (HGMMPipeline *)pdata;
	//QMap<HPlayerPositionType, PipelineData> *pmap = (QMap<HPlayerPositionType, PipelineData> *)pdata;
	//QMapIterator<HPlayerPositionType, PipelineData> it(*pmap);

//	PipelineData *pPipelineData = (PipelineData *)pdata;
//    g_print("MESSAGE, msg = %s from %s\n",
//      gst_message_type_get_name(GST_MESSAGE_TYPE(msg)),
//	  GST_MESSAGE_SRC_NAME(msg));

    // we only care about certain messages here. For those of interest, parse the element name so
    // we can fetch the PipelineData and act accordingly.

	switch(GST_MESSAGE_TYPE(msg))
	{
	case GST_MESSAGE_ASYNC_DONE:
		// Check if we have video and if it is slated to loop
		// One quirk: looping is only implemented for single stimulus
		if (pPipeline->getPipelineDataMap().count() == 1)
		{
			PipelineData& pdata = pPipeline->getPipelineDataMap().first();
			if (pdata.isVideo && pdata.isLoop && !pdata.isLoopingStarted)
			{
		    		qDebug() << "Got ASYNC_DONE on " << GST_MESSAGE_SRC_NAME(msg) << " do flushing segment seek on pipeline";
		    		// do flushing seek GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT
				if (!gst_element_seek(pdata.pipeline, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
				{
					qCritical() << "SEEK FAILED";
				}
				else
				{
					pdata.isLoopingStarted = true;	// so we know we've done this already
				}
			}
		}
		break;
	case GST_MESSAGE_SEGMENT_DONE:
		if (pPipeline->getPipelineDataMap().count() == 1)
		{
			PipelineData& pdata = pPipeline->getPipelineDataMap().first();
			if (pdata.isVideo && pdata.isLoop)
			{
				// do non-flushing seek GST_SEEK_FLAG_SEGMENT
				qDebug() << "Got SEGMENT_DONE on " << GST_MESSAGE_SRC_NAME(msg) << " do flushing segment seek on pipeline";
				if (!gst_element_seek(pdata.pipeline, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
				{
					g_print("SEEK FAILED\n");
				}
			}
		}
		break;
	case GST_MESSAGE_STATE_CHANGED:
		gst_message_parse_state_changed(msg, &old_state, &state, NULL);
		if (state == GST_STATE_PLAYING &&
				parseElementName(GST_MESSAGE_SRC_NAME(msg), factoryName, pppt, id) &&
				factoryName == "pipeline")
		{
			qDebug() << GST_MESSAGE_SRC_NAME(msg) << " - " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(state);
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
				//GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(pPipelineData->pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "movie");
			}
			else qCritical() << "negative numerator?";
		}
		else qCritical() << "framerate not in caps or unexpected value type";
	}
	else if (g_str_has_prefix(s_new_pad_caps, "audio/x-raw"))
	{
		qDebug() << "Audio src - TODO";
	}
	// cleanup
	gst_caps_unref(new_pad_caps);
	return;
}


