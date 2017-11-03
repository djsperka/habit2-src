/*
 * HPipelineColor.h
 *
 *  Created on: Nov 1, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HPIPELINECOLOR_H_
#define LIBS_HABUTIL_HGST_HPIPELINECOLOR_H_

#include <hgst/HPipeline.h>

class HPipelineColor: public HPipeline
{
	bool m_bSingle;

public:
	HPipelineColor(int id, const QColor& color, const QString& soundFile = QString(), QObject *parent=NULL);
	virtual ~HPipelineColor();

	// allocate resources, do not open files or start streaming
	virtual void ready();

	// open files (if needed), begin parse, etc, pipleline will be ready to play immediately
	virtual void preroll();

	virtual void play();
	virtual void pause();

	virtual void detachWidgetsFromSinks();
	virtual void attachWidgetsToSinks(QWidget *w0, QWidget *w1=NULL);

};

#endif /* LIBS_HABUTIL_HGST_HPIPELINECOLOR_H_ */
