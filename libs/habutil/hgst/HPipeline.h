/*
 * HPipeline.h
 *
 *  Created on: Nov 1, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HPIPELINE_H_
#define LIBS_HABUTIL_HGST_HPIPELINE_H_


#include <gst/gst.h>
#include "stimulussettings.h"
#include "stimulusdisplayinfo.h"
#include <ostream>
#include <QWidget>

#define C_STR(x) x.toStdString().c_str()

typedef QMap<int, WId> PPTWIdMap;

class HVideoWidget;
class HPipeline: public QObject
{
	Q_OBJECT

	int m_id;
	Habit::StimulusSettings m_ss;
	Habit::StimulusDisplayInfo m_sdinfo;
	PPTWIdMap m_wid;			// ppt - to - WId mapping for gstreamer bus sync callback

	Q_DISABLE_COPY(HPipeline);

public:
	HPipeline(int id, const Habit::StimulusSettings& ss, const Habit::StimulusDisplayInfo& info, const PPTWIdMap& pptwidMap, QObject *parent=NULL);

	int id() const { return m_id; };
	const Habit::StimulusSettings& stimulusSettings() const { return m_ss; };
	bool iss() const { return m_sdinfo.getUseISS(); }
	const HStimulusLayoutType& stimulusLayoutType() const { return m_sdinfo.getStimulusLayoutType(); }
	const Habit::StimulusDisplayInfo& getStimulusDisplayInfo() const { return m_sdinfo; }
	// must cleanup and initialize again after calling this
	void setStimulusSettings(const Habit::StimulusSettings& ss) { m_ss = ss; }

	void reconfigure(const Habit::StimulusDisplayInfo& info);

	// pipeline control
	virtual void initialize() = 0;
	virtual void cleanup() = 0;
	virtual bool isStatic() = 0;

	// open files (if needed), begin parse, etc, pipleline will be ready to play immediately
	virtual void preroll() = 0;
	virtual void play() = 0;
	virtual void pause() = 0;
	virtual void rewind() = 0;
	virtual void dump();

//	virtual void detachWidgetsFromSinks();	// default is a no-op
//	virtual void attachWidgetsToSinks(HVideoWidget *w0, HVideoWidget *w1=NULL, HVideoWidget *w2=NULL);		// default is a no-op

	// get windowId map
	const PPTWIdMap& widMap() { return m_wid; }

	//static HPipeline* createPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bSound=false, bool bISS=false);

	// (static) utility functions
	static QString makeElementName(const char *factoryName, const HPlayerPositionType& ppt, int number, const char *prefix=NULL);
	static bool parseElementName(const QString& elementName, QString& factoryName, const HPlayerPositionType*& pppt, int& id, QString& prefix);
	static GstElement *makeElement(const char *factoryName, const HPlayerPositionType& ppt, int number, const char *prefix=NULL);
	static GstElement *makeElement(const char *factoryName, const char *fakeName, const HPlayerPositionType& ppt, int number, const char *prefix=NULL);
	static const HPlayerPositionType& getPPTFromElementName(const QString& elementName);
	static void parseCaps(GstCaps* caps, bool& isVideo, bool& isImage, int& width, int& height, bool& isAudio);
	// output
	virtual void write(std::ostream&) const;


	Q_SIGNALS:
		void nowPlaying();
		// called when ASYNC_DONE message received on bus that indicates prerolling is done
		void prerolled(int);

		// called from preroll()
		void prerolling(int);

		// this really means "this stim has started. the int arg is an HPlayerPositionType -
		// call getPlayerPositionType()
		void screen(const QString&, int);

};

std::ostream& operator<<(std::ostream& os, const HPipeline& p);


#endif /* LIBS_HABUTIL_HGST_HPIPELINE_H_ */
