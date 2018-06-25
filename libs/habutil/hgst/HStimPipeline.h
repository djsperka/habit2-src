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
	bool bWaitingForPrerollSegment;	// true when flushing segment seek issued, but segment not received. Used in event probe.
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

	bool m_bInitialized;
	QDir m_dirStimRoot;
	//Habit::StimulusDisplayInfo m_stimulusDisplayInfo;
	//const HStimulusLayoutType& m_stimulusLayoutType;
	//bool m_bISS;
	bool m_bRewindPending;	// true when expecting ASYNC_DONE due to rewind() call
	GstElement *m_pipeline;
	QMutex m_mutex;
	QMap<HPlayerPositionType, HStimPipelineSource* > m_mapPipelineSources;

	// Add the stimulus contained in 'info' to 'pipeline' at position 'ppt'.
	// The bool vars 'bVideo' and 'bAudio' dictate whether to IGNORE any streams of their respective types.
	// This only applies to stim that are not static color-type, for which we only have a filename and a position
	// type (i.e. Center/Left/Right, which imply video/image, and Sound, which implies... sound). The input stim
	// files are fed to a decodebin, and the 'pad-added' signal is connected to our 'padAdded' method. In that method
	// stream types can be deduced from the caps. Note that audio streams may be ignored for either of two reasons:
	// if 'bAudio' is false, OR if 'bAudio' is true, but the sound is muted in 'info' (i.e. it is set to 0).

	HStimPipelineSource *addStimulusInfo(const HPlayerPositionType& ppt, const Habit::StimulusInfo& info, bool bAudio);
	void emitNowPlaying();
	void dumpGstStateChangeReturn(const QString& s, GstStateChangeReturn r);

public:
	bool bInitialFlushingSeekDone;
	int m_iAsyncPause;
	HStimPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const Habit::StimulusDisplayInfo& info, const QDir& stimRoot, QObject *parent);
	virtual ~HStimPipeline();

	// create pipeline (each of its partial pipelines for single/dual screen and/or audio created and linked).
	// The pipeline is left in the NULL state. Where decodebin is used, there is just a filesrc!decodebin partial
	// pipeline, with a sink as well (created initially to avoid threading problems later).
	virtual void initialize();

	// free all pipeline resources. This SHOULD also close all file descriptors, though there is a case or two where
	// a pair of descriptors are left open (and later re-used -- so there doesn't appear to be a growing list of
	// open descriptors)
	virtual void cleanup();
	virtual void preroll() { pause(); };

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


	void detachWidgetsFromSinks();	// default is a no-op
	void attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1=NULL);		// default is a no-op
	void setWidgetPropertyOnSink(HVideoWidget *w, const HPlayerPositionType& ppt);

	GstElement *pipeline() const { return m_pipeline; }
	QMap<HPlayerPositionType, HStimPipelineSource* >& getPipelineSourceMap() { return m_mapPipelineSources; }
	QMutex *mutex() { return &m_mutex; };

	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata);

	// Callback function for 'pad-added' signal from decodebin elements
	static void padAdded(GstElement *src, GstPad *newPad, gpointer p);

	static GstPadProbeReturn eventProbeCB(GstPad * pad, GstPadProbeInfo * info, gpointer p);

};

HPipeline* HStimPipelineFactory(int id, const Habit::StimulusSettings& stimulusSettings, const Habit::StimulusDisplayInfo& info, const QDir& stimRoot, bool bStatic, QObject *parent);

#endif /* LIBS_HABUTIL_HGST_HSTIMPIPELINE_H_ */
