/*
 * HGMMHelper.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: dan
 */

#include "HGMMHelper.h"
#include <QString>
#include <QStringList>
#include <QMapIterator>

#include <QThread>
#include <QApplication>

#include <QGst/ElementFactory>
#include <QGst/Pad>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/Event>
#include <QGst/Caps>
#include <gst/videotestsrc/gstvideotestsrc.h>

#define SPECIALPLAYHACK 1

QString HGMMHelper::makeElementName(const char *factoryName, const HPlayerPositionType& ppt, int number)
{
	QString format("%1-%2-%3");
	return format.arg(QString(factoryName)).arg(ppt.name()).arg(number);
}

QGst::ElementPtr HGMMHelper::makeElement(const char *factoryName, const HPlayerPositionType& ppt, int number)
{
	QString elementName = makeElementName(factoryName, ppt, number);
	QGst::ElementPtr element = QGst::ElementFactory::make(factoryName, elementName.toStdString().c_str());
	return element;
}

QString HGMMHelper::stateName(QGst::State s)
{
	QString name("UNKNOWN");
	switch (s)
	{
	case 0:
		name = QString("StateVoidPending");
		break;
	case 1:
		name = QString("StateNull");
		break;
	case 2:
		name = QString("StateReady");
		break;
	case 3:
		name = QString("StatePaused");
		break;
	case 4:
		name = QString("StatePlaying");
		break;
	}
	return name;
}

// default constructor
HGMMHelper::HGMMHelper(QObject *parent)
: QObject(parent)
, m_id(-999)
, m_pStimulusLayoutType(&HStimulusLayoutType::HStimulusLayoutUnknown)
, m_root("/")
, m_bISS(false)
, m_pipeline()
{}

HGMMHelper::HGMMHelper(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, QGst::Ui::VideoWidget *pCenter, bool bISS, QObject *parent)
: QObject(parent)
, m_id(id)
, m_pStimulusLayoutType(&HStimulusLayoutType::HStimulusLayoutSingle)
, m_stimulus(stimulus)
, m_root(stimRoot)
, m_bISS(bISS)
{
	// create pipeline
	m_pipeline = QGst::Pipeline::create();

	// watch pipeline bus for messages
    QGst::BusPtr bus = m_pipeline->bus();
    bus->addSignalWatch();
    QGlib::connect(bus, "message", this, &HGMMHelper::onBusMessage);

    // set up signal/slot for padAdded.
    connect(this, SIGNAL(handlePadAdded(int)), this, SLOT(doPadAdded(int)), Qt::QueuedConnection);

    // assign stimulus
	setStimulus(stimulus.getCenterStimulusInfo(), pCenter, HPlayerPositionType::Center);
	m_pipeline->setState(QGst::StatePaused);
}

HGMMHelper::HGMMHelper(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, QGst::Ui::VideoWidget *pLeft, QGst::Ui::VideoWidget *pRight, bool bISS, QObject *parent)
: QObject(parent)
, m_id(id)
, m_pStimulusLayoutType(&HStimulusLayoutType::HStimulusLayoutLeftRight)
, m_stimulus(stimulus)
, m_root(stimRoot)
, m_bISS(bISS)
{
	// create pipeline
	m_pipeline = QGst::Pipeline::create();

	// watch pipeline bus for messages
    QGst::BusPtr bus = m_pipeline->bus();
    bus->addSignalWatch();
    QGlib::connect(bus, "message", this, &HGMMHelper::onBusMessage);

    // assign stimuli
	setStimulus(stimulus.getLeftStimulusInfo(), pLeft, HPlayerPositionType::Left);
	setStimulus(stimulus.getRightStimulusInfo(), pRight, HPlayerPositionType::Right);
	m_pipeline->setState(QGst::StatePaused);
}

HGMMHelper& HGMMHelper::operator=(const HGMMHelper& rhs)
{
	if (&rhs == this)
		return *this;

	setParent(rhs.parent());
	m_id = rhs.id();
	m_pStimulusLayoutType = rhs.m_pStimulusLayoutType;
	m_bISS = rhs.m_bISS;
	m_pipeline = rhs.m_pipeline;
	return *this;
}

bool HGMMHelper::hasPads() const
{
	qDebug() << "TODO HAS PADS";
	return false;
}

int HGMMHelper::id() const
{
	return m_id;
}

QGst::ElementPtr HGMMHelper::sink(const HPlayerPositionType& ppt) const
{
	QGst::ElementPtr sink;
	if (m_mapPaths.contains(ppt))
		sink = m_mapPaths[ppt].sink;
	return sink;
}

void HGMMHelper::padAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
{
	QString senderName = sender->property("name").get<QString>();
	//qDebug() << "padAdded (" << senderName << ") src CAPS: " << srcPad->currentCaps()->toString();
	qDebug() << "padAdded - main thread? " << (QThread::currentThread() == QApplication::instance()->thread());
	const HPlayerPositionType& ppt = getPPTFromElementName(sender->property("name").get<QString>());
	Q_ASSERT(ppt != HPlayerPositionType::UnknownPlayerPositionType);

	// audio or video
	QString caps = srcPad->currentCaps()->toString();
	if (caps.startsWith(QString("video/x-raw")))
	{
		qDebug() << "padAdded (" << senderName << ") This is a video src pad";

		// get resolution
		QGst::StructurePtr structure = srcPad->currentCaps()->internalStructure(0);
		m_mapPaths[ppt].size.setHeight(structure->value("height").toInt());
		m_mapPaths[ppt].size.setWidth(structure->value("width").toInt());

		// get frame rate
		QGlib::Value fr = structure->value("framerate");
		QString sfr = fr.toString();
		QStringList slfr = sfr.split("/");
		bool hasFramerate = false;
		bool isImage = false;
		if (slfr.size() == 2)
		{
			bool bOK;
			int n = slfr[0].toInt(&bOK);
			if (bOK)
			{
				isImage = (n==0);
				hasFramerate = true;
			}
		}
		m_mapPaths[ppt].isImage = isImage;
		m_mapPaths[ppt].isVideo = !isImage;
		m_mapPaths[ppt].isAudio = false;
		m_mapPaths[ppt].srcPad = srcPad;
		qDebug() << "padAdded (" << senderName << ") framerate " << sfr << " has framerate? " << hasFramerate << " isImage? " << isImage;

#ifndef CREATE_BEFORE_PAD_ADDED
		Q_EMIT handlePadAdded(ppt.number());
#else
		// elements are created and saved in m_mapPaths[ppt]
		if (isImage)
		{
			pipeline()->add(m_mapPaths[ppt].convert, m_mapPaths[ppt].imagefreeze, m_mapPaths[ppt].sink);
			srcPad->link(m_mapPaths[ppt].convert->getStaticPad("sink"));
			m_mapPaths[ppt].convert->link(m_mapPaths[ppt].imagefreeze);
			m_mapPaths[ppt].imagefreeze->link(m_mapPaths[ppt].sink);
		}
		else
		{
			pipeline()->add(m_mapPaths[ppt].convert, m_mapPaths[ppt].identity, m_mapPaths[ppt].sink);
			srcPad->link(m_mapPaths[ppt].convert->getStaticPad("sink"));
			m_mapPaths[ppt].convert->link(m_mapPaths[ppt].identity);
			m_mapPaths[ppt].identity->link(m_mapPaths[ppt].sink);
		}
#endif
	}
	else if (caps.startsWith(QString("audio/x-raw")))
	{
		qDebug() << "padAdded (" << senderName << ") This is an audio src pad. TODO - DEAL WITH THIS PAD";
		m_mapPaths[ppt].isImage = false;
		m_mapPaths[ppt].isVideo = false;
		m_mapPaths[ppt].isAudio = true;
		m_mapPaths[ppt].srcPad = srcPad;

#ifndef CREATE_BEFORE_PAD_ADDED
		Q_EMIT handlePadAdded(ppt.number());
#endif

	}
	else
	{
		qDebug() << "padAdded (" << senderName << ") UNKNOWN CAPS TYPE IN HGMMHelper::padAdded";
	}
}


void HGMMHelper::doPadAdded(int ippt)
{
	const HPlayerPositionType& ppt = getPlayerPositionType(ippt);
	qDebug() << "doPadAdded - " << ppt.name() << " main thread? " << (QThread::currentThread() == QApplication::instance()->thread());

	if (m_mapPaths[ppt].isVideo)
	{

		QGst::ElementPtr convert = makeElement("videoconvert", ppt, id());
		Q_ASSERT(convert);

		QGst::ElementPtr identity = makeElement("identity", ppt, id());
		Q_ASSERT(identity);
		identity->setProperty("single-segment", true);

//			QGst::ElementPtr scale = makeElement("videoscale", ppt, id());
//			Q_ASSERT(scale);
//
//			m_mapPaths[ppt].capsfilter = makeElement("capsfilter", ppt, id());
//			Q_ASSERT(m_mapPaths[ppt].capsfilter);
//
		m_mapPaths[ppt].sink = makeElement("qwidget5videosink", ppt, id());
		Q_ASSERT(m_mapPaths[ppt].sink);


//			m_pipeline->add(convert, identity, scale, m_mapPaths[ppt].capsfilter, m_mapPaths[ppt].sink);
		m_pipeline->add(convert, identity, m_mapPaths[ppt].sink);

		// start linking
		m_mapPaths[ppt].srcPad->link(convert->getStaticPad("sink"));
		convert->link(identity);
		identity->link(m_mapPaths[ppt].sink);
	}
	else if (m_mapPaths[ppt].isImage)
	{

		QGst::ElementPtr convert = makeElement("autovideoconvert", ppt, id());
		Q_ASSERT(convert);

		QGst::ElementPtr imagefreeze = makeElement("imagefreeze", ppt, id());
		Q_ASSERT(imagefreeze);

		m_mapPaths[ppt].sink = makeElement("qwidget5videosink", ppt, id());
		Q_ASSERT(m_mapPaths[ppt].sink);

		m_pipeline->add(convert, imagefreeze, m_mapPaths[ppt].sink);
		m_mapPaths[ppt].srcPad->link(convert->getStaticPad("sink"));
		convert->link(imagefreeze);
		imagefreeze->link(m_mapPaths[ppt].sink);
	}
	else if (m_mapPaths[ppt].isAudio)
	{
		qDebug() << "doPadAdded: AUDIO PAD TODO TODO TODO";
	}


}
// The entry in m_mapPaths is created here.
void HGMMHelper::setStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget, const HPlayerPositionType& ppt)
{
	m_mapPaths[ppt].isLoop = info.isLoopPlayBack();
	m_mapPaths[ppt].videoWidget = pwidget;

	if (info.isColor())
	{
		setSolidColorStimulus(info.getColor(), ppt);
		m_mapPaths[ppt].needPad = false;
	}
	else if (info.isJPG())
	{
		setUnknownStimulus(info, ppt);
//		setJPGStimulus(info, ppt);
		m_mapPaths[ppt].needPad = false;
	}
	else if (info.isPNG())
	{
		setUnknownStimulus(info, ppt);
//		setPNGStimulus(info, ppt);
		m_mapPaths[ppt].needPad = false;
	}
	else
	{
		setUnknownStimulus(info, ppt);
		m_mapPaths[ppt].needPad = true;
	}
}


void HGMMHelper::setUnknownStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt)
{
	// Video has filesrc!decodebin. Video pad will go directly into the input selector.
	QString filename = info.getAbsoluteFileName(m_root);
	qDebug() << "HGMMHelper::setUnknownStimulus " << info.getFileName() << "   filename " << filename;
	qDebug() << "HGMMHelper::setUnknownStimulus - main thread? " << (QThread::currentThread() == QApplication::instance()->thread());


	QGst::ElementPtr filesrc = makeElement("filesrc", ppt, id());
	Q_ASSERT(filesrc);
	filesrc->setProperty("location", filename);

	QGst::ElementPtr decodebin = makeElement("decodebin", ppt, id());
	Q_ASSERT(decodebin);

	m_pipeline->add(filesrc, decodebin);
	filesrc->link(decodebin);	// src pad on decodebin will be connected to 'lnkToPad' input arg.

#ifdef CREATE_BEFORE_PAD_ADDED

	// Create elements that may be needed in padAdded.
	// Elements must be created in main thread
	m_mapPaths[ppt].convert = makeElement("autovideoconvert", ppt, id());
	Q_ASSERT(m_mapPaths[ppt].convert);

	// for images
	m_mapPaths[ppt]. imagefreeze = makeElement("imagefreeze", ppt, id());
	Q_ASSERT(m_mapPaths[ppt].imagefreeze);

	// for movies
	m_mapPaths[ppt].identity = makeElement("identity", ppt, id());
	Q_ASSERT(m_mapPaths[ppt].identity);
	m_mapPaths[ppt].identity->setProperty("single-segment", true);

	m_mapPaths[ppt].sink = makeElement("qwidget5videosink", ppt, id());
	Q_ASSERT(m_mapPaths[ppt].sink);

#endif

	QGlib::connect(decodebin, "pad_added", this, &HGMMHelper::padAdded, QGlib::PassSender);


#if 0
	sink = QGst::ElementFactory::make("qwidget5videosink", sinkName.toStdString().c_str());
	Q_ASSERT(sink);
	sink->setProperty("force-aspect-ratio", true);

	linkToElement = QGst::ElementFactory::make("videoconvert", convertName.toStdString().c_str());
	if (!linkToElement)
	{
		qWarning() << "CAnnot make videoconvert element";
	}
	QGst::ElementPtr identity = QGst::ElementFactory::make("identity", identityName.toStdString().c_str());
	identity->setProperty("single-segment", true);
	m_pipeline->add(filesrc, decodebin, linkToElement, identity, sink);
	filesrc->link(decodebin);	// src pad on decodebin will be connected to 'lnkToPad' input arg.
	linkToElement->link(identity);
	identity->link(sink);
	QGlib::connect(decodebin, "pad_added", this, func, QGlib::PassSender);
	// one last thing - add probe to filesrc src pad looking for EOS
	QGst::PadPtr fileSrcPad = filesrc->getStaticPad("src");
	QGst::PadPtr decodeSinkPad = decodebin->getStaticPad("sink");
	QGst::PadPtr sinkPad = sink->getStaticPad("sink");
	QGst::PadPtr convertSinkPad = linkToElement->getStaticPad("sink");

	// static_cast<GstPadProbeType>(static_cast<int>(GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM) | static_cast<int>(GST_PAD_PROBE_TYPE_BLOCK));}

	// works on convertSinkPad
	// does not work on filesrc src pad
	// does not work on decodebin sink pad
	unsigned int probeid = convertSinkPad->addProbe(GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HGMMHelper::onEvent, this);
	//unsigned int probeid = decodeSinkPad->addProbe(GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &HGMMHelper::onEvent, this);
//			static_cast<GstPadProbeType>(static_cast<int>(GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM) | static_cast<int>(GST_PAD_PROBE_TYPE_BLOCK)),
//			&HGMMHelper::onEOSEvent, this);
	qDebug() << "got probe id " << probeid;
#endif
}


void HGMMHelper::play()
{

	// Do something special when movies
	QMapIterator<HPlayerPositionType, HelperPath> it(m_mapPaths);
	while (it.hasNext())
	{
		it.next();
//		if (it.value().isLoop)
		if (it.value().sink)
		{
			qDebug() << "flushing seek play (loop=" << it.value().isLoop << ") for " << it.key().name();
			QGst::SeekEventPtr seekEv = QGst::SeekEvent::create(
			   1.0,
			   QGst::FormatTime,
			   QGst::SeekFlagSegment|QGst::SeekFlagFlush,
			   QGst::SeekTypeSet, 0,
			   QGst::SeekTypeNone, GST_CLOCK_TIME_NONE);
			it.value().sink->sendEvent(seekEv);
		}
	}

	m_pipeline->setState(QGst::StatePlaying);

	//GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS("pipeline", GST_DEBUG_GRAPH_SHOW_ALL, "pipeline")
}

const HPlayerPositionType& HGMMHelper::getPPTFromElementName(const QString& elementName)
{
	QStringList l = elementName.toLower().split('-');
	if (l.size() == 3)
	{
		if (l[1] == QString("left"))
			return HPlayerPositionType::Left;
		else if (l[1] == QString("right"))
			return HPlayerPositionType::Right;
		else if (l[1] == QString("center"))
			return HPlayerPositionType::Center;
	}
	return HPlayerPositionType::UnknownPlayerPositionType;
}


// this is a static func
GstPadProbeReturn HGMMHelper::onEvent(
		GstPad          *pad,
        GstPadProbeInfo *info,
		gpointer         user_data)
{
	HGMMHelper *phelper = (HGMMHelper *)user_data;
	GstEvent *event = GST_PAD_PROBE_INFO_EVENT(info);
	GstElement* padParentElement = gst_pad_get_parent_element(pad);
	gchar *parentName = gst_element_get_name(padParentElement);
	QString sParentName(parentName);
//	qDebug() << "Got event: " << GST_EVENT_TYPE_NAME (event) << " at element " << sParentName;
	g_free(parentName);
	gst_object_unref(padParentElement);
	const HPlayerPositionType& ppt = getPPTFromElementName(sParentName);
	Q_ASSERT(ppt != HPlayerPositionType::UnknownPlayerPositionType);
	Q_ASSERT(phelper->m_mapPaths.contains(ppt));
	HelperPath *phelperPath = &phelper->m_mapPaths[ppt];

	//qDebug() << "id from info " << GST_PAD_PROBE_INFO_ID (info) << " type " << info->type << " data? " << (info->data ? "yes": "NULL");
	if (GST_EVENT_TYPE (event) == GST_EVENT_EOS)
	{
   		qDebug() << "onEOSEvent got EOS from " << sParentName;

#ifdef SPECIALPLAYHACK
		QGst::SeekEventPtr seekEv = QGst::SeekEvent::create(
	       1.0,
	       QGst::FormatTime,
	       QGst::SeekFlagSegment,
	       QGst::SeekTypeSet, 0,
	       QGst::SeekTypeNone, GST_CLOCK_TIME_NONE);

		if (phelperPath->isLoop)
		{
			if (phelperPath->sink)
			{
				qDebug() << "send seek event to sink";
				phelperPath->sink->sendEvent(seekEv);
				return GST_PAD_PROBE_DROP;
			}
			else
			{
				qDebug() << "send seek event to pipeline";
				phelper->m_pipeline->sendEvent(seekEv);
			}
		}
#else
		return GST_PAD_PROBE_PASS;
#endif
	}
	else if (GST_EVENT_TYPE (event) == GST_EVENT_SEGMENT_DONE)
	{
   		qDebug() << "onEOSEvent got SEGMENT_DONE from " << sParentName;
   		// send seek here, to filesrc
   		// make it a segment seek
   		//
   		// gst_element_seek(pipeline, 1, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_END, 0);

#ifdef SPECIALPLAYHACK
		QGst::SeekEventPtr seekEv = QGst::SeekEvent::create(
	       1.0,
	       QGst::FormatTime,
	       QGst::SeekFlagSegment,
	       QGst::SeekTypeSet, 0,
	       QGst::SeekTypeNone, GST_CLOCK_TIME_NONE);

		// if looped source, send seek event
		if (phelperPath->isLoop)
		{
			if (phelperPath->sink)
			{
				qDebug() << "send seek event to sink";
				phelperPath->sink->sendEvent(seekEv);
				return GST_PAD_PROBE_DROP;
			}
			else
			{
				qDebug() << "send seek event to pipeline";
				phelper->pipeline()->sendEvent(seekEv);
			}
		}
#else
		return GST_PAD_PROBE_PASS;
#endif
	}
	else if (GST_EVENT_TYPE (event) == GST_EVENT_CAPS)
	{
		GstCaps *pcaps = NULL;
		gst_event_parse_caps(event, &pcaps);
		qDebug() << "CAPS event " << gst_caps_to_string(pcaps);
	}
	return GST_PAD_PROBE_PASS;

//	  gst_pad_remove_probe (pad, GST_PAD_PROBE_INFO_ID (info));

//	return GST_PAD_PROBE_OK;
}


void HGMMHelper::setSolidColorStimulus(const QColor& color, const HPlayerPositionType& ppt)
{
	m_mapPaths[ppt].sink = makeElement("qwidget5videosink", ppt, id());
	Q_ASSERT(m_mapPaths[ppt].sink);

	QString sDescription = QString("videotestsrc pattern=solid-color foreground-color=%1 name=%2").arg(color.rgba()).arg(makeElementName("videotestsrc", ppt, id()));
	qDebug() << "solid color descr: "<< sDescription;
	QGst::BinPtr videotestsrc = QGst::Bin::fromDescription(sDescription);


//	QGst::ElementPtr videotestsrc = makeElement("videotestsrc", ppt, id());
	Q_ASSERT(videotestsrc);
	//videotestsrc->setProperty("pattern", QString("solid-color"));
//	videotestsrc->setProperty<GstVideoTestSrcPattern>("pattern", GST_VIDEO_TEST_SRC_SOLID);
//	videotestsrc->setProperty("foreground-color", QString("%1").arg(color.rgba()));

	qDebug() << "TODO - setSolidColorStimulus - need additional elements";

	m_pipeline->add(videotestsrc, m_mapPaths[ppt].sink);
	videotestsrc->link(m_mapPaths[ppt].sink);
}

//void HGMMHelper::setSoundStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget)
//{
//
//}

#if 0
void HGMMHelper::setJPGStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt)
{
	// jpg has filesrc!jpegdec!imagefreeze. Video pad will go directly into the input selector.
	QString filename = info.getAbsoluteFileName(m_root);
	qDebug() << "HGMMHelper::setJPGStimulus " << info.getFileName() << "   filename " << filename;

	QGst::ElementPtr filesrc = makeElement("filesrc", ppt, id());
	Q_ASSERT(filesrc);
	filesrc->setProperty("location", filename);

	QGst::ElementPtr jpegdec = makeElement("jpegdec", ppt, id());
	Q_ASSERT(jpegdec);

	QGst::ElementPtr imagefreeze = makeElement("imagefreeze", ppt, id());
	Q_ASSERT(imagefreeze);

	QGst::ElementPtr convert = makeElement("autovideoconvert", ppt, id());
	Q_ASSERT(convert);

	QGst::ElementPtr scale = makeElement("videoscale", ppt, id());
	Q_ASSERT(scale);

	m_mapPaths[ppt].capsfilter = makeElement("capsfilter", ppt, id());
	Q_ASSERT(m_mapPaths[ppt].capsfilter);

	m_mapPaths[ppt].sink = makeElement("qwidget5videosink", ppt, id());
	Q_ASSERT(m_mapPaths[ppt].sink);

	m_pipeline->add(filesrc, jpegdec, imagefreeze, convert, scale, m_mapPaths[ppt].capsfilter, m_mapPaths[ppt].sink);
	filesrc->link(jpegdec);
	jpegdec->link(imagefreeze);
	imagefreeze->link(convert);
	convert->link(scale);
	scale->link(m_mapPaths[ppt].capsfilter);
	m_mapPaths[ppt].capsfilter->link(m_mapPaths[ppt].sink);
}

void HGMMHelper::setPNGStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt)
{
	// png has filesrc!pngdec!imagefreeze.
	// jpg has filesrc!jpegdec!imagefreeze. Video pad will go directly into the input selector.
	QString filename = info.getAbsoluteFileName(m_root);
	qDebug() << "HGMMHelper::setJPGStimulus " << info.getFileName() << "   filename " << filename;

	QGst::ElementPtr filesrc = makeElement("filesrc", ppt, id());
	Q_ASSERT(filesrc);
	filesrc->setProperty("location", filename);

	QGst::ElementPtr pngdec = makeElement("pngdec", ppt, id());
	Q_ASSERT(pngdec);

	QGst::ElementPtr imagefreeze = makeElement("imagefreeze", ppt, id());
	Q_ASSERT(imagefreeze);

	QGst::ElementPtr convert = makeElement("autovideoconvert", ppt, id());
	Q_ASSERT(convert);

	QGst::ElementPtr scale = makeElement("videoscale", ppt, id());
	Q_ASSERT(scale);

	m_mapPaths[ppt].capsfilter = makeElement("capsfilter", ppt, id());
	Q_ASSERT(m_mapPaths[ppt].capsfilter);

	m_mapPaths[ppt].sink = makeElement("qwidget5videosink", ppt, id());
	Q_ASSERT(m_mapPaths[ppt].sink);

	m_pipeline->add(filesrc, pngdec, imagefreeze, convert, scale, m_mapPaths[ppt].capsfilter, m_mapPaths[ppt].sink);
	filesrc->link(pngdec);
	pngdec->link(imagefreeze);
	imagefreeze->link(convert);
	convert->link(scale);
	scale->link(m_mapPaths[ppt].capsfilter);
	m_mapPaths[ppt].capsfilter->link(m_mapPaths[ppt].sink);
}
#endif

void HGMMHelper::handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm)
{
	//qInfo() << "HGMMPlayer::handlePipelineStateChange: " << stateName(scm->oldState()) << "-->" << stateName(scm->newState());
    switch (scm->newState()) {
    case QGst::StatePlaying:
        //start the timer when the pipeline starts playing
    		//qDebug() << "NOW PLAYIng!";
    		Q_EMIT(nowPlaying());
        break;
    case QGst::StatePaused:
        //stop the timer when the pipeline pauses
    		//qInfo() << "QGst::StatePaused";
        break;
    default:
    		//qInfo() << "QGst::State???";
        break;
    }
}

void HGMMHelper::onBusMessage(const QGst::MessagePtr & message)
{
	//qDebug() << "HGMMHelper::onBusMessage(" << id() << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
	if (message->source().dynamicCast<QGst::Pipeline>() != m_pipeline)
	{
		return;
	}
	QGst::SeekingQueryPtr query;
	QGst::SeekEventPtr seekEv;

	switch (message->type())
    {
    case QGst::MessageEos: //End of stream. We reached the end of the file.
    		//qDebug() << "HGMMHelper::onBusMessage(" << id() << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
        break;
    case QGst::MessageError: //Some error occurred.
    		qDebug() << "HGMMHelper::onBusMessage(" << id() << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
    		qCritical() << "Got error, message is:";
        qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
        break;
    case QGst::MessageStateChanged: //The element in message->source() has changed state
    		//qDebug() << "HGMMHelper::onBusMessage(" << id() << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
    		handlePipelineStateChange(message.staticCast<QGst::StateChangedMessage>());
        break;
    case QGst::MessageAsyncDone:
    		//ClockTime ct = message.staticCast<QGst::AsyncDoneMessage>()->runningTime();
    		//qDebug() << "HGMMHelper::onBusMessage(" << id() << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
    		//qDebug() << "AsyncDone, running time " <<
    		//	message.staticCast<QGst::AsyncDoneMessage>()->runningTime() <<
		//		" , source " <<
		//		message.staticCast<QGst::AsyncDoneMessage>()->source()->name();
    		break;
    default:
    		//qDebug() << "HGMMHelper::onBusMessage(" << id() << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
    		//qDebug() << "Unhandled msg: " << message->typeName() << " type " << message->type();
        break;
    }
}




#if 0
       case GST_MESSAGE_EOS:
            /* restart playback if at end */
            if (!gst_element_seek(play,
                        1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                        GST_SEEK_TYPE_SET, 0,
                        GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
                g_print("Seek failed!\n");
#endif


#if 0

                typedef enum
                {
                  GST_MESSAGE_UNKNOWN           = 0,
              1    GST_MESSAGE_EOS               = (1 << 0),
              2    GST_MESSAGE_ERROR             = (1 << 1),
              4    GST_MESSAGE_WARNING           = (1 << 2),
              8    GST_MESSAGE_INFO              = (1 << 3),
              16    GST_MESSAGE_TAG               = (1 << 4),
              32    GST_MESSAGE_BUFFERING         = (1 << 5),
              64    GST_MESSAGE_STATE_CHANGED     = (1 << 6),
              128    GST_MESSAGE_STATE_DIRTY       = (1 << 7),
              256    GST_MESSAGE_STEP_DONE         = (1 << 8),
              512    GST_MESSAGE_CLOCK_PROVIDE     = (1 << 9),
              1024    GST_MESSAGE_CLOCK_LOST        = (1 << 10),
              2048    GST_MESSAGE_NEW_CLOCK         = (1 << 11),
              4096    GST_MESSAGE_STRUCTURE_CHANGE  = (1 << 12),
              8192    GST_MESSAGE_STREAM_STATUS     = (1 << 13),
              16384    GST_MESSAGE_APPLICATION       = (1 << 14),
              32768    GST_MESSAGE_ELEMENT           = (1 << 15),
              65536    GST_MESSAGE_SEGMENT_START     = (1 << 16),
              131072    GST_MESSAGE_SEGMENT_DONE      = (1 << 17),
              262144    GST_MESSAGE_DURATION_CHANGED  = (1 << 18),
              524288    GST_MESSAGE_LATENCY           = (1 << 19),
              1048576    GST_MESSAGE_ASYNC_START       = (1 << 20),
              2097152    GST_MESSAGE_ASYNC_DONE        = (1 << 21),
              4194304    GST_MESSAGE_REQUEST_STATE     = (1 << 22),
              8388608    GST_MESSAGE_STEP_START        = (1 << 23),
              16777216    GST_MESSAGE_QOS               = (1 << 24),
              33554432    GST_MESSAGE_PROGRESS          = (1 << 25),
              67108864    GST_MESSAGE_TOC               = (1 << 26),
              134217728    GST_MESSAGE_RESET_TIME        = (1 << 27),
                  GST_MESSAGE_STREAM_START      = (1 << 28),
                  GST_MESSAGE_NEED_CONTEXT      = (1 << 29),
                  GST_MESSAGE_HAVE_CONTEXT      = (1 << 30),
                  GST_MESSAGE_EXTENDED          = (gint) (1u << 31),
                  GST_MESSAGE_DEVICE_ADDED      = GST_MESSAGE_EXTENDED + 1,
                  GST_MESSAGE_DEVICE_REMOVED    = GST_MESSAGE_EXTENDED + 2,
                  GST_MESSAGE_PROPERTY_NOTIFY   = GST_MESSAGE_EXTENDED + 3,
                  GST_MESSAGE_STREAM_COLLECTION = GST_MESSAGE_EXTENDED + 4,
                  GST_MESSAGE_STREAMS_SELECTED  = GST_MESSAGE_EXTENDED + 5,
                  GST_MESSAGE_REDIRECT          = GST_MESSAGE_EXTENDED + 6,
                  GST_MESSAGE_ANY               = (gint) (0xffffffff)
                } GstMessageType;


#endif
//void HGMMHelper::setRightStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget)
//{
//	if (info.isColor())
//	{
//		setSolidColorStimulus(info.getColor(), m_sinkRight, QString("right"));
//		m_bNeedPadRight = false;
//	}
//	else if (info.isJPG())
//	{
//		setJPGStimulus(info, m_sinkRight, QString("right"));
//		m_bNeedPadRight = false;
//	}
//	else if (info.isPNG())
//	{
//		setPNGStimulus(info, m_sinkRight, QString("right"));
//		m_bNeedPadRight = false;
//	}
//	else
//	{
//		setUnknownStimulus(info, m_sinkRight, m_linkToElementRight, &HGMMHelper::rightPadAdded, QString("right"));
//		m_bNeedPadRight = true;
//	}
//
//	HelperPath path = {
//			m_sinkRight,
//			m_linkToElementRight,
//			pwidget,
//			m_bNeedPadRight,
//			info.isLoopPlayBack() };
//	m_mapPaths.insert(HPlayerPositionType::Right, path);
//
//}
//
//void HGMMHelper::setCenterStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget)
//{
//	if (info.isColor())
//	{
//		setSolidColorStimulus(info.getColor(), m_sinkCenter, QString("center"));
//		m_bNeedPadCenter = false;
//	}
//	else if (info.isJPG())
//	{
//		setJPGStimulus(info, m_sinkCenter, QString("center"));
//		m_bNeedPadCenter = false;
//	}
//	else if (info.isPNG())
//	{
//		setPNGStimulus(info, m_sinkCenter, QString("center"));
//		m_bNeedPadCenter = false;
//	}
//	else
//	{
//		setUnknownStimulus(info, m_sinkCenter, m_linkToElementCenter, &HGMMHelper::centerPadAdded, QString("center"));
//		m_bNeedPadCenter = true;
//	}
//
//	HelperPath path = {
//			m_sinkCenter,
//			m_linkToElementCenter,
//			pwidget,
//			m_bNeedPadCenter,
//			info.isLoopPlayBack() };
//	m_mapPaths.insert(HPlayerPositionType::Center, path);
//}
//

//void HGMMHelper::setLeftSink(QGst::ElementPtr& leftSink)
//{
//	m_sinkLeft = leftSink;
//	m_bNeedPadLeft = false;
//}
//
//void HGMMHelper::setRightSink(QGst::ElementPtr& rightSink)
//{
//	m_sinkRight = rightSink;
//	m_bNeedPadRight = false;
//}
//
//void HGMMHelper::setCenterSink(QGst::ElementPtr& centerSink)
//{
//	m_sinkCenter = centerSink;
//	m_bNeedPadCenter = false;
//}
//

//void HGMMHelper::leftPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
//{
//	qDebug() << "leftPadAdded: sender obj " << sender->property("name").get<QString>();
//	qDebug() << "src CAPS: " << srcPad->currentCaps()->toString();
//
//	// audio or video
//	QString caps = srcPad->currentCaps()->toString();
//	if (caps.startsWith(QString("video/x-raw")))
//	{
//		qDebug() << "This is a video src pad";
//		if (!m_linkToElementLeft)
//		{
//			qWarning() << "m_linkToElementLeft is NULL";
//		}
//		else
//		{
//			qDebug() << "m_linkToElementLeft is OK";
//			QGst::PadPtr sinkPad = m_linkToElementLeft->getStaticPad("sink");
//			if (!sinkPad)
//			{
//				qWarning() << "cannot get sink pad from link to element left";
//				return;
//			}
//			else
//			{
//				qDebug() << "sinkPad is OK";
//				//qDebug() << "sink CAPS: : " << sinkPad->currentCaps()->toString();
//				QGst::PadLinkReturn ret  = srcPad->link(sinkPad);
//				if (ret != QGst::PadLinkOk) qDebug() << "PAD LINK FAIL " << (int)ret;
//				m_bNeedPadLeft = false;
//			}
//		}
//	}
//	else if (caps.startsWith(QString("audio/x-raw")))
//	{
//		qDebug() << "This is an audio src pad. TODO - DEAL WITH THIS PAD";
//	}
//}
//
//void HGMMHelper::rightPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
//{
//	qDebug() << "rightPadAdded: sender obj " << sender->property("name").get<QString>();
//	qDebug() << "src CAPS: " << srcPad->currentCaps()->toString();
//
//	// audio or video
//	QString caps = srcPad->currentCaps()->toString();
//	if (caps.startsWith(QString("video/x-raw")))
//	{
//		qDebug() << "This is a video src pad";
//		if (!m_linkToElementRight)
//		{
//			qWarning() << "m_linkRToElementRight is NULL";
//		}
//		else
//		{
//			qDebug() << "m_linkToElementRight is OK";
//			QGst::PadPtr sinkPad = m_linkToElementRight->getStaticPad("sink");
//			if (!sinkPad)
//			{
//				qWarning() << "cannot get sink pad from link to element right";
//				return;
//			}
//			else
//			{
//				qDebug() << "sinkPad is OK";
//				//qDebug() << "sink CAPS: : " << sinkPad->currentCaps()->toString();
//				QGst::PadLinkReturn ret  = srcPad->link(sinkPad);
//				if (ret != QGst::PadLinkOk) qDebug() << "PAD LINK FAIL " << (int)ret;
//				m_bNeedPadRight = false;
//			}
//		}
//	}
//	else if (caps.startsWith(QString("audio/x-raw")))
//	{
//		qDebug() << "This is an audio src pad. TODO - DEAL WITH THIS PAD";
//	}
//
//}
//
//void HGMMHelper::centerPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
//{
//	qDebug() << "centerPadAdded: sender obj " << sender->property("name").get<QString>();
//	qDebug() << "src CAPS: " << srcPad->currentCaps()->toString();
//
//
//	// audio or video
//	QString caps = srcPad->currentCaps()->toString();
//	if (caps.startsWith(QString("video/x-raw")))
//	{
//		qDebug() << "This is a video src pad";
//		if (!m_linkToElementCenter)
//		{
//			qWarning() << "m_linkRToElementCenter is NULL";
//		}
//		else
//		{
//			qDebug() << "m_linkToElementCenter is OK";
//			QGst::PadPtr sinkPad = m_linkToElementCenter->getStaticPad("sink");
//			if (!sinkPad)
//			{
//				qWarning() << "cannot get sink pad from link to element center";
//				return;
//			}
//			else
//			{
//				qDebug() << "sinkPad is OK";
//				//qDebug() << "sink CAPS: : " << sinkPad->currentCaps()->toString();
//				QGst::PadLinkReturn ret  = srcPad->link(sinkPad);
//				if (ret != QGst::PadLinkOk) qDebug() << "PAD LINK FAIL " << (int)ret;
//				m_bNeedPadCenter = false;
//			}
//		}
//	}
//	else if (caps.startsWith(QString("audio/x-raw")))
//	{
//		qDebug() << "This is an audio src pad. TODO - DEAL WITH THIS PAD";
//	}
//}
