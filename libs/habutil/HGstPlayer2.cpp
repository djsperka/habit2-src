/*
 * HGstPlayer2.cpp
 *
 *  Created on: Apr 3, 2017
 *      Author: dan
 */

#include "HGstPlayer2.h"
#include <QWidget>
#include <QUrl>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGlib/Value>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/Element>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Pad>
#include <exception>
#include <gst/gst.h>


GstPadProbeReturn HGstPlayer2::cb_blocked_active (GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	GST_INFO("HGstPlayer2::cb_blocked_active - start");
	qDebug() << "HGstPlayer2::cb_blocked_active - start";
	HGstPlayer2 *player = (HGstPlayer2 *)user_data;

	// link new src and unblock it....
	qDebug() << "HGstPlayer2::cb_blocked_active - call setActivePad";
	player->setActivePad();
	qDebug() << "HGstPlayer2::cb_blocked_active - done";
	return GST_PAD_PROBE_OK;
}

GstPadProbeReturn HGstPlayer2::cb_blocked(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	qDebug() << "HGstPlayer2::cb_blocked - start";
	HGstPlayer2::QGstPtrs *ptrs = (HGstPlayer2::QGstPtrs *)user_data;
	qDebug() << "HGstPLayer::cb_blocked - pad is blocked: " << ptrs->vSrcPadPtr->parentElement()->name() << ":" << ptrs->vSrcPadPtr->name();
	return GST_PAD_PROBE_OK;
}


HGstPlayer2::HGstPlayer2(int ID, QWidget* w, const QDir& dir)
: HPlayer(ID, w, dir)
, m_iCurrentStimulusId(0)
, m_hasCurrent(false)
{
	// Create pipeline
	m_pipeline = QGst::Pipeline::create();

    QGst::BusPtr bus = m_pipeline->bus();
    bus->addSignalWatch();
    QGlib::connect(bus, "message", this, &HGstPlayer2::onBusMessage);

    // create sink
	m_sink = QGst::ElementFactory::make("qwidget5videosink");
	QGst::PadPtr sinkPad = m_sink->getStaticPad("sink");
	m_pipeline->add(m_sink);

	// Now stick in the background (hardcoded to be gray)
	QGst::ElementPtr videoTestSrc = QGst::ElementFactory::make("videotestsrc", "bkgd");
	//videoTestSrc->setProperty("pattern", "solid-color");
	//videoTestSrc->setProperty("foreground-color", 0xff40a010);

	QGst::PadPtr srcPad = videoTestSrc->getStaticPad("src");
	m_pipeline->add(videoTestSrc);
	//srcPad->link(sinkPad);

	QGstPtrs ptrs;
	ptrs.vSrcPadPtr = srcPad;
	m_pads.insert(0, ptrs);
	m_pads[0].uiVideoPadProbeID = srcPad->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HGstPlayer2::cb_blocked, &m_pads[0]);
	m_iCurrentStimulusId = 0;
	m_hasCurrent = false;
}

HGstPlayer2::~HGstPlayer2()
{
}


unsigned int HGstPlayer2::addBackground(unsigned int id, const QColor& bkgdColor)
{
	GST_INFO("HGstPlayer2::addBackground");

	// do direct connect from videotestsrc src pad
	QGst::ElementPtr videoTestSrc = QGst::ElementFactory::make("videotestsrc", "bkgd");
	//videoTestSrc->setProperty("pattern", "solid-color");
	//videoTestSrc->setProperty("foreground-color", 0xff40a010);

	QGst::PadPtr srcPad = videoTestSrc->getStaticPad("src");
	m_pipeline->add(videoTestSrc);

	QGstPtrs ptrs;
	ptrs.vSrcPadPtr = srcPad;
	m_pads.insert(id, ptrs);
	m_pads[id].uiVideoPadProbeID = srcPad->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HGstPlayer2::cb_blocked, &m_pads[id]);
	return id;
}

#if 0
unsigned int HGstPlayer2::addBinFromDescription(unsigned int id, const QString& description)
{
	QGst::BinPtr bin;
	qDebug() << "addBinFromDescription( " << id << " , " << description << " )";
	try
	{
		bin = QGst::Bin::fromDescription(description);
	}
	catch (const QGlib::Error & error)
	{
        qCritical() << error;
        qFatal("Error create element. Aborting...");
    }

	if (!bin)
	{
		qCritical() << "Failed to create element.";
		qFatal("Failed to create element.");
	}



	// we're going to add the bin to the pipeline, and link its src
	// to the input selector. Duh! You request a pad from the element,
	// then just link it! Brilliant!

	// first, get the sink pad
	QGst::PadPtr pad = m_input_selector->getRequestPad("sink_%u");
	qDebug() << "addBinFromDescription - input-selector request pad: " << pad->name();
	if (!pad)
		qDebug() << "Cannot get pad for sink+u";

	// now add the bin and link it to the sink pad.
	m_pipeline->add(bin);
	if (!bin->link(m_input_selector, pad->name().toStdString().c_str()))
	{
		qFatal("addBinFromDescription: Cannot link bin");
	}

	// Get the src pad from element "bkgd", and probe/block it
	QGst::ElementPtr bkgd = bin->getElementByName("bkgd");
	QGst::PadPtr bkgdSrcPad = bkgd->getStaticPad("src");	// should be videotestsrc. Bad, I know. FIXME.

	// now save
	QGstPtrs ptrs;
	m_pads.insert(id, ptrs);
	m_pads[id].inputSelectorPadPtr = pad;
	m_pads[id].videoSrcPadPtr = bkgdSrcPad;
	m_pads[id].uiVideoPadProbeID = bkgdSrcPad->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HGstPlayer2::cb_blocked, &m_pads[id]);

	return id;
}
#endif

void HGstPlayer2::decodebinPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
{
	qDebug() << "decodebinPadAdded - Element " << sender->property("name").get<QString>() << " added new pad pad: " << srcPad->name();
	//qDebug() << "caps: " << srcPad->currentCaps()->toString();

	QString senderName = sender->property("name").get<QString>();
	// sender name should be "decodebin_%u"
	// we parse out the number - that's the id.
	// There should have been one created previously, and
	//

	if (senderName.startsWith("db_"))
	{
		// check that this is video not audio
		if (srcPad->currentCaps()->toString().startsWith("video"))
		{
			unsigned int id = senderName.right(senderName.length()-3).toUInt();
			if (m_pads.contains(id))
			{
				QGstPtrs *ptrs = &m_pads[id];
				ptrs->vSrcPadPtr = srcPad;
				ptrs->uiVideoPadProbeID = srcPad->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HGstPlayer2::cb_blocked, ptrs);
				qDebug() << "decodebinPadAdded - new pad " << srcPad->parentElement()->name() << ":" << srcPad->name();
			}
			else
			{
				qCritical() << "decodebinPadAdded: id not found in m_pads? " << id;
			}
		}
		else if (srcPad->currentCaps()->toString().startsWith("audio"))
		{
			qDebug() << "decodebinPadAdded: audio pad added - IGNORING FIXME FIXME";
		}
	}
	else if (senderName.startsWith("di_"))
	{
		// this is an image source
		unsigned int id = senderName.right(senderName.length()-3).toUInt();
		if (m_pads.contains(id))
		{
			QGstPtrs *ptrs = &m_pads[id];

			// Image sources go like this:
			// filesrc -> decodebin -***-> imagefreeze -> input-selector
			// we have to establish the -***-> connection here.
			QString ifName = QString("if_%1").arg(id);
			QGst::ElementPtr ifPtr = QGst::ElementFactory::make("imagefreeze", ifName.toStdString().c_str());
			if (!ifPtr)
			{
				qFatal("Cannot create imagefreeze element");
			}

			// The pad should be connected to the imagefreeze element
			m_pipeline->add(ifPtr);
			QGst::PadPtr ifSinkPad = ifPtr->getStaticPad("sink");
			QGst::PadLinkReturn ret = srcPad->link(ifSinkPad);
			if (ret == QGst::PadLinkOk)
			{
				qDebug() << "decodebinPadAdded: linked src pad " << srcPad->parentElement()->name() << ":" << srcPad->name() << " to " << ifSinkPad->parentElement()->name() << ":" << ifSinkPad->name();

				// connect imagefreeze to .. nothing

				// Note for image sources, the imagefreeze element is already connected to the input-selector, so
				// that pad is already set in ptrs->inputSelectorPadPtr
				ptrs->vSrcPadPtr = ifPtr->getStaticPad("src");
				ptrs->uiVideoPadProbeID = ptrs->vSrcPadPtr->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, cb_blocked, ptrs);
				qDebug() << "decodebinPadAdded: Video pad, add blocking probe, returned probe id " << ptrs->uiVideoPadProbeID;
			}
			else
			{
				qCritical() << "decodebinPadAdded: pad link failed! src pad " << srcPad->name() << " sink " << ifSinkPad->name();
			}
		}
		else
		{
			qCritical() << "decodebinPadAdded: id not found in m_pads? " << id;
		}
	}
}


const HMediaContainerType& HGstPlayer2::getMediaContainerType(const QString& filename)
{
    // create a discoverer and initiate discovery process.
    QGst::DiscovererPtr discoverer = QGst::Discoverer::create(QGst::ClockTime::fromSeconds(1));
    QGst::DiscovererInfoPtr info;
    QUrl uri = QUrl::fromLocalFile(filename);
	int iAudio=0, iVideo=0, iImage=0;
    try
    {
        info = discoverer->discoverUri(uri);
    	Q_FOREACH(const QGst::DiscovererStreamInfoPtr &streamInfo, info->streams())
    	{
    		if (streamInfo->caps()->toString().startsWith(QString("audio"))) iAudio++;
    		else if (streamInfo->caps()->toString().startsWith(QString("video"))) iVideo++;
    		else if (streamInfo->caps()->toString().startsWith(QString("image"))) iImage++;
      		qDebug() << "getMediaContainerType: caps=" << streamInfo->caps();
        }
    	if (iImage==1 && iVideo==0 && iAudio==0)
    		return HMediaContainerType::Image;
    	else if (iImage==0 && iVideo==1 && iAudio==0)
    		return HMediaContainerType::Video;
    	else if (iImage==0 && iVideo==0 && iAudio==1)
    		return HMediaContainerType::Audio;
    	else if (iImage==0 && iVideo==1 && iAudio==1)
    		return HMediaContainerType::AudioVideo;
    	else
    		return HMediaContainerType::UnknownContainerType;
    }
    catch(const QGlib::Error &error)
    {
        qCritical() << "Error in discovery: " << error;
    }
	return HMediaContainerType::UnknownContainerType;
}

unsigned int HGstPlayer2::addStimulusPrivate(unsigned int id)
{
	QGst::BinPtr bin;
	QString decodebinName;
	QString filesrcName;
	QString imagefreezeName;
	QString stimulusPath;

	filesrcName = QString("fs_%1").arg(id);
	decodebinName = QString("db_%1").arg(id);
	imagefreezeName = QString("if_%1").arg(id);
	stimulusPath = getStimulusInfo(id).getAbsoluteFileName(getStimulusRoot());
	qDebug() << "addStimulusPrivate: Create elements for stim file location=" << stimulusPath;

	try {

		const HMediaContainerType& mct = getMediaContainerType(stimulusPath);
		if (mct == HMediaContainerType::UnknownContainerType)
		{
			QString s = QString("addStimulusPrivate: Unknown container type: %1").arg(stimulusPath);
			throw s.toStdString().c_str();
		}
		else if (mct == HMediaContainerType::Audio)
		{
			QString s = QString("addStimulusPrivate: Expecting video or image container, but got audio: %1").arg(stimulusPath);
			throw s.toStdString().c_str();
		}
		else if (mct == HMediaContainerType::Video || mct == HMediaContainerType::AudioVideo)
		{
			// Video has filesrc!decodebin. Video pad will go directly into the input selector.
			QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc", stimulusPath.toStdString().c_str());
			filesrc->setProperty("location", stimulusPath);
			QGst::ElementPtr decodebin = QGst::ElementFactory::make("decodebin", decodebinName.toStdString().c_str());
			QGlib::connect(decodebin, "pad_added", this, &HGstPlayer2::decodebinPadAdded, QGlib::PassSender);
			m_pipeline->add(filesrc, decodebin);
			filesrc->link(decodebin);	// decodebin still not linked to anything...
			QGstPtrs ptrs;
			m_pads.insert(id, ptrs);	// ptrs will be filled in the slot
		}
		else if (mct == HMediaContainerType::Image)
		{
			decodebinName = QString("di_%1").arg(id);

			// image takes the decodebin directly to imagefreeze ("if"). We know there will be
			// exactly one pad created, so we can create the bin directly, without needing to use
			// the pad_added signal.
			QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc", filesrcName.toStdString().c_str());
			filesrc->setProperty("location", stimulusPath);
			QGst::ElementPtr decodebin = QGst::ElementFactory::make("decodebin", decodebinName.toStdString().c_str());
			QGlib::connect(decodebin, "pad_added", this, &HGstPlayer2::decodebinPadAdded, QGlib::PassSender);
			m_pipeline->add(filesrc, decodebin);
			filesrc->link(decodebin);

			QGstPtrs ptrs;
			m_pads.insert(id, ptrs);	// ptrs will be filled in the slot
		}
		else
		{
			// this should not happen
			throw "Unhandled media container type!";
		}
	} catch (const char *str) {
		qCritical() << "Exception in addStimulusPrivate: " << str;
	}

	return id;
}



// play() is a multi-step process with gstreamer.
// The current stim must have its src pad blocked. We do that and handle the next step in the callback.
// In the callback, the pad is blocked, so we can switch the current input pad on the input selector
// to the new stim to be played. We must also remove the probe on that pad, and we do so here.
// Finally, set the pipeline back to PLAYING state.

void HGstPlayer2::play(unsigned int num)
{
	qDebug()  << "HGstPlayer2::play( " << num << " ) - start";
	if (m_hasCurrent && num == m_iCurrentStimulusId)
	{
		qDebug() << "HGstPlayer::play - same stim, nothing to do.";
		return;
	}

	if (m_pads.contains(num))
	{
		QGstPtrs *ptrs = &m_pads[num];

		qDebug() << "HGstPlayer2::play - pause pipeline";
		m_pipeline->setState(QGst::StatePaused);

		// If there is a current stim, block its pad.
		// If there is NOT a current stim, we will unblock the new stim's pad and
		// set the active pad.

		if (m_hasCurrent)
		{
			QGstPtrs *ptrsCurrent = &m_pads[m_iCurrentStimulusId];
			qDebug() << "HGstPlayer2::play - current stim id " << m_iCurrentStimulusId << " send blocking probe to " << ptrsCurrent->vSrcPadPtr->parentElement()->name() << ":" << ptrsCurrent->vSrcPadPtr->name();
			ptrsCurrent->uiVideoPadProbeID = ptrsCurrent->vSrcPadPtr->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, cb_blocked_active,  ptrsCurrent);


			// HACK FIXME
			m_iPendingStimulusId = num;
			//m_iCurrentStimulusId = num;
			m_hasCurrent = true;
		}
		else
		{
			qDebug() << "HGstPlayer2::play - No current stim, set active pad to " << num;
			setActivePad(num);
		}
		QGst::State statePipeline = m_pipeline->currentState();
		qDebug() << "HGstPlayer2::play - pipeline state " << statePipeline;
	}
	else
	{
		qDebug() << "HGstPlayer2::play - id not found!!!";
	}
	qDebug()  << "HGstPlayer2::play( " << num << " ) - done";
}

void HGstPlayer2::setActivePad(void)
{
	qDebug() << "HGstPlayer2:play() - pending " << m_iPendingStimulusId << " current? " << m_hasCurrent << " (" << m_iCurrentStimulusId << ")";
	setActivePad(m_iPendingStimulusId);
};


void HGstPlayer2::setActivePad(unsigned int num)
{
	// FIXME FIXME NO PROTECTION!
	QGstPtrs *ptrs = &m_pads[num];
	qDebug() << "setActivePad(" << num << ") - start - vSrcPad blocked? " << ptrs->vSrcPadPtr->isBlocked();

	// get sink pad
	QGst::PadPtr sinkPad = m_sink->getStaticPad("sink");

	// If there is a current src, then unlink it. It should have been blocked already.
	if (m_hasCurrent)
	{
		QGstPtrs *ptrsCurrent = &m_pads[m_iCurrentStimulusId];
		qDebug() << "setActivePad: Got sink pad " << sinkPad->parentElement()->name() << ":" << sinkPad->name();
		qDebug() << "setActivePad: Unlinking " << ptrsCurrent->vSrcPadPtr->parentElement()->name() << ":" << ptrsCurrent->vSrcPadPtr->name();
		ptrsCurrent->vSrcPadPtr->unlink(sinkPad);	// should be blocked.
	}
	else
	{
		qDebug() << "setActivePad: no current src, go ahead and link new src";
		ptrs->vSrcPadPtr->link(sinkPad);
	}

	// to set pad offset we have to obtain the element base time and the clock time....
	QGst::ClockTime baseTime = m_pipeline->getBaseTime();
	QGst::ClockTime absTime = m_pipeline->clock()->clockTime();

	qDebug() << "setActivePad - abs time " << absTime << " base time " << baseTime;
	//ptrs->inputSelectorPadPtr->setOffset(absTime - baseTime);

	qDebug() << "setActivePad - remove blocking probe id " << ptrs->uiVideoPadProbeID << " from " << ptrs->vSrcPadPtr->parentElement()->name() << ":" << ptrs->vSrcPadPtr->name();

	ptrs->vSrcPadPtr->removeProbe(ptrs->uiVideoPadProbeID);
	ptrs->uiVideoPadProbeID = 0;
	m_iCurrentStimulusId = num;
	m_hasCurrent = true;
	m_pipeline->setState(QGst::StatePlaying);
	qDebug() << "setActivePad(" << num << ") - done - videoSrcPad blocked? " << ptrs->vSrcPadPtr->isBlocked();
}


void HGstPlayer2::pause()
{
	m_pipeline->setState(QGst::StatePaused);
}

void HGstPlayer2::clear()
{
	play(0);
}

void HGstPlayer2::stop()
{
	m_pipeline->setState(QGst::StateNull);
    //Q_EMIT stateChanged();
}

void HGstPlayer2::onBusMessage(const QGst::MessagePtr & message)
{
	//qDebug() << "HGstPlayer2::onBusMessage";
    switch (message->type()) {
    case QGst::MessageEos: //End of stream. We reached the end of the file.
    	qCritical() << "Got EOS!";
        stop();
        break;
    case QGst::MessageError: //Some error occurred.
    	qCritical() << "Got error, message is:";
        qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
        stop();
        break;
    case QGst::MessageStateChanged: //The element in message->source() has changed state
       	//qDebug() << "HGstPlayer2::onBusMessage( state changed )";
            //handlePipelineStateChange(message.staticCast<QGst::StateChangedMessage>());
        break;
    default:
    	qDebug() << "Unknown msg: " << message->typeName();
        break;
    }
}

