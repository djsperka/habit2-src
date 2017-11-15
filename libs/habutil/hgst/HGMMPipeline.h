/*
 * HGMMPipeline.h
 *
 *  Created on: Oct 23, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HGMMPIPELINE_H_
#define LIBS_HABUTIL_HGST_HGMMPIPELINE_H_

#include <QObject>
#include <QDir>
#include <QMap>
#include "HTypes.h"
#include "stimulussettings.h"
#include "HVideoWidget.h"
#include "HPipeline.h"

#include <glib.h>
#include <gst/gst.h>
//#include <gst/video/videooverlay.h>
//#include <gst/videotestsrc/gstvideotestsrc.h>


#define CREATE_BEFORE_PAD_ADDED 1


QString stateName(GstState s);


class HGMMPipeline: public HPipeline
{
	Q_OBJECT


	// this struct holds the information for a single stimulus and its portion of the pipeline
	// In other words, a single stimulus (image, movie, movie+sound, or sound) is encoded in the pipeline
	// using the elements here.
	//
	struct PipelineData
	{
		GstElement *pipeline;
		GstElement *sink;
		GstElement *convert;
		GstElement *identity;
		GstElement *volume;	// sound only
		GstElement *freeze;
		// no longer used HVideoWidget *videoWidget;
		QSize size;		// stimulus size, as found in file
		bool needPad;
		bool isVideo;
		bool isLoop;
		bool isAudio;
		//bool isPrerolled;
		PipelineData() : sink(NULL), convert(NULL), identity(NULL), freeze(NULL), size(), needPad(true), isVideo(false), isLoop(false), isAudio(false) {};
	};

	QMap<HPlayerPositionType, PipelineData> m_mapPipelineData;

public:
	HGMMPipeline(QObject *parent=NULL);
	HGMMPipeline(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, HVideoWidget *pCenter, bool bISS=false, QObject *parent=NULL);
	HGMMPipeline(int id, const Habit::StimulusSettings& stimulus, const QDir& stimRoot, HVideoWidget *pLeft, HVideoWidget *pRight, bool bISS=false, QObject *parent=NULL);
	HGMMPipeline(const HGMMPipeline& h);
	virtual ~HGMMPipeline();

	HGMMPipeline& operator=(const HGMMPipeline& rhs);
	bool hasPads() const;
	//int id() const { return m_id; };
	GstPipeline *pipeline() { return GST_PIPELINE(m_pipeline); };
	virtual void play();
	virtual void pause();
	virtual void preroll();
	virtual void ready();
	virtual void rewind();

	virtual void detachWidgetsFromSinks();	// default is a no-op
	virtual void attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1=NULL);

	virtual const Habit::StimulusSettings& stimulusSettings() const { return m_stimulus; };
	GstElement* sink(const HPlayerPositionType& ppt) const;

	// (static) utility functions
	static QString makeElementName(const char *factoryName, const HPlayerPositionType& ppt, int number);
	static bool parseElementName(const QString& elementName, QString& factoryName, const HPlayerPositionType*& pppt, int& id);
	static GstElement *makeElement(const char *factoryName, const HPlayerPositionType& ppt, int number);
	static QString stateName(GstState s);
	static const HPlayerPositionType& getPPTFromElementName(const QString& elementName);

	static HPipeline *createHGMMPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bSound, bool bISS, QObject *parent);

private:
	const HStimulusLayoutType& m_stimulusLayoutType;
	Habit::StimulusSettings m_stimulus;
	QDir m_root;
	bool m_bISS;
	GstElement *m_pipeline;
	bool isPrerolled;
	bool isPrerollPending;

	QMap<HPlayerPositionType, PipelineData>& getPipelineDataMap() { return m_mapPipelineData; };

	// called from static busCallback to emit nowPlaying signal
	void emitNowPlaying();

	static void padAdded(GstElement *src, GstPad *new_pad, PipelineData *pdata);

	static gboolean busCallback(GstBus *bus, GstMessage *msg, gpointer pdata);

	static GstPadProbeReturn eventProbeCB (GstPad * pad, GstPadProbeInfo * info, gpointer user_data);


	// add stimulus to this helper
	void addMedia(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

	// add sound stimulus to this helper
	void setSoundStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

	// use videotestsrc, static (known) sink.
	void setSolidColorStimulus(const QColor& color, const HPlayerPositionType& ppt);

	// use decodebin and async sink pad creation
	void setUnknownStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

	// jpg
	void setJPGStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

	// png
	void setPNGStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

//
//	Q_SIGNALS:
//		void nowPlaying();
};




#endif /* LIBS_HABUTIL_HGST_HGMMPIPELINE_H_ */
