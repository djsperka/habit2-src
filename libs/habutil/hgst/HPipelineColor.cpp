/*
 * HPipelineColor.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: dan
 */

#include <hgst/HPipelineColor.h>

HPipelineColor::HPipelineColor(int id, const QColor& color, const QString& soundFile, QObject *parent)
: HPipeline(id, parent)
, m_bSingle(true)
{

}

HPipelineColor::~HPipelineColor()
{
	// TODO Auto-generated destructor stub
}

// allocate resources, do not open files or start streaming
void HPipelineColor::ready()
{

}

// open files (if needed), begin parse, etc, pipleline will be ready to play immediately
void HPipelineColor::preroll()
{

}

void HPipelineColor::play()
{

}

void HPipelineColor::pause()
{

}

void detachWidgetsFromSinks()
{

}

void attachWidgetsToSinks(QWidget *w0, QWidget *w1)
{

}
