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
#include <QGst/ElementFactory>
#include <QGst/Pad>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/Event>
#include <gst/videotestsrc/gstvideotestsrc.h>

#define SPECIALPLAYHACK 1


QString stateName(QGst::State s)
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
, m_sinkLeft(), m_sinkRight(), m_sinkCenter()
, m_bNeedPadLeft(true)
, m_bNeedPadRight(true)
, m_bNeedPadCenter(true)
{}

HGMMHelper::HGMMHelper(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, QGst::Ui::VideoWidget *pCenter, bool bISS, QObject *parent)
: QObject(parent)
, m_id(id)
, m_pStimulusLayoutType(&HStimulusLayoutType::HStimulusLayoutSingle)
, m_stimulus(stimulus)
, m_root(stimRoot)
, m_bISS(bISS)
, m_sinkLeft(), m_sinkRight(), m_sinkCenter()
, m_bNeedPadLeft(false)
, m_bNeedPadRight(false)
, m_bNeedPadCenter(true)
, m_linkToElementLeft()
, m_linkToElementRight()
, m_linkToElementCenter()
{
	// create pipeline
	m_pipeline = QGst::Pipeline::create();

	// watch pipeline bus for messages
    QGst::BusPtr bus = m_pipeline->bus();
    bus->addSignalWatch();
    QGlib::connect(bus, "message", this, &HGMMHelper::onBusMessage);

    // assign stimulus
	setCenterStimulus(stimulus.getCenterStimulusInfo(), pCenter);
}

HGMMHelper::HGMMHelper(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, QGst::Ui::VideoWidget *pLeft, QGst::Ui::VideoWidget *pRight, bool bISS, QObject *parent)
: QObject(parent)
, m_id(id)
, m_pStimulusLayoutType(&HStimulusLayoutType::HStimulusLayoutLeftRight)
, m_stimulus(stimulus)
, m_root(stimRoot)
, m_bISS(bISS)
, m_sinkLeft(), m_sinkRight(), m_sinkCenter()
, m_bNeedPadLeft(true)
, m_bNeedPadRight(true)
, m_bNeedPadCenter(false)
, m_linkToElementLeft()
, m_linkToElementRight()
, m_linkToElementCenter()
{
	// create pipeline
	m_pipeline = QGst::Pipeline::create();

	// watch pipeline bus for messages
    QGst::BusPtr bus = m_pipeline->bus();
    bus->addSignalWatch();
    QGlib::connect(bus, "message", this, &HGMMHelper::onBusMessage);

    // assign stimuli
	setLeftStimulus(stimulus.getLeftStimulusInfo(), pLeft);
	setRightStimulus(stimulus.getRightStimulusInfo(), pRight);
}

HGMMHelper& HGMMHelper::operator=(const HGMMHelper& rhs)
{
	if (&rhs == this)
		return *this;

	setParent(rhs.parent());
	m_id = rhs.m_id;
	m_pStimulusLayoutType = rhs.m_pStimulusLayoutType;
	m_bISS = rhs.m_bISS;
	m_pipeline = rhs.m_pipeline;
	m_sinkLeft = rhs.m_sinkLeft;
	m_sinkRight = rhs.m_sinkRight;
	m_sinkCenter = rhs.m_sinkCenter;
	m_bNeedPadLeft = rhs.m_bNeedPadLeft;
	m_bNeedPadRight = rhs.m_bNeedPadRight;
	m_bNeedPadCenter = rhs.m_bNeedPadCenter;
	m_pVideoWidgetLeft = rhs.m_pVideoWidgetLeft;
	m_pVideoWidgetRight = rhs.m_pVideoWidgetRight;
	m_pVideoWidgetCenter = rhs.m_pVideoWidgetCenter;
	m_linkToElementLeft = rhs.m_linkToElementLeft;
	m_linkToElementRight = rhs.m_linkToElementRight;
	m_linkToElementCenter = rhs.m_linkToElementCenter;
	return *this;
}

bool HGMMHelper::hasPads() const
{
	return (!m_bNeedPadLeft && !m_bNeedPadRight && !m_bNeedPadCenter);
}

int HGMMHelper::id() const
{
	return m_id;
}

void HGMMHelper::leftPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
{
	qDebug() << "leftPadAdded: sender obj " << sender->property("name").get<QString>();
	qDebug() << "src CAPS: " << srcPad->currentCaps()->toString();

	// audio or video
	QString caps = srcPad->currentCaps()->toString();
	if (caps.startsWith(QString("video/x-raw")))
	{
		qDebug() << "This is a video src pad";
		if (!m_linkToElementLeft)
		{
			qWarning() << "m_linkToElementLeft is NULL";
		}
		else
		{
			qDebug() << "m_linkToElementLeft is OK";
			QGst::PadPtr sinkPad = m_linkToElementLeft->getStaticPad("sink");
			if (!sinkPad)
			{
				qWarning() << "cannot get sink pad from link to element left";
				return;
			}
			else
			{
				qDebug() << "sinkPad is OK";
				//qDebug() << "sink CAPS: : " << sinkPad->currentCaps()->toString();
				QGst::PadLinkReturn ret  = srcPad->link(sinkPad);
				if (ret != QGst::PadLinkOk) qDebug() << "PAD LINK FAIL " << (int)ret;
				m_bNeedPadLeft = false;
			}
		}
	}
	else if (caps.startsWith(QString("audio/x-raw")))
	{
		qDebug() << "This is an audio src pad. TODO - DEAL WITH THIS PAD";
	}
}

void HGMMHelper::rightPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
{
	qDebug() << "rightPadAdded: sender obj " << sender->property("name").get<QString>();
	qDebug() << "src CAPS: " << srcPad->currentCaps()->toString();

	// audio or video
	QString caps = srcPad->currentCaps()->toString();
	if (caps.startsWith(QString("video/x-raw")))
	{
		qDebug() << "This is a video src pad";
		if (!m_linkToElementRight)
		{
			qWarning() << "m_linkRToElementRight is NULL";
		}
		else
		{
			qDebug() << "m_linkToElementRight is OK";
			QGst::PadPtr sinkPad = m_linkToElementRight->getStaticPad("sink");
			if (!sinkPad)
			{
				qWarning() << "cannot get sink pad from link to element right";
				return;
			}
			else
			{
				qDebug() << "sinkPad is OK";
				//qDebug() << "sink CAPS: : " << sinkPad->currentCaps()->toString();
				QGst::PadLinkReturn ret  = srcPad->link(sinkPad);
				if (ret != QGst::PadLinkOk) qDebug() << "PAD LINK FAIL " << (int)ret;
				m_bNeedPadRight = false;
			}
		}
	}
	else if (caps.startsWith(QString("audio/x-raw")))
	{
		qDebug() << "This is an audio src pad. TODO - DEAL WITH THIS PAD";
	}

}

void HGMMHelper::centerPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
{
	qDebug() << "centerPadAdded: sender obj " << sender->property("name").get<QString>();
	qDebug() << "src CAPS: " << srcPad->currentCaps()->toString();


	// audio or video
	QString caps = srcPad->currentCaps()->toString();
	if (caps.startsWith(QString("video/x-raw")))
	{
		qDebug() << "This is a video src pad";
		if (!m_linkToElementCenter)
		{
			qWarning() << "m_linkRToElementCenter is NULL";
		}
		else
		{
			qDebug() << "m_linkToElementCenter is OK";
			QGst::PadPtr sinkPad = m_linkToElementCenter->getStaticPad("sink");
			if (!sinkPad)
			{
				qWarning() << "cannot get sink pad from link to element center";
				return;
			}
			else
			{
				qDebug() << "sinkPad is OK";
				//qDebug() << "sink CAPS: : " << sinkPad->currentCaps()->toString();
				QGst::PadLinkReturn ret  = srcPad->link(sinkPad);
				if (ret != QGst::PadLinkOk) qDebug() << "PAD LINK FAIL " << (int)ret;
				m_bNeedPadCenter = false;
			}
		}
	}
	else if (caps.startsWith(QString("audio/x-raw")))
	{
		qDebug() << "This is an audio src pad. TODO - DEAL WITH THIS PAD";
	}
}

void HGMMHelper::setLeftSink(QGst::ElementPtr& leftSink)
{
	m_sinkLeft = leftSink;
	m_bNeedPadLeft = false;
}

void HGMMHelper::setRightSink(QGst::ElementPtr& rightSink)
{
	m_sinkRight = rightSink;
	m_bNeedPadRight = false;
}

void HGMMHelper::setCenterSink(QGst::ElementPtr& centerSink)
{
	m_sinkCenter = centerSink;
	m_bNeedPadCenter = false;
}

void HGMMHelper::setLeftStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget)
{
	if (info.isColor())
	{
		setSolidColorStimulus(info.getColor(), m_sinkLeft, QString("left"));
		m_bNeedPadLeft = false;
	}
	else if (info.isJPG())
	{
		setJPGStimulus(info, m_sinkLeft, QString("left"));
		m_bNeedPadLeft = false;
	}
	else if (info.isPNG())
	{
		setPNGStimulus(info, m_sinkLeft, QString("left"));
		m_bNeedPadLeft = false;
	}
	else
	{
		setUnknownStimulus(info, m_sinkLeft, m_linkToElementLeft, &HGMMHelper::leftPadAdded, QString("left"));
		m_bNeedPadLeft = true;
	}

	HelperPath path = {
			m_sinkLeft,
			m_linkToElementLeft,
			pwidget,
			m_bNeedPadLeft,
			info.isLoopPlayBack() };
	m_mapPaths.insert(HPlayerPositionType::Left, path);

}

void HGMMHelper::setRightStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget)
{
	if (info.isColor())
	{
		setSolidColorStimulus(info.getColor(), m_sinkRight, QString("right"));
		m_bNeedPadRight = false;
	}
	else if (info.isJPG())
	{
		setJPGStimulus(info, m_sinkRight, QString("right"));
		m_bNeedPadRight = false;
	}
	else if (info.isPNG())
	{
		setPNGStimulus(info, m_sinkRight, QString("right"));
		m_bNeedPadRight = false;
	}
	else
	{
		setUnknownStimulus(info, m_sinkRight, m_linkToElementRight, &HGMMHelper::rightPadAdded, QString("right"));
		m_bNeedPadRight = true;
	}

	HelperPath path = {
			m_sinkRight,
			m_linkToElementRight,
			pwidget,
			m_bNeedPadRight,
			info.isLoopPlayBack() };
	m_mapPaths.insert(HPlayerPositionType::Right, path);

}

void HGMMHelper::setCenterStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget)
{
	if (info.isColor())
	{
		setSolidColorStimulus(info.getColor(), m_sinkCenter, QString("center"));
		m_bNeedPadCenter = false;
	}
	else if (info.isJPG())
	{
		setJPGStimulus(info, m_sinkCenter, QString("center"));
		m_bNeedPadCenter = false;
	}
	else if (info.isPNG())
	{
		setPNGStimulus(info, m_sinkCenter, QString("center"));
		m_bNeedPadCenter = false;
	}
	else
	{
		setUnknownStimulus(info, m_sinkCenter, m_linkToElementCenter, &HGMMHelper::centerPadAdded, QString("center"));
		m_bNeedPadCenter = true;
	}

	HelperPath path = {
			m_sinkCenter,
			m_linkToElementCenter,
			pwidget,
			m_bNeedPadCenter,
			info.isLoopPlayBack() };
	m_mapPaths.insert(HPlayerPositionType::Center, path);
}


void HGMMHelper::setUnknownStimulus(const Habit::StimulusInfo& info, QGst::ElementPtr& sink, QGst::ElementPtr& linkToElement, padAddedMember func, const QString& s)
{
	// Video has filesrc!decodebin. Video pad will go directly into the input selector.
	QString filename = info.getAbsoluteFileName(m_root);
	QString format("%1-%2-%3");
	qDebug() << "HGMMHelper::setUnknownStimulus " << info.getFileName() << "   filename " << filename;
	QString filesrcName = QString(format).arg(QString("filesrc")).arg(s).arg(id());
	qDebug() << "filesrc name " << filesrcName;
	QString decodebinName = QString(format).arg(QString("decodebin")).arg(s).arg(id());
	qDebug() << "decodebin name " << decodebinName;
	QString sinkName = QString(format).arg(QString("sink")).arg(s).arg(id());
	qDebug() << "sink name " << sinkName;
	QString convertName = QString(format).arg(QString("convert")).arg(s).arg(id());
	qDebug() << "convert name " << convertName;
	QString identityName = QString(format).arg(QString("identity")).arg(s).arg(id());
	qDebug() << "identity name " << identityName;

	QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc", filesrcName.toStdString().c_str());
	filesrc->setProperty("location", filename);
	QGst::ElementPtr decodebin = QGst::ElementFactory::make("decodebin", decodebinName.toStdString().c_str());
	sink = QGst::ElementFactory::make("qwidget5videosink", sinkName.toStdString().c_str());
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

}


void HGMMHelper::play()
{
	// Do something special when movies
	QMapIterator<HPlayerPositionType, HelperPath> it(m_mapPaths);
	while (it.hasNext())
	{
		it.next();
//		if (it.value().isLoop)
		if (it.value().linkTo)
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
	qDebug() << "done";
	m_pipeline->setState(QGst::StatePlaying);
}



const HPlayerPositionType& getPPT(const QString& elementName)
{
	QStringList l = elementName.split('-');
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
	HelperPath *pHelperPath = NULL;
	QString sParentName(parentName);
//	qDebug() << "Got event: " << GST_EVENT_TYPE_NAME (event) << " at element " << sParentName;
	g_free(parentName);
	gst_object_unref(padParentElement);

	if (phelper->m_mapPaths.contains(getPPT(sParentName)))
	{
		pHelperPath = &phelper->m_mapPaths[getPPT(sParentName)];
		//qDebug() << "found in paths";
	}
	else
		qDebug() << "NOT found in paths";

	//qDebug() << "id from info " << GST_PAD_PROBE_INFO_ID (info) << " type " << info->type << " data? " << (info->data ? "yes": "NULL");
	if (GST_EVENT_TYPE (event) == GST_EVENT_EOS)
	{
   		qDebug() << "onEOSEvent got EOS from " << sParentName;
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

//		QGst::SeekEventPtr seekEv = QGst::SeekEvent::create(
//	       1.0,
//	       QGst::FormatTime,
//	       QGst::SeekFlagFlush,
//	       QGst::SeekTypeSet, 0,
//	       QGst::SeekTypeNone, GST_CLOCK_TIME_NONE);

		//m_pipeline->sendEvent(seekEv);
//		if (phelper->m_sinkCenter)
		if (pHelperPath->isLoop)
		{
			if (pHelperPath->sink)
			{
				qDebug() << "send seek event to sink";
				pHelperPath->sink->sendEvent(seekEv);
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

//		QGst::SeekEventPtr seekEv = QGst::SeekEvent::create(
//	       1.0,
//	       QGst::FormatTime,
//	       QGst::SeekFlagFlush,
//	       QGst::SeekTypeSet, 0,
//	       QGst::SeekTypeNone, GST_CLOCK_TIME_NONE);

		//m_pipeline->sendEvent(seekEv);
		// if looped source, send seek event
		if (pHelperPath->isLoop)
		{
			if (pHelperPath->sink)
			{
				qDebug() << "send seek event to sink";
				pHelperPath->sink->sendEvent(seekEv);
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
	return GST_PAD_PROBE_PASS;

//	  gst_pad_remove_probe (pad, GST_PAD_PROBE_INFO_ID (info));

//	return GST_PAD_PROBE_OK;
}


void HGMMHelper::setSolidColorStimulus(const QColor& color, QGst::ElementPtr& sink, const QString& s)
{
	QString format = QString("%1-%2");
	sink = QGst::ElementFactory::make("qwidget5videosink", format.arg(QString("qwidget5videosink")).arg(s).toStdString().c_str());
	QString binName = format.arg(QString("videotestsrc")).arg(s).toStdString().c_str();
	QGst::BinPtr bin = QGst::Bin::fromDescription(QString("videotestsrc name=%1 pattern=solid-color foreground-color=%2").arg(binName).arg(color.rgba()));
	qDebug() << "FYI - " << QString("videotestsrc name=%1 pattern=solid-color foreground-color=%2").arg(binName).arg(color.rgba());
	if (!bin)
	{
		qDebug() << "bin is NULL";
	}
	m_pipeline->add(bin, sink);
	bin->link(sink);
}

void HGMMHelper::setSoundStimulus(const Habit::StimulusInfo& info, QGst::Ui::VideoWidget *pwidget)
{

}

void HGMMHelper::setJPGStimulus(const Habit::StimulusInfo& info, QGst::ElementPtr& sink, const QString& s)
{
	// jpg has filesrc!jpegdec!imagefreeze. Video pad will go directly into the input selector.
	QString filename = info.getAbsoluteFileName(m_root);
	QString format("%1-%2");
	qDebug() << "HGMMHelper::setJPGStimulus " << info.getFileName() << "   filename " << filename;
	QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc", format.arg(QString("filesrc")).arg(s).toStdString().c_str());
	filesrc->setProperty("location", filename);
	QGst::ElementPtr jpegdec = QGst::ElementFactory::make("jpegdec", format.arg(QString("jpegdec")).arg(s).toStdString().c_str());
	QGst::ElementPtr imagefreeze = QGst::ElementFactory::make("imagefreeze", format.arg(QString("imagefreeze")).arg(s).toStdString().c_str());
	QGst::ElementPtr convert = QGst::ElementFactory::make("autovideoconvert", format.arg(QString("autovideoconvert")).arg(s).toStdString().c_str());
	sink = QGst::ElementFactory::make("qwidget5videosink", format.arg(QString("qwidget5videosink")).arg(s).toStdString().c_str());
	m_pipeline->add(filesrc, jpegdec, imagefreeze, convert, sink);
	filesrc->link(jpegdec);
	jpegdec->link(imagefreeze);
	imagefreeze->link(convert);
	convert->link(sink);
}

void HGMMHelper::setPNGStimulus(const Habit::StimulusInfo& info, QGst::ElementPtr& sink, const QString& s)
{
	// png has filesrc!pngdec!imagefreeze.
	QString format("%1-%2");
	QString filename = info.getAbsoluteFileName(m_root);
	qDebug() << "HGMMHelper::setJPGStimulus " << info.getFileName() << "   filename " << filename;
	QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc", format.arg(QString("filesrc")).arg(s).toStdString().c_str());
	filesrc->setProperty("location", filename);
	QGst::ElementPtr pngdec = QGst::ElementFactory::make("pngdec", format.arg(QString("pngdec")).arg(s).toStdString().c_str());
	QGst::ElementPtr imagefreeze = QGst::ElementFactory::make("imagefreeze", format.arg(QString("imagefreeze")).arg(s).toStdString().c_str());
	QGst::ElementPtr convert = QGst::ElementFactory::make("autovideoconvert", format.arg(QString("autovideoconvert")).arg(s).toStdString().c_str());
	sink = QGst::ElementFactory::make("qwidget5videosink", format.arg(QString("qwidget5videosink")).arg(s).toStdString().c_str());
	m_pipeline->add(filesrc, pngdec, imagefreeze, convert, sink);
	filesrc->link(pngdec);
	pngdec->link(imagefreeze);
	imagefreeze->link(convert);
	convert->link(sink);
}

void HGMMHelper::handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm)
{
	qInfo() << "HGMMPlayer::handlePipelineStateChange: " << stateName(scm->oldState()) << "-->" << stateName(scm->newState());
    switch (scm->newState()) {
    case QGst::StatePlaying:
        //start the timer when the pipeline starts playing
    		qDebug() << "NOW PLAYIng!";
    		Q_EMIT(nowPlaying());
        break;
    case QGst::StatePaused:
        //stop the timer when the pipeline pauses
    		qInfo() << "QGst::StatePaused";
        break;
    default:
    		qInfo() << "QGst::State???";
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
void HGMMHelper::onBusMessage(const QGst::MessagePtr & message)
{
	//qDebug() << "HGMMHelper::onBusMessage(" << m_id << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
	if (message->source().dynamicCast<QGst::Pipeline>() != m_pipeline)
	{
		return;
	}
	QGst::SeekingQueryPtr query;
	bool bb;

	QGst::SeekEventPtr seekEv;

	switch (message->type())
    {
    case QGst::MessageEos: //End of stream. We reached the end of the file.
    		qDebug() << "HGMMHelper::onBusMessage(" << m_id << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
        break;
    case QGst::MessageError: //Some error occurred.
    		qDebug() << "HGMMHelper::onBusMessage(" << m_id << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
    		qCritical() << "Got error, message is:";
        qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
        break;
    case QGst::MessageStateChanged: //The element in message->source() has changed state
    		qDebug() << "HGMMHelper::onBusMessage(" << m_id << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
    		handlePipelineStateChange(message.staticCast<QGst::StateChangedMessage>());
        break;
    case QGst::MessageAsyncDone:
    		//ClockTime ct = message.staticCast<QGst::AsyncDoneMessage>()->runningTime();
    		qDebug() << "HGMMHelper::onBusMessage(" << m_id << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
    		qDebug() << "AsyncDone, running time " <<
    			message.staticCast<QGst::AsyncDoneMessage>()->runningTime() <<
				" , source " <<
				message.staticCast<QGst::AsyncDoneMessage>()->source()->name();
    		break;
    default:
    		qDebug() << "HGMMHelper::onBusMessage(" << m_id << ") source " << message->source()->property("name").get<QString>() << " type "<< message->typeName();
    		qDebug() << "Unhandled msg: " << message->typeName() << " type " << message->type();
        break;
    }
}
