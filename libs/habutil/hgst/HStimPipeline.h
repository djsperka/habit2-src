/*
 * HStimPipeline.h
 *
 *  Created on: Dec 14, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HSTIMPIPELINE_H_
#define LIBS_HABUTIL_HGST_HSTIMPIPELINE_H_

#include <hgst/HPipeline.h>
#include <QMutex>
#include <QDir>
#include <QMap>
#include <QSize>
#include "stimulussettings.h"
#include "stimulusdisplayinfo.h"
#include "HTypes.h"
#include <gst/gst.h>

class HStimPipeline;

class HStimPipelineSource
{
	HStimPipeline *m_pStimPipeline;

public:
	// these are public because I'm being lazy
	bool bWaitingForPreroll;		// i.e. waiting for ASYNC_DONE after initial flushing segment seek. Looping sources only.
	bool bPrerolled;
	//bool bWaitingForPrerollSegment;	// true when flushing segment seek issued, but segment not received. Used in event probe.
	bool bWaitingForSegment;		// true when non-flushing segment seek issued, but segment not received. Used in event probe.
	bool bWaitingForSegment2;
	QString sWaitingForSegment2Pad;
	bool bAudio;
	bool bVideo;
	bool bLoop;
	float volume;
	QSize size;		// stim size for video/image streams
	unsigned int nPadsLinked;

	HStimPipelineSource(HStimPipeline *pipe);
	~HStimPipelineSource();
	HStimPipeline *stimPipeline() { return m_pStimPipeline; };
	GstElement *pipeline();
};

class HStimPipeline: public HPipeline
{
	Q_OBJECT

	bool m_bInitialized;	// on creation, not initialized. Init'd on first call to any of pause(), preroll(), play().
	QDir m_dirStimRoot;
	bool m_bUsingMixer;		// check isUsingMixer() in padAdded() to see if audio streams connect to mixer or not
	bool m_bRewindPending;	// true when expecting ASYNC_DONE due to rewind() call
	GstElement *m_pipeline;
	QMutex m_mutex;
	QMap<HPlayerPositionType, HStimPipelineSource* > m_mapPipelineSources;

	// Add the stimulus contained in 'info' to 'pipeline' at position 'ppt'. This method kicks off the construction
	// of the pipeline, setting up the filesrc and decodebin, but not performing any change-of-state.
	// If 'bAudio' is true, then the pipeline will incorporate any audio stream from this source into the
	// pipeline as well. If 'bAudio' is false, then any audio stream is ignored. Obviously this seems silly
	// in the event of a Sound stimulus, but we can have a non-NULL sound source in an experiment that doesn't
	// use ISS. In such an experiment, however, the user might be using a sound-only attention-getter. IN that case,
	// the caller must handle the logic appropriately.
	HStimPipelineSource *addStimulusInfo(const HPlayerPositionType& ppt, const Habit::StimulusInfo& info, bool bAudio);

	// the sink carries a 'widget' property. Setting it to the widget pointer makes the video stream appear on that widget.
	void setWidgetPropertyOnSink(HVideoWidget *w, const HPlayerPositionType& ppt);

	void emitNowPlaying();
	void emitPrerolling();
	void emitPrerolled();
	void emitScreen(int ppid);
	void dumpGstStateChangeReturn(const QString& s, GstStateChangeReturn r);

public:
	bool bInitialFlushingSeekDone;
	int m_iAsyncPause;
	HStimPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const Habit::StimulusDisplayInfo& info, const PPTWIdMap& pwMap, const QDir& stimRoot, QObject *parent);
	virtual ~HStimPipeline();

	// create pipeline (each of its partial pipelines for single/dual screen and/or audio created and linked).
	// The pipeline is left in the NULL state. Where decodebin is used, there is just a filesrc!decodebin partial
	// pipeline, with a sink as well (created initially to avoid threading problems later).
	virtual void initialize();

	// free all pipeline resources. This SHOULD also close all file descriptors, though there is a case or two where
	// a pair of descriptors are left open (and later re-used -- so there doesn't appear to be a growing list of
	// open descriptors)
	virtual void cleanup();
	virtual bool isStatic() { return false; };

	// preroll pipeline
	virtual void preroll() { pause(); Q_EMIT prerolling(this->id()); };

	// set to pipeline to playing state. The bus callback function will emit 'nowPlaying' signal
	// when state change is complete.
	virtual void play();

	// set state of pipeline to paused. The pipeline should be prerolled when done, though that is done asynchronously
	// and isn't done when this call returns. If any of the partial pipelines require looping, that is also prepared
	// during the preroll process.
	virtual void pause();

	// set the stimulus to play from the beginning, first set it to paused, issue flushing seek, wait for ASYNC)
	// _DONE in bus handler.
	virtual void rewind();

	// set GST_DEBUG_DUMP_DOT_DIR and call this for dot file of current pipeline
	virtual void dump();

	// manage widget property on video sink element
	void detachWidgetsFromSinks();	// default is a no-op
	void attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1=NULL, HVideoWidget *w2=NULL);		// default is a no-op

	// return the GstElement corresponding to the entire pipeline
	GstElement *pipeline() const { return m_pipeline; }

	bool getUsingMixer() const { return m_bUsingMixer; };

	QMap<HPlayerPositionType, HStimPipelineSource* >& getPipelineSourceMap() { return m_mapPipelineSources; }
	QMutex *mutex() { return &m_mutex; };

	// callback functions for gstreamer

	// bus sync handler
	static GstBusSyncReply busSyncHandler (GstBus * bus, GstMessage * message, gpointer user_data);

	// bus callback - bus messages here
	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata);

	// Callback function for 'pad-added' signal from decodebin elements
	static void padAdded(GstElement *src, GstPad *newPad, gpointer p);

	// event probe for events passing through the pipeline
	static GstPadProbeReturn eventProbeCB(GstPad * pad, GstPadProbeInfo * info, gpointer p);

};

HPipeline* HStimPipelineFactory(int id, const Habit::StimulusSettings& stimulusSettings, const Habit::StimulusDisplayInfo& info, const PPTWIdMap& pwMap, const QDir& stimRoot, bool bStatic, QObject *parent);

#endif /* LIBS_HABUTIL_HGST_HSTIMPIPELINE_H_ */
