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

#include <glib.h>
#include <gst/gst.h>
//#include <gst/video/videooverlay.h>
//#include <gst/videotestsrc/gstvideotestsrc.h>


#define CREATE_BEFORE_PAD_ADDED 1


QString stateName(GstState s);


class HGMMPipeline: public QObject
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
		GstElement *freeze;
		HVideoWidget *videoWidget;
		QSize size;		// stimulus size, as found in file
		bool needPad;
		bool isVideo;
		bool isLoop;
		bool isLoopingStarted;
		PipelineData() : sink(NULL), convert(NULL), identity(NULL), freeze(NULL), videoWidget(NULL), size(), needPad(true), isVideo(false), isLoop(false), isLoopingStarted(false) {};
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
	int id() const { return m_id; };
	GstPipeline *pipeline() { return GST_PIPELINE(m_pipeline); };
	void play();
	void pause();
	const Habit::StimulusSettings& stimulusSettings() const { return m_stimulus; };
	GstElement* sink(const HPlayerPositionType& ppt) const;

	// set the "widget" property for each sink to NULL
	void detachWidgetsFromSinks();

	// set the "widget" property for each sink to its respective widget value
	void attachWidgetsToSinks();

	// (static) utility functions
	static QString makeElementName(const char *factoryName, const HPlayerPositionType& ppt, int number);
	static bool parseElementName(const QString& elementName, QString& factoryName, const HPlayerPositionType*& pppt, int& id);
	static GstElement *makeElement(const char *factoryName, const HPlayerPositionType& ppt, int number);
	static QString stateName(GstState s);
	static const HPlayerPositionType& getPPTFromElementName(const QString& elementName);

private:
	int m_id;
	const HStimulusLayoutType *m_pStimulusLayoutType;
	Habit::StimulusSettings m_stimulus;
	QDir m_root;
	bool m_bISS;
	GstElement *m_pipeline;

	QMap<HPlayerPositionType, PipelineData>& getPipelineDataMap() { return m_mapPipelineData; };

	// called from static busCallback to emit nowPlaying signal
	void emitNowPlaying();

	static void padAdded(GstElement *src, GstPad *new_pad, PipelineData *pdata);

	static gboolean busCallback(GstBus *bus, GstMessage *msg, gpointer pdata);

	// add stimulus to this helper
	void addMedia(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

	// use videotestsrc, static (known) sink.
	void setSolidColorStimulus(const QColor& color, const HPlayerPositionType& ppt);

	// use decodebin and async sink pad creation
	void setUnknownStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

	// jpg
	void setJPGStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);

	// png
	void setPNGStimulus(const Habit::StimulusInfo& info, const HPlayerPositionType& ppt);


	Q_SIGNALS:
		void nowPlaying();
};




#endif /* LIBS_HABUTIL_HGST_HGMMPIPELINE_H_ */
