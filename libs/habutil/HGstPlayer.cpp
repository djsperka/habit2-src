/*
 * HGstPlayer.cpp
 *
 *  Created on: Apr 3, 2017
 *      Author: dan
 */

#include "HGstPlayer.h"
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


GstPadProbeReturn HGstPlayer::cb_blocked_active (GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	GST_INFO("HGstPlayer::cb_blocked_active - start");
	qDebug() << "HGstPlayer::cb_blocked_active - start";
	HGstPlayer *player = (HGstPlayer *)user_data;
	gchar *name = gst_pad_get_name(pad);
	qDebug() << "CB_BLOCKED_ACTIVE on pad " << name;
	g_free(name);

	// Now can remove probe from the pending pad and set it as the active pad
	qDebug() << "HGstPlayer::cb_blocked_active - call setActivePad";
	player->setActivePad();
	GST_INFO("HGstPlayer::cb_blocked_active - done");
	qDebug() << "HGstPlayer::cb_blocked_active - done";
	return GST_PAD_PROBE_OK;
}

GstPadProbeReturn HGstPlayer::cb_blocked(GstPad *pad, GstPadProbeInfo *info, gpointer user_data)
{
	GST_INFO("HGstPlayer::cb_blocked - start");
	qDebug() << "HGstPLayer::cb_blocked - start";
	//qDebug() << "CB_BLOCKED on pad " << name << " probe id ZZZ";// << GST_PAD_PROBE_INFO_ID (info);
	HGstPlayer::QGstPtrs *ptrs = (HGstPlayer::QGstPtrs *)user_data;
	//gchar *name = gst_pad_get_name(pad);
	//g_free(name);
	qDebug() << "HGstPLayer::cb_blocked - pad is blocked: " << ptrs->videoSrcPadPtr->parentElement()->name() << ":" << ptrs->videoSrcPadPtr->name();
	GST_INFO("HGstPlayer::cb_blocked - pad is blocked: %s:%s", ptrs->videoSrcPadPtr->parentElement()->name().toStdString().c_str(), ptrs->videoSrcPadPtr->name().toStdString().c_str());
	return GST_PAD_PROBE_OK;
}


HGstPlayer::HGstPlayer(int ID, QWidget* w, const QDir& dir)
: HPlayer(ID, w, dir)
, m_iCurrentStimulusId(0)
, m_hasCurrent(false)
{
	// Create pipeline
	m_pipeline = QGst::Pipeline::create();

    QGst::BusPtr bus = m_pipeline->bus();
    bus->addSignalWatch();
    QGlib::connect(bus, "message", this, &HGstPlayer::onBusMessage);


	// Create elements for back end of pipeline.
	m_input_selector = QGst::ElementFactory::make("input-selector", "vis");
	QGst::ElementPtr avconvert = QGst::ElementFactory::make("autovideoconvert");
	m_sink = QGst::ElementFactory::make("qwidget5videosink");

	// add to pipeline and link
	m_pipeline->add(m_input_selector, avconvert, m_sink);
	if (!m_input_selector->link(avconvert) || !avconvert->link(m_sink))
	{
		qFatal("Error linking back end of pipeline.");
	}

	// Now stick in the background (hardcoded to be gray)
	qDebug() << "Create background element...";
	//addBinFromDescription(0, "videotestsrc pattern=solid-color foreground-color=0xff40a010 name=bkgd");
	//addBinFromDescription(0, "videotestsrc name=bkgd");
	//addBinFromDescription(102, "videotestsrc pattern=ball");
	//addBinFromDescription(103, "videotestsrc pattern=circular");
	//addBinFromDescription(104, "videotestsrc pattern=solid-color foreground-color=0xff3010e0");
	//qDebug() << "start playing";
	//m_pipeline->setState(QGst::StatePlaying);
	addBackground(0, QColor(127, 127, 127));
}

HGstPlayer::~HGstPlayer()
{
}


unsigned int HGstPlayer::addBackground(unsigned int id, const QColor& bkgdColor)
{
	GST_INFO("HGstPlayer::addBackground");

	// do direct connect from videotestsrc src pad
	QGst::ElementPtr videoTestSrc = QGst::ElementFactory::make("videotestsrc", "bkgd");
	//videoTestSrc->setProperty("pattern", "solid-color");
	//videoTestSrc->setProperty("foreground-color", 0xff40a010);

	QGst::PadPtr srcPad = videoTestSrc->getStaticPad("src");
	m_pipeline->add(videoTestSrc);

	// get request pad from input-selector and link
	QGst::PadPtr sinkPad = m_input_selector->getRequestPad("sink_%u");
	QGst::PadLinkReturn ret = srcPad->link(sinkPad);
	if (ret == QGst::PadLinkOk)
	{
		qDebug() << "addBackground: linked src pad " << srcPad->name() << " to " << sinkPad->name();

		// now save. Must send &m_pads[id] as arg to callback if callback is to modify it (it doesn't)!
		QGstPtrs ptrs;
		ptrs.inputSelectorPadPtr = sinkPad;
		ptrs.videoSrcPadPtr = srcPad;
		m_pads.insert(id, ptrs);
		m_pads[id].uiVideoPadProbeID = srcPad->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HGstPlayer::cb_blocked, &m_pads[id]);
	}
	else
	{
		qCritical() << "addBackground: pad link failed! src pad " << srcPad->name() << " sink " << sinkPad->name();
		qFatal("no background!");
	}
	return id;
}

unsigned int HGstPlayer::addBinFromDescription(unsigned int id, const QString& description)
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
	m_pads[id].uiVideoPadProbeID = bkgdSrcPad->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HGstPlayer::cb_blocked, &m_pads[id]);

	return id;
}

void HGstPlayer::decodebinPadAdded(const QGlib::ObjectPtr & sender, const QGst::PadPtr & srcPad)
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

				// request a pad from the input selector, then link the pad to it.
				QGst::PadPtr sinkPad = m_input_selector->getRequestPad("sink_%u");
				//sinkPad->setProperty("always-ok", true);
				QGst::PadLinkReturn ret = srcPad->link(sinkPad);
				if (ret == QGst::PadLinkOk)
				{
					qDebug() << "decodebinPadAdded - linked src pad " << srcPad->parentElement()->name() << ":" << srcPad->name() << " to " << sinkPad->parentElement()->name() << ":" << sinkPad->name();

					ptrs->inputSelectorPadPtr = sinkPad;
					ptrs->videoSrcPadPtr = srcPad;
					ptrs->uiVideoPadProbeID = srcPad->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, &HGstPlayer::cb_blocked, ptrs);
					qDebug() << "decodebinPadAdded - Video pad, add blocking probe, returned probe id " << ptrs->uiVideoPadProbeID;

				}
				else
				{
					qCritical() << "decodebinPadAdded: pad link failed! src pad " << srcPad->name() << " sink " << sinkPad->name();
				}
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
			QGst::ElementPtr ifPtr = m_pipeline->getElementByName(ifName.toStdString().c_str());
			if (!ifPtr)
			{
				qCritical() << "Cannot find element named " << ifName;
			}
			else
			{
				// The pad should be connected to the imagefreeze element
				QGst::PadPtr sinkPad = ifPtr->getStaticPad("sink");
				QGst::PadLinkReturn ret = srcPad->link(sinkPad);
				if (ret == QGst::PadLinkOk)
				{
					qDebug() << "decodebinPadAdded: linked src pad " << srcPad->parentElement()->name() << ":" << srcPad->name() << " to " << sinkPad->parentElement()->name() << ":" << sinkPad->name();

					// Note for image sources, the imagefreeze element is already connected to the input-selector, so
					// that pad is already set in ptrs->inputSelectorPadPtr
					ptrs->videoSrcPadPtr = srcPad;
					ptrs->uiVideoPadProbeID = srcPad->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, cb_blocked, ptrs);
					qDebug() << "decodebinPadAdded: Video pad, add blocking probe, returned probe id " << ptrs->uiVideoPadProbeID;
				}
				else
				{
					qCritical() << "decodebinPadAdded: pad link failed! src pad " << srcPad->name() << " sink " << sinkPad->name();
				}
			}
		}
		else
		{
			qCritical() << "decodebinPadAdded: id not found in m_pads? " << id;
		}
	}
}


const HMediaContainerType& HGstPlayer::getMediaContainerType(const QString& filename)
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

unsigned int HGstPlayer::addStimulusPrivate(unsigned int id)
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
			QGlib::connect(decodebin, "pad_added", this, &HGstPlayer::decodebinPadAdded, QGlib::PassSender);
			m_pipeline->add(filesrc, decodebin);
			filesrc->link(decodebin);	// decodebin still not linked to anything...
			qDebug() << "addStimulusPrivate: linked filesrc to decodebin element.";
			QGstPtrs ptrs;
			m_pads.insert(id, ptrs);	// ptrs will be filled in the slot

		}
		else if (mct == HMediaContainerType::Image)
		{
			qDebug() << "Image file";
			decodebinName = QString("di_%1").arg(id);

			// image takes the decodebin directly to imagefreeze ("if"). We know there will be
			// exactly one pad created, so we can create the bin directly, without needing to use
			// the pad_added signal.
			QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc", filesrcName.toStdString().c_str());
			filesrc->setProperty("location", stimulusPath);
			QGst::ElementPtr decodebin = QGst::ElementFactory::make("decodebin", decodebinName.toStdString().c_str());
			QGlib::connect(decodebin, "pad_added", this, &HGstPlayer::decodebinPadAdded, QGlib::PassSender);
			QGst::ElementPtr imagefreeze = QGst::ElementFactory::make("imagefreeze", imagefreezeName.toStdString().c_str());
			m_pipeline->add(filesrc, decodebin, imagefreeze);
			filesrc->link(decodebin);
			//decodebin->link(imagefreeze);  // do this in slot?

			// get a pad from the input selector and link to src pad on imagefreeze
			// first, get the pad
			QGst::PadPtr sink = m_input_selector->getRequestPad("sink_%u");
			qDebug() << "addStimulusPrivate: input-selector request pad: " << sink->name();

			// Get src pad from imagefreeze
			QGst::PadPtr src = imagefreeze->getStaticPad("src");
			src->link(sink);

			QGstPtrs ptrs;
			ptrs.inputSelectorPadPtr = sink;
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

void HGstPlayer::play(unsigned int num)
{
	qDebug()  << "HGstPlayer::play( " << num << " ) - start";
	if (m_pads.contains(num))
	{
		QGstPtrs *ptrs = &m_pads[num];
		if (ptrs->inputSelectorPadPtr)
		{
			//qDebug() << "HGstPlayer::play - pause pipeline";
			//m_pipeline->setState(QGst::StatePaused);

#if 0
			m_input_selector->setProperty("active-pad", ptrs->inputSelectorPadPtr);
			m_pipeline->setState(QGst::StatePlaying);

#else
			// If there is a current stim, block its pad.
			// If there is NOT a current stim, we will unblock the new stim's pad and
			// set the active pad.

			if (m_hasCurrent)
			{
				QGstPtrs *ptrsCurrent = &m_pads[m_iCurrentStimulusId];
				qDebug() << "HGstPlayer::play - current stim id " << m_iCurrentStimulusId << " send blocking probe to " << ptrsCurrent->videoSrcPadPtr->parentElement()->name() << ":" << ptrsCurrent->videoSrcPadPtr->name();
				ptrsCurrent->uiVideoPadProbeID = ptrsCurrent->videoSrcPadPtr->addProbe(GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, cb_blocked_active,  this);


				// HACK FIXME
				m_iPendingStimulusId = num;
				//m_iCurrentStimulusId = num;
				m_hasCurrent = true;
			}
			else
			{
				qDebug() << "HGstPlayer::play - No current stim, set active pad to " << num;
				setActivePad(num);
			}
#endif
			QGst::State statePipeline = m_pipeline->currentState();
			QGst::State stateInputSelector = m_input_selector->currentState();
			qDebug() << "HGstPlayer::play - pipeline state " << statePipeline << " input_sel state " << stateInputSelector;
		}
		else
			qCritical() << "HGstPlayer::play - No input selector pad!?!";

	   	m_iCurrentStimulusId = num;
	}
	else
	{
		qDebug() << "HGstPlayer::play - id not found!!!";
	}
	qDebug()  << "HGstPlayer::play( " << num << " ) - done";
}

void HGstPlayer::setActivePad(unsigned int num)
{
	// FIXME FIXME NO PROTECTION!
	QGstPtrs *ptrs = &m_pads[num];
	qDebug() << "setActivePad(" << num << ") - start - videoSrcPad blocked? " << ptrs->videoSrcPadPtr->isBlocked();

	m_input_selector->setProperty("active-pad", ptrs->inputSelectorPadPtr);

	// to set pad offset we have to obtain the element base time and the clock time....
	QGst::ClockTime baseTime = m_pipeline->getBaseTime();
	QGst::ClockTime absTime = m_pipeline->clock()->clockTime();

	qDebug() << "setActivePad - abs time " << absTime << " pipeline basetime " << baseTime << " running time " << absTime-baseTime;
	ptrs->inputSelectorPadPtr->setOffset(absTime - baseTime);

	qDebug() << "setActivePad - remove blocking probe id " << ptrs->uiVideoPadProbeID << " from " << ptrs->videoSrcPadPtr->parentElement()->name() << ":" << ptrs->videoSrcPadPtr->name();

	ptrs->videoSrcPadPtr->removeProbe(ptrs->uiVideoPadProbeID);
	ptrs->uiVideoPadProbeID = 0;
	m_iCurrentStimulusId = num;
	m_hasCurrent = true;
	m_pipeline->setState(QGst::StatePlaying);
	qDebug() << "setActivePad(" << num << ") - done - videoSrcPad blocked? " << ptrs->videoSrcPadPtr->isBlocked();
}


void HGstPlayer::pause()
{
	m_pipeline->setState(QGst::StatePaused);
}

void HGstPlayer::clear()
{
	play(0);
}

void HGstPlayer::stop()
{
	m_pipeline->setState(QGst::StateNull);
    //Q_EMIT stateChanged();
}

void HGstPlayer::handleStreamStatusMessage(const QGst::StreamStatusMessagePtr & message)
{
}

void HGstPlayer::onBusMessage(const QGst::MessagePtr & message)
{
	//qDebug() << "HGstPlayer::onBusMessage";
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
       	qDebug() << "HGstPlayer::onBusMessage( state changed )";
            //handlePipelineStateChange(message.staticCast<QGst::StateChangedMessage>());
        break;
    case QGst::MessageAsyncDone:
    	//ClockTime ct = message.staticCast<QGst::AsyncDoneMessage>()->runningTime();
    	qDebug() << "onBusMessage: AsyncDone, running time " <<
    			message.staticCast<QGst::AsyncDoneMessage>()->runningTime() <<
				" , source " <<
				message.staticCast<QGst::AsyncDoneMessage>()->source()->name();
    case QGst::MessageQos:
    	qDebug() << "onBusMessage: running time " << message.staticCast<QGst::QosMessage>()->runningTime() <<
				" jitter " << message.staticCast<QGst::QosMessage>()->jitter() <<
				" stream time " << message.staticCast<QGst::QosMessage>()->streamTime() <<
				" timestamp " << message.staticCast<QGst::QosMessage>()->timestamp();
//    	handleStreamStatusMessage(message.staticCast<QGst::StreamStatusMessage>());
    	break;
    default:
    	qDebug() << "Unknown msg: " << message->typeName() << " type " << message->type();
        break;
    }
}
//10 00000 00000 00000 00000

#if 0
ClockTime AsyncDoneMessage::runningTime() const
#endif
