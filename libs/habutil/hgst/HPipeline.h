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

#define C_STR(x) x.toStdString().c_str()

class HPipeline: public QObject
{
	Q_OBJECT

	int m_id;

	static HPipeline *createPipelineSingle(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, bool bSound, bool bISS);
	static HPipeline *createPipelineLeftRight(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, bool bSound, bool bISS);
public:
	HPipeline(int id, QObject *parent=NULL);

	int id() const { return m_id; };

	// pipeline control

	// allocate resources, do not open files or start streaming
	virtual void ready() = 0;

	// open files (if needed), begin parse, etc, pipleline will be ready to play immediately
	virtual void preroll() = 0;

	virtual void play() = 0;
	virtual void pause() = 0;

	virtual void detachWidgetsFromSinks();	// default is a no-op
	virtual void attachWidgetsToSinks(QWidget *w0, QWidget *w1=NULL);		// default is a no-op

	static HPipeline* createPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& layoutType, bool bSound=false, bool bISS=false);

};



#endif /* LIBS_HABUTIL_HGST_HPIPELINE_H_ */
