/*
 * HStimulusPipeline.h
 *
 *  Created on: Nov 14, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HSTIMULUSPIPELINE_H_
#define LIBS_HABUTIL_HGST_HSTIMULUSPIPELINE_H_


#include <QtDebug>
#include <QMutex>
#include "stimulussettings.h"
#include "HPipeline.h"
#include <gst/gst.h>

class HStimulusPipeline: public HPipeline
{
	bool m_bInitialized;
	Habit::StimulusSettings m_stimulusSettings;
	QDir m_dirStimRoot;
	const HStimulusLayoutType& m_stimulusLayoutType;
	bool m_bISS;
	GstElement *m_pipeline;
	GstElement *m_audioMixer;
	GstElement *m_audioSink;
	bool m_bAudioElementsAddedToPipeline;
	QMutex m_mutex;



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
		unsigned long probeID;				// when looping, this is the id of the event probe that handles it
		GstElement *probeElement;			// when looping, this is the element where the probe is installed
		int volume;							// volume level, 0-100.
		QSize size;							// resolution of video, if any. Can be undefined -- will take any shape of target screen (i.e.full screen)

		BinData(HStimulusPipeline *pipeline, const HPlayerPositionType& pptype);
	};

	QMap<HPlayerPositionType, BinData* > m_mapBinData;



	// called from static busCallback to emit nowPlaying signal
	void emitNowPlaying();

	void setWidgetPropertyOnSink(HVideoWidget *w, const HPlayerPositionType& ppt);
	void addStimulusInfo(BinData *pdata, const Habit::StimulusInfo& info, const QDir& stimRoot, const HPlayerPositionType& ppt);
	void setSizeOnWidget(HVideoWidget *w, const HPlayerPositionType& ppt);
	void lazyAudioInitializeAndAddToPipeline();

	Q_DISABLE_COPY(HStimulusPipeline);

public:
	HStimulusPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bISS, QObject *parent);
	virtual ~HStimulusPipeline();

	void initialize();
	void cleanup();

//	void null();
	void ready();
	void pause();
	void play();
	void preroll();
	void rewind();
	void dump();
	void attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1);
	void detachWidgetsFromSinks();
	virtual void write(std::ostream& os) const;

	GstElement *pipeline() const { return m_pipeline; }
	bool iss() const { return m_bISS; }
	const HStimulusLayoutType& stimulusLayoutType() const { return m_stimulusLayoutType; }
	QMap<HPlayerPositionType, BinData* >& getBinDataMap() { return m_mapBinData; }
	QMutex *mutex() { return &m_mutex; };

	//parse stream type and resolution from GstCaps
	static void parseCaps(GstCaps* caps, bool& isVideo, bool& isImage, int& width, int& height, bool& isAudio);

	// Callback function for 'pad-added' signal from decodebin elements
	static void padAdded(GstElement *src, GstPad *newPad, gpointer p);

	// bus callback function. Used to act on state changed, ASYNC_DONE, ...
	static gboolean busCallback(GstBus *, GstMessage *msg, gpointer pdata);

	// event probe callback, used to detect and act on segment_done events for looping sources
	static GstPadProbeReturn eventProbeCB (GstPad * pad, GstPadProbeInfo * info, gpointer p);
};


HPipeline* HStimulusPipelineFactory(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bSound, bool bISS, QObject *parent);





#endif /* LIBS_HABUTIL_HGST_HSTIMULUSPIPELINE_H_ */
