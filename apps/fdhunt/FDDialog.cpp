/*
 * FDDialog.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: dan
 */


#include "FDDialog.h"
#include "FDHuntDialog.h"
#include <gst/gst.h>
#include <QMutexLocker>
#include <boost/bind.hpp>


#define IMAGE_PNG_TIRE "/Users/dan/Desktop/habit22/stim/examples/images/tools/tire.png"
#define VIDEO_AVI_PLANE "/Users/dan/Desktop/habit22/stim/examples/avi/toy_plane_liftoff.avi"
#define VIDEO_MOV_TAXI "/Users/dan/Desktop/habit22/stim/examples/mov/taxi-480p.mov"
#define VIDEO_MOV_LOOP "/Users/dan/Desktop/habit22/stim/examples/mov/Loop.mov"
#define USE_AUDIOMIXER 1
#define DO_FLUSHING_SEGMENT_SEEK_IN_BUS_CALLBACK 1
#define DONT_USE_EVENT_PROBES 1
//#define USE_AUDIOMIXER 1


PipelineData::PipelineData()
: pipeline(NULL)
, bLoop(false)
, probeID(-1)
, bWaitingForFlushingSeekCompletion(false)
, bPrerolled(false)
, bAudio(false)
, bVideo(false)
{}

PipelineData::~PipelineData()
{
}

FDDialog::FDDialog(QWidget *parent)
: QDialog(parent)
{
	QDialogButtonBox *buttonBox;
	QVBoxLayout *vbox;
	QStringList sl;

	sl 		<< "Test src 1" 										// 0
			<< "Test src 2" 										// 1
			<< "Decodebin image tire" 							// 2
			<< "Decodebin avi plane no loop no sound"  			// 3
			<< "Decodebin mov taxi no loop no sound" 			// 4
			<< "Decodebin mov dot loop no sound"					// 5
			<< "Decodebin mov dot no loop with sound"			// 6
			<< "Decodebin mov dot loop with sound"				// 7
			<< "Decodebin avi plane loop no sound"  				// 8
			<< "Decodebin mov taxi loop no sound" 				// 9
			<< "Decodebin mov taxi loop with sound"				// 10
			<< "Decodebin loop sound no video";					// 11
	m_pListWidget = new QListWidget(this);
	m_pListWidget->addItems(sl);

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal);

	m_pbGo = new QPushButton("Go");
	buttonBox->addButton(m_pbGo, QDialogButtonBox::ActionRole);
	connect(m_pbGo, SIGNAL(clicked()), this, SLOT(goClicked()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));
	vbox = new QVBoxLayout;
	vbox->addWidget(m_pListWidget);
	vbox->addWidget(buttonBox);
	setLayout(vbox);


	// Initialize gstreamer here
	gst_init(0, NULL);

	// create glib main loop
	m_pgml = g_main_loop_new(NULL, FALSE);

	// start thread
	m_gthread = g_thread_new("FDDialog-main-loop", &FDDialog::threadFunc, m_pgml);

}


PipelineData *FDDialog::createPipeline(int iSelected)
{
	switch (iSelected)
	{
	case 0:
		return createTestSrcPipeline();
		break;
	case 1:
		return createTestSrcSolidColorPipeline();
		break;
	case 2:
		return createDecodebinImagePipeline();
		break;
	case 3:
		return createDecodebinVideoPipeline(VIDEO_AVI_PLANE, false, false);
		break;
	case 4:
		return createDecodebinVideoPipeline(VIDEO_MOV_TAXI, false, false);
		break;
	case 5:
		return createDecodebinVideoPipeline(VIDEO_MOV_LOOP, true, false);
		break;
	case 6:
		return createDecodebinVideoPipeline(VIDEO_MOV_LOOP, false, true);
		break;
	case 7:
		return createDecodebinVideoPipeline(VIDEO_MOV_LOOP, true, true);
		break;
	case 8:
		return createDecodebinVideoPipeline(VIDEO_AVI_PLANE, true, false);
		break;
	case 9:
		return createDecodebinVideoPipeline(VIDEO_MOV_TAXI, true, false);
		break;
	case 10:
		return createDecodebinVideoPipeline(VIDEO_MOV_TAXI, true, true);
		break;
	case 11:
		return createDecodebinVideoPipeline(VIDEO_MOV_LOOP, true, true, false);	// no video
		break;
	default:
		qWarning() << "Not configured to create pipeline for row " << iSelected;
		return NULL;
	}
}


gpointer FDDialog::threadFunc(gpointer user_data)
{
	GMainLoop *pgml = (GMainLoop *)user_data;
	// create and start main loop here
	qDebug() << "FDDialog::threadFunc - starting main loop";
	g_main_loop_run(pgml);
	qDebug() << "FDDialog::threadFunc - main loop ended";
	return NULL;
}

gboolean FDDialog::busCallback(GstBus *, GstMessage *msg, gpointer p)
{
	PipelineData *pdata = (PipelineData *)p;

	qDebug() << "busCallback: " << GST_MESSAGE_TYPE_NAME(msg) << " from " << GST_MESSAGE_SRC_NAME(msg);
	if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ASYNC_DONE)
	{
		qDebug() << "busCallback: Got ASYNC_DONE from " << GST_MESSAGE_SRC_NAME(msg);

		// Loop over each stim source on this bus.
		// The ASYNC_DONE message doesn't come until all sources have prerolled, or entered PAUSED state.
		// If any of the sources are looping sources, and if they are not already prerolled, then we
		// must issue a flushing seek here on that portion of the pipeline.

		if (pdata->bPrerolled)
		{
			qDebug() << "busCallback: Already prerolled.";
		}
		else
		{
			if (!pdata->bLoop)
			{
				// If not looping, then this is the initial preroll.
				qDebug() << "busCallback: Initial preroll done for non-looping source.";
				pdata->bPrerolled = true;
			}
			else
			{
				if (pdata->bWaitingForFlushingSeekCompletion)
				{
					pdata->bWaitingForFlushingSeekCompletion = false;
					pdata->bPrerolled = true;
					qDebug() << "busCallback: Flushing seek done for looping source. Prerolled.";
				}
				else
				{
					QMutexLocker locker(&pdata->mutex);
					// flushing seek on decodebin element.
					GstElement *decodebin = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "decodebin");
					Q_ASSERT(decodebin);
					if (!gst_element_seek(decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
					{
						qCritical() << "busCallback: SEEK FAILED";
					}
					else
					{
						qDebug() << "busCallback: flush seek done";
						pdata->bWaitingForFlushingSeekCompletion = true;
					}
					gst_object_unref(decodebin);
				}
			}
		}
	}
	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_STATE_CHANGED)
	{
		if (GST_ELEMENT(msg->src) == pdata->pipeline)
		{
			GstState old_state, new_state;
			gst_message_parse_state_changed(msg, &old_state, &new_state, NULL);
			qDebug() << "busCallback: got STATE_CHANGED " << gst_element_state_get_name(old_state) << "-" << gst_element_state_get_name(new_state) << " from " << GST_MESSAGE_SRC_NAME(msg);
		}
	}
	else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_SEGMENT_DONE)
	{
		QMutexLocker(&pdata->mutex);
		qDebug() << "busCallback: SEGMENT DONE from "<< GST_MESSAGE_SRC_NAME(msg);
#if DO_FLUSHING_SEGMENT_SEEK_IN_BUS_CALLBACK
		GstElement *decodebin = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "decodebin");
		Q_ASSERT(decodebin);
		if (!gst_element_seek(decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
		{
			qCritical() << "busCallback: SEEK FAILED";
		}
		else
			qCritical() << "busCallback: SEEK OK";
		gst_object_unref(decodebin);
#endif
	}
	return TRUE;
}

FDDialog::~FDDialog()
{
	// exit main loop
	qDebug() << "FDDialog::~FDDialog - quit main loop";
	g_main_loop_quit(m_pgml);
	qDebug() << "FDDialog::~FDDialog - g_thread_join...";
	g_thread_join(m_gthread);
	qDebug() << "FDDialog::~FDDialog - g_thread_join done";

	g_main_loop_unref(m_pgml);
	g_thread_unref(m_gthread);
}

void FDDialog::goClicked()
{
	qDebug() << "Go clicked() current row is " << m_pListWidget->currentRow();

	PipelineData *pipelineData = createPipeline(m_pListWidget->currentRow());
	FDHuntDialog *pdlg = new FDHuntDialog(pipelineData);
	pdlg->move(this->x(), this->y() + this->frameGeometry().height());

	// set up bus watch
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipelineData->pipeline));
	gst_bus_add_watch(bus, &FDDialog::busCallback, pipelineData);
	gst_object_unref(bus);

	pdlg->exec();

	// cleanup
	qDebug() << "Cleaning up pipeline...";
	bus = gst_pipeline_get_bus(GST_PIPELINE(pipelineData->pipeline));
	gst_bus_remove_watch(bus);
	gst_object_unref(bus);
	gst_element_set_state(pipelineData->pipeline, GST_STATE_NULL);
	gst_object_unref(pipelineData->pipeline);
	delete pipelineData;
	delete pdlg;
	qDebug() << "Cleanup done.";
}


PipelineData *FDDialog::createTestSrcPipeline()
{
	GstElement *pipeline, *src, *sink, *convert;
	PipelineData *pdata = new PipelineData;

	/* Create the elements */
	src = gst_element_factory_make ("videotestsrc", "source");
	Q_ASSERT(src);
	sink = gst_element_factory_make ("qwidget5videosink", "sink");
	Q_ASSERT(sink);
	convert = gst_element_factory_make("videoconvert", "convert");
	Q_ASSERT(convert);

	/* Create the empty pipeline */
	pipeline = gst_pipeline_new ("test-src-pipeline");
	Q_ASSERT(pipeline);

	pdata->pipeline = pipeline;
	pdata->bLoop = false;
	pdata->bWaitingForFlushingSeekCompletion = false;
	pdata->bPrerolled = false;
	pdata->bAudio = false;

	/* Build the pipeline */
	gst_bin_add_many (GST_BIN (pipeline), src, convert, sink, NULL);
	if (gst_element_link(src, convert) != TRUE)
	{
		qCritical() << "src-convert could not be linked.";
		gst_object_unref (pipeline);
		delete pdata;
		return NULL;
	}
	if (gst_element_link(convert, sink) != TRUE)
	{
		qCritical() << "convert-sink could not be linked.";
		gst_object_unref (pipeline);
		delete pdata;
		return NULL;
	}
	return pdata;
}

PipelineData *FDDialog::createTestSrcSolidColorPipeline()
{
	GstElement *pipeline, *src, *sink, *convert;
	PipelineData *pdata = new PipelineData;

	/* Create the elements */
	GError *gerror = NULL;
	QString s = QString("videotestsrc pattern=solid-color foreground-color=0xff808080");
	src = gst_parse_bin_from_description(s.toStdString().c_str(), true, &gerror);
	if (src == NULL || gerror != NULL)
	{
		qCritical() << "cannot create videotestsrc!";
		return NULL;
	}
	Q_ASSERT(src);

	sink = gst_element_factory_make ("qwidget5videosink", "sink");
	Q_ASSERT(sink);
	convert = gst_element_factory_make("videoconvert", "convert");
	Q_ASSERT(convert);

	/* Create the empty pipeline */
	pipeline = gst_pipeline_new ("test-src-solid-pipeline");
	Q_ASSERT(pipeline);

	pdata->pipeline = pipeline;
	pdata->bLoop = false;
	pdata->bWaitingForFlushingSeekCompletion = false;
	pdata->bPrerolled = false;
	pdata->bAudio = false;
	pdata->bVideo = true;

	/* Build the pipeline */
	gst_bin_add_many (GST_BIN (pipeline), src, convert, sink, NULL);
	if (gst_element_link(src, convert) != TRUE)
	{
		qCritical() << "src-convert could not be linked.";
		gst_object_unref (pipeline);
		delete pdata;
		return NULL;
	}
	if (gst_element_link(convert, sink) != TRUE)
	{
		qCritical() << "convert-sink could not be linked.";
		gst_object_unref (pipeline);
		delete pdata;
		return NULL;
	}
	return pdata;
}


PipelineData *FDDialog::createDecodebinImagePipeline()
{
	GstElement *pipeline, *src, *sink;
	PipelineData *pdata = new PipelineData;

	/* Create the empty pipeline */
	pipeline = gst_pipeline_new ("test-image-png-pipeline");
	Q_ASSERT(pipeline);

	pdata->pipeline = pipeline;
	pdata->bLoop = false;
	pdata->bWaitingForFlushingSeekCompletion = false;
	pdata->bPrerolled = false;
	pdata->bAudio = false;
	pdata->bVideo = true;

	/* Create the elements */
	src = gst_element_factory_make ("filesrc", "source");
	Q_ASSERT(src);
	g_object_set(G_OBJECT(src), "location", IMAGE_PNG_TIRE, NULL);

	GstElement *decodebin = gst_element_factory_make ("decodebin", "decodebin");
	g_signal_connect(decodebin, "pad-added", G_CALLBACK(&FDDialog::imagePadAdded), pdata);

	sink = gst_element_factory_make ("qwidget5videosink", "sink");
	Q_ASSERT(sink);


	/*
	 * Build the partial pipeline
	 * Note "sink" is left unconnected in the pipeline.
	 * padAdded method will link it. Must be created here or threading-related-errors occur.
	 */

	gst_bin_add_many (GST_BIN (pipeline), src, decodebin, sink, NULL);
	if (gst_element_link(src, decodebin) != TRUE)
	{
		qCritical() << "src-decodebin could not be linked.";
		gst_object_unref (pipeline);
		delete pdata;
		return NULL;
	}
	return pdata;
}

void FDDialog::imagePadAdded(GstElement *src, GstPad *newPad, PipelineData *pdata)
{
	qDebug() << "imagePadAdded: Got new pad '" << GST_PAD_NAME(newPad) << " from " << GST_ELEMENT_NAME(src);


	// get the caps and parse them. That will tell us whether its audio or video. If its video, then
	// we get the frame rate - that tells us whether its an image (rate=0) or video. We also get the
	// resolution.

	GstCaps *new_pad_caps = NULL;
	new_pad_caps = gst_pad_get_current_caps(newPad);
	bool isVideo=false, isImage=false, isAudio=false;
	int width=-1, height=-1;
	parseCaps(new_pad_caps, isVideo, isImage, width, height, isAudio);
	gst_caps_unref(new_pad_caps);

	qDebug() << "imagePadAdded: image - " << GST_ELEMENT_NAME(src) << " - link into pipeline";
	GstElement *videoConvert = gst_element_factory_make("videoconvert", "videoconvert");
	GstElement *freeze = gst_element_factory_make("imagefreeze", "imagefreeze");
	gst_bin_add_many(GST_BIN(pdata->pipeline), videoConvert, freeze, NULL);

	GstPad *sinkPad = gst_element_get_static_pad(videoConvert, "sink");
	gst_pad_link(newPad, sinkPad);
	gst_object_unref(sinkPad);
	gst_element_link (videoConvert, freeze);

	GstElement *sink = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "sink");
	Q_ASSERT(sink);
	gst_element_link (freeze, sink);

	// set states of all newly-added elements.
	if (!gst_element_sync_state_with_parent(videoConvert))
		qCritical() << "Cannot sync videoConvert with pipeline state";
	if (!gst_element_sync_state_with_parent(freeze))
		qCritical() << "Cannot sync freeze with pipeline state";
	if (!gst_element_sync_state_with_parent(sink))
		qCritical() << "Cannot sync sink with pipeline state";

	qDebug() << "imagePadAdded: image - " << GST_ELEMENT_NAME(src) << " - done.";
}



PipelineData *FDDialog::createDecodebinVideoPipeline(const char *movieFileName, bool bLoop, bool bAudio, bool bVideo)
{
	GstElement *pipeline, *src, *sink;

	/* Create the empty pipeline */
	pipeline = gst_pipeline_new ("test-video-mov-pipeline");
	Q_ASSERT(pipeline);
	PipelineData *pdata = new PipelineData;
	pdata->pipeline = pipeline;
	pdata->bLoop = bLoop;
	pdata->bWaitingForFlushingSeekCompletion = false;
	pdata->bPrerolled = false;
	pdata->bAudio = bAudio;
	pdata->bVideo = bVideo;

	/* Create the elements */
	src = gst_element_factory_make ("filesrc", "source");
	Q_ASSERT(src);
	g_object_set(G_OBJECT(src), "location", movieFileName, NULL);

	GstElement *decodebin = gst_element_factory_make ("decodebin", "decodebin");
	g_signal_connect(decodebin, "pad-added", G_CALLBACK(&FDDialog::videoPadAdded), pdata);

	sink = gst_element_factory_make ("qwidget5videosink", "sink");
	Q_ASSERT(sink);


	/*
	 * Build the partial pipeline
	 * Note "sink" is left unconnected in the pipeline.
	 * padAdded method will link it. Must be created here or threading-related-errors occur when trying to
	 * connect widget to sink.
	 */

	gst_bin_add_many (GST_BIN (pipeline), src, decodebin, sink, NULL);
	if (gst_element_link(src, decodebin) != TRUE)
	{
		qCritical() << "src-decodebin could not be linked.";
		gst_object_unref (pipeline);
		delete pdata;
		return NULL;
	}
	return pdata;
}

void FDDialog::videoPadAdded(GstElement *src, GstPad *newPad, PipelineData *pdata)
{
	qDebug() << "videoPadAdded: Got new pad " << GST_PAD_NAME(newPad) << " from " << GST_ELEMENT_NAME(src);


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
		qDebug() << "videoPadAdded - ignoring image pad";
		return;
	}
	else if (isVideo)
	{
		if (pdata->bVideo)
		{
			qDebug() << "videoPadAdded: video - " << GST_ELEMENT_NAME(src) << " - link into pipeline. Loop? " << pdata->bLoop;
			GstElement *videoConvert = gst_element_factory_make("videoconvert", "videoconvert");
			gst_bin_add_many(GST_BIN(pdata->pipeline), videoConvert, NULL);

			GstPad *sinkPad = gst_element_get_static_pad(videoConvert, "sink");
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);

			GstElement *sink = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "sink");
			Q_ASSERT(sink);
			gst_element_link (videoConvert, sink);

			// set states of all newly-added elements.
			if (!gst_element_sync_state_with_parent(videoConvert))
				qCritical() << "Cannot sync videoConvert with pipeline state";
			if (!gst_element_sync_state_with_parent(sink))
				qCritical() << "Cannot sync sink with pipeline state";

			gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &FDDialog::eventProbeDoNothingCB, (gpointer)pdata, NULL);

#if !DONT_USE_EVENT_PROBES
			// If looping, add event probe to videoConvert sink pad
			if (pdata->bLoop)
			{
				// add event probe
				qDebug() << "videoPadAdded: video - looping, so add event probe.";
				GstPad *sinkPad = gst_element_get_static_pad(videoConvert, "sink");
				pdata->probeID = gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &FDDialog::eventProbeCB, (gpointer)pdata, NULL);
				gst_object_unref(sinkPad);
			}
			else
				qDebug() << "videoPadAdded: video - not looping, no event probe.";
#else
			if (pdata->bLoop)
			{
				qDebug() << "Not using event probes. Will handle looping in bus callback.";
			}
#endif
		}
		else
		{
			// remove video sink from pipeline, create fakesink.
			qDebug() << "Ignoring video - remove videosink from pipeline.";
			GstElement *sink = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "sink");
			Q_ASSERT(sink);		// there should be a sink here now
			gst_bin_remove(GST_BIN(pdata->pipeline), sink);

#if USE_FAKESINK_WHEN_IGNORING
			// now create fakesink
			qDebug() << "Create fakesink, add to pipeline, and link";
			sink = gst_element_factory_make("fakesink", "fakesink-video");
			gst_bin_add(GST_BIN(pdata->pipeline), sink);

			// and link
			GstPad *sinkPad = gst_element_get_static_pad(sink, "sink");
			Q_ASSERT(sinkPad);
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);

			if (!gst_element_sync_state_with_parent(sink))
				qCritical() << "Cannot sync fakesink with pipeline state";
#endif
		}

		qDebug() << "videoPadAdded: video - " << GST_ELEMENT_NAME(src) << " - done.";
	}
	else if (isAudio)
	{
		if (!pdata->bAudio)
		{
#if !USE_FAKESINK_WHEN_IGNORING
			qDebug() << "videoPadAdded - ignoring audio pad";
#else
			qDebug() << "videoPadAdded - ignoring audio - use fakesink";
			GstElement *fakesink = gst_element_factory_make("fakesink", "fakesink");
			Q_ASSERT(fakesink);
			gst_bin_add_many(GST_BIN(pdata->pipeline), fakesink, NULL);
			GstPad *sinkPad = gst_element_get_static_pad(fakesink, "sink");
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);
			if (!gst_element_sync_state_with_parent(fakesink))
				qCritical() << "Cannot sync fakesink with pipeline state";
#endif
			return;
		}
		else
		{
			qDebug() << "videoPadAdded - link audio pad";

			// check for existing audio sink
			// If not found, then create it and the audiomixer->audioconvert
			GstElement *audioSink = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "audiosink");
			if (!audioSink)
			{
				audioSink = gst_element_factory_make("osxaudiosink", "audiosink");
				Q_ASSERT(audioSink);

//				g_object_set(G_OBJECT(audioSink), "async", (gboolean)false, NULL);		// ???
//																						// https://stackoverflow.com/questions/27547500/how-to-resume-playing-after-paused-using-gstreamer

#ifdef USE_AUDIOMIXER
				GstElement *audioMixer = gst_element_factory_make("audiomixer", "audiomixer");
				Q_ASSERT(audioMixer);
				gst_bin_add_many(GST_BIN(pdata->pipeline), audioMixer, audioSink, NULL);
				gst_element_link(audioMixer, audioSink);
			}
#else
				gst_bin_add_many(GST_BIN(pdata->pipeline), audioSink, NULL);

				if (!gst_element_sync_state_with_parent(audioSink))
					qCritical() << "Cannot sync audioSink with pipeline state";
			}
#endif

#if !DONT_USE_EVENT_PROBES
			// event probe on audiosink
			if (pdata->bLoop)
			{
				// add event probe
				qDebug() << "videoPadAdded: video - looping, so add event probe.";
				GstPad *sinkPad = gst_element_get_static_pad(audioSink, "sink");
				pdata->probeID = gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &FDDialog::audioSinkEventProbeCB, (gpointer)pdata, NULL);
				gst_object_unref(sinkPad);
			}
#else
			if (pdata->bLoop)
			{
				qDebug() << "Not using event probes. Will handle looping in bus callback.";
			}
#endif

			// create elements for audio stream
			GstElement *audioresample = gst_element_factory_make("audioresample", "audioresample");
			Q_ASSERT(audioresample);
			GstElement *audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
			Q_ASSERT(audioconvert);
			GstElement *volume = gst_element_factory_make("volume", "volume");
			Q_ASSERT(volume);
			g_object_set(G_OBJECT(volume), "volume", 0.25, NULL);		// set volume to some low level

			gst_bin_add_many(GST_BIN(pdata->pipeline), audioresample, audioconvert, volume, NULL);

			// resample->audioconvert GstPad *pad = gst_element_get_static_pad(audioresample, "sink");
			GstPad *pad = gst_element_get_static_pad(audioconvert, "sink");
			gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &FDDialog::eventProbeAudioDoNothingCB, (gpointer)pdata, NULL);

			Q_ASSERT(pad);
			gst_pad_link(newPad, pad);
			gst_object_unref(pad);

			gst_element_link(audioconvert, audioresample);
			gst_element_link(audioresample, volume);
			// resample->audioconvert gst_element_link(audioresample, audioconvert);
			// resample->audioconvert gst_element_link(audioconvert, volume);

#ifdef USE_AUDIOMIXER
			GstElement *audioMixer = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "audiomixer");
			Q_ASSERT(audioMixer);
			gst_element_link(volume, audioMixer);	// audioMixer already linked with audioSink
			if (!gst_element_sync_state_with_parent(audioMixer))
				qCritical() << "Cannot sync audioMixer with pipeline state";
			if (!gst_element_sync_state_with_parent(audioSink))
				qCritical() << "Cannot sync audioSink with pipeline state";
#else
			gst_element_link(volume, audioSink);
#endif
			if (!gst_element_sync_state_with_parent(audioresample))
				qCritical() << "Cannot sync audioresample with pipeline state";
			if (!gst_element_sync_state_with_parent(audioconvert))
				qCritical() << "Cannot sync audioconvert with pipeline state";
			if (!gst_element_sync_state_with_parent(volume))
				qCritical() << "Cannot sync volume with pipeline state";
		}
	}
}


void FDDialog::stimPadAdded(GstElement *src, GstPad *newPad, PipelineData *pdata)
{
	qDebug() << "stimPadAdded: Got new pad " << GST_PAD_NAME(newPad) << " from " << GST_ELEMENT_NAME(src);


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
		qDebug() << "stimPadAdded - ignoring image pad";
		return;
	}
	else if (isVideo)
	{
		if (pdata->bVideo)
		{
			qDebug() << "stimPadAdded: video - " << GST_ELEMENT_NAME(src) << " - link into pipeline. Loop? " << pdata->bLoop;
			GstElement *videoConvert = gst_element_factory_make("videoconvert", "videoconvert");
			gst_bin_add_many(GST_BIN(pdata->pipeline), videoConvert, NULL);

			GstPad *sinkPad = gst_element_get_static_pad(videoConvert, "sink");
			gst_pad_link(newPad, sinkPad);
			gst_object_unref(sinkPad);

			GstElement *sink = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "sink");
			Q_ASSERT(sink);
			gst_element_link (videoConvert, sink);

			// set states of all newly-added elements.
			if (!gst_element_sync_state_with_parent(videoConvert))
				qCritical() << "Cannot sync videoConvert with pipeline state";
			if (!gst_element_sync_state_with_parent(sink))
				qCritical() << "Cannot sync sink with pipeline state";

			if (pdata->bLoop)
			{
				qDebug() << "Not using event probes. Will handle looping in bus callback.";
			}
		}
		else
		{
			// remove video sink from pipeline, create fakesink.
			qDebug() << "Ignoring video - remove videosink from pipeline.";
			GstElement *sink = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "sink");
			Q_ASSERT(sink);		// there should be a sink here now
			gst_bin_remove(GST_BIN(pdata->pipeline), sink);

		}

		qDebug() << "stimPadAdded: video - " << GST_ELEMENT_NAME(src) << " - done.";
	}
	else if (isAudio)
	{
		if (!pdata->bAudio)
		{
			qDebug() << "stimPadAdded - ignoring audio pad";
			return;
		}
		else
		{
			qDebug() << "stimPadAdded - link audio pad";

			// check for existing audio sink
			// If not found, then create it and the audiomixer->audioconvert
			GstElement *audioSink = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "audiosink");
			if (!audioSink)
			{
				audioSink = gst_element_factory_make("osxaudiosink", "audiosink");
				Q_ASSERT(audioSink);

#ifdef USE_AUDIOMIXER
				GstElement *audioMixer = gst_element_factory_make("audiomixer", "audiomixer");
				Q_ASSERT(audioMixer);
				gst_bin_add_many(GST_BIN(pdata->pipeline), audioMixer, audioSink, NULL);
				gst_element_link(audioMixer, audioSink);

				if (!gst_element_sync_state_with_parent(audioMixer))
					qCritical() << "Cannot sync audioMixer with pipeline state";
				if (!gst_element_sync_state_with_parent(audioSink))
					qCritical() << "Cannot sync audioSink with pipeline state";
			}
#else
				gst_bin_add_many(GST_BIN(pdata->pipeline), audioSink, NULL);

				if (!gst_element_sync_state_with_parent(audioSink))
					qCritical() << "Cannot sync audioSink with pipeline state";
			}
#endif

#if !DONT_USE_EVENT_PROBES
			// event probe on audiosink
			if (pdata->bLoop)
			{
				// add event probe
				qDebug() << "videoPadAdded: video - looping, so add event probe.";
				GstPad *sinkPad = gst_element_get_static_pad(audioSink, "sink");
				pdata->probeID = gst_pad_add_probe (sinkPad, GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM, &FDDialog::audioSinkEventProbeCB, (gpointer)pdata, NULL);
				gst_object_unref(sinkPad);
			}
#else
			if (pdata->bLoop)
			{
				qDebug() << "Not using event probes. Will handle looping in bus callback.";
			}
#endif

			// create elements for audio stream
			GstElement *audioresample = gst_element_factory_make("audioresample", "audioresample");
			Q_ASSERT(audioresample);
			GstElement *audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
			Q_ASSERT(audioconvert);
			GstElement *volume = gst_element_factory_make("volume", "volume");
			Q_ASSERT(volume);
			g_object_set(G_OBJECT(volume), "volume", 0.25, NULL);		// set volume to some low level

			gst_bin_add_many(GST_BIN(pdata->pipeline), audioresample, audioconvert, volume, NULL);

			// resample->audioconvert GstPad *pad = gst_element_get_static_pad(audioresample, "sink");
			GstPad *pad = gst_element_get_static_pad(audioconvert, "sink");
			Q_ASSERT(pad);
			gst_pad_link(newPad, pad);
			gst_object_unref(pad);

			gst_element_link(audioconvert, audioresample);
			gst_element_link(audioresample, volume);
			// resample->audioconvert gst_element_link(audioresample, audioconvert);
			// resample->audioconvert gst_element_link(audioconvert, volume);

#ifdef USE_AUDIOMIXER
			GstElement *audioMixer = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "audiomixer");
			Q_ASSERT(audioMixer);
			gst_element_link(volume, audioMixer);	// audioMixer already linked with audioSink
#else
			gst_element_link(volume, audioSink);
#endif
			if (!gst_element_sync_state_with_parent(audioresample))
				qCritical() << "Cannot sync audioresample with pipeline state";
			if (!gst_element_sync_state_with_parent(audioconvert))
				qCritical() << "Cannot sync audioconvert with pipeline state";
			if (!gst_element_sync_state_with_parent(volume))
				qCritical() << "Cannot sync volume with pipeline state";
		}
	}
}



GstPadProbeReturn FDDialog::eventProbeDoNothingCB(GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	PipelineData *pdata = (PipelineData *)p;

	// Note: GST_PAD_PROBE_INFO_EVENT(d) returns a GstEvent* or NULL if info->data does not have an event.
	// In this routine, because its an event probe, it should always contain an event?

	GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
	if (event)
	{
		GstElement* parent = GST_PAD_PARENT(pad);
		qDebug() << "eventProbeDoNothingCB: Event type " << GST_EVENT_TYPE_NAME(event);
	}
	return GST_PAD_PROBE_OK;
}

GstPadProbeReturn FDDialog::eventProbeAudioDoNothingCB(GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	PipelineData *pdata = (PipelineData *)p;

	// Note: GST_PAD_PROBE_INFO_EVENT(d) returns a GstEvent* or NULL if info->data does not have an event.
	// In this routine, because its an event probe, it should always contain an event?

	GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
	if (event)
	{
		GstElement* parent = GST_PAD_PARENT(pad);
		qDebug() << "eventProbeAudioDoNothingCB: Event type " << GST_EVENT_TYPE_NAME(event);
	}
	return GST_PAD_PROBE_OK;
}

#if !DONT_USE_EVENT_PROBES
GstPadProbeReturn FDDialog::eventProbeCB (GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	PipelineData *pdata = (PipelineData *)p;

	// Note: GST_PAD_PROBE_INFO_EVENT(d) returns a GstEvent* or NULL if info->data does not have an event.
	// In this routine, because its an event probe, it should always contain an event?

	GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
	if (event)
	{
		GstElement* parent = GST_PAD_PARENT(pad);
		if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT)
		{
			gchar *parentName = gst_element_get_name(parent);
			qDebug() << "eventProbeCB(" << parentName << "): got SEGMENT event";
			// segment info
			const GstSegment* segment;
			gst_event_parse_segment(event, &segment);
			qDebug() << "eventProbeCB(" << parentName << "): segment base: " << segment->base
					<< " offset: " << segment->offset << " start: " << segment->start << " stop: " << segment->stop
					<< " time: " << segment->time << " position: " << segment->position
					<< " duration: " << segment->duration << " format: " << gst_format_get_name(segment->format);
			g_free(parentName);

			if (pdata->bWaitingForFlushingSeekCompletion)
			{
				pdata->bPrerolled = true;
				qDebug() << "Prerolling done.";
			}

		}
		else if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT_DONE)
		{
			gchar *parentName = gst_element_get_name(parent);
			qDebug() << "eventProbeCB(" << parentName << ") got SEGMENT_DONE event";
			// do segment seek
			qDebug() << "eventProbeCB(" << parentName << "): Running time is " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));

#if DO_FLUSHING_SEGMENT_SEEK_IN_BUS_CALLBACK
			qDebug() << "eventProbeCB(" << parentName << "): do segment seek";
#else
			qDebug() << "eventProbeCB(" << parentName << "): do segment seek";

			//FDHuntDialog::printPipelineState(pdata->pipeline);

			// get decodebin element
			GstElement *decodebin = gst_bin_get_by_name(GST_BIN(pdata->pipeline), "decodebin");
			Q_ASSERT(decodebin);
			//if (!gst_element_seek(decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			if (!gst_element_seek(decodebin, 1.0, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT), GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
			{
				qCritical() << "SEEK FAILED";
			}
			else
				qCritical() << "SEEK OK";
			gst_object_unref(decodebin);
#endif
			g_free(parentName);
		}
	}
	return GST_PAD_PROBE_OK;
}

GstPadProbeReturn FDDialog::audioSinkEventProbeCB (GstPad * pad, GstPadProbeInfo * info, gpointer p)
{
	PipelineData *pdata = (PipelineData *)p;

	// Note: GST_PAD_PROBE_INFO_EVENT(d) returns a GstEvent* or NULL if info->data does not have an event.
	// In this routine, because its an event probe, it should always contain an event?

	GstEvent* event = GST_PAD_PROBE_INFO_EVENT(info);
	if (event)
	{
		qDebug() << "audioSinkEventProbeCB: got event type " << GST_EVENT_TYPE_NAME(event);
		GstElement* parent = GST_PAD_PARENT(pad);
		if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT)
		{
			gchar *parentName = gst_element_get_name(parent);
			qDebug() << "audioSinkEventProbeCB(" << parentName << "): got SEGMENT event";
			// segment info
			const GstSegment* segment;
			gst_event_parse_segment(event, &segment);
			qDebug() << "audioSinkEventProbeCB(" << parentName << "): segment base: " << GST_TIME_AS_MSECONDS(segment->base)
					<< " offset: " << GST_TIME_AS_MSECONDS(segment->offset) << " start: " << GST_TIME_AS_MSECONDS(segment->start) << " stop: " << GST_TIME_AS_MSECONDS(segment->stop)
					<< " time: " << GST_TIME_AS_MSECONDS(segment->time) << " position: " << GST_TIME_AS_MSECONDS(segment->position)
					<< " duration: " << GST_TIME_AS_MSECONDS(segment->duration) << " format: " << gst_format_get_name(segment->format);
			g_free(parentName);

			if (pdata->bWaitingForFlushingSeekCompletion)
			{
				pdata->bPrerolled = true;
				qDebug() << "Prerolling done.";
			}

		}
		else if (GST_EVENT_TYPE(event) == GST_EVENT_SEGMENT_DONE)
		{
			gchar *parentName = gst_element_get_name(parent);
			qDebug() << "audioSinkEventProbeCB(" << parentName << ") got SEGMENT_DONE event";
			// do segment seek
			qDebug() << "audioSinkEventProbeCB(" << parentName << "): Running time is " << (gst_clock_get_time(gst_element_get_clock(parent)) - gst_element_get_base_time(parent));
			g_free(parentName);

			FDHuntDialog::printPipelineState(pdata->pipeline);
		}
	}
	return GST_PAD_PROBE_OK;
}
#endif    // DONT_USE_EVENT_PROBES

void FDDialog::parseCaps(GstCaps* caps, bool& isVideo, bool& isImage, int& width, int& height, bool& isAudio)
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
