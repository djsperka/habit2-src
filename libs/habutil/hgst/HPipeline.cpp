/*
 * HPipeline.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: dan
 */

#include "HPipeline.h"
#include "HPipelineColor.h"
#include "HGMMException.h"

HPipeline::HPipeline(int id, QObject *parent)
: QObject(parent)
, m_id(id)
{
}


void HPipeline::detachWidgetsFromSinks()
{
	//no-op
}

void HPipeline::attachWidgetsToSinks(QWidget *w0, QWidget *w1)
{
	// no-op
}


HPipeline *HPipeline::createPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, const HStimulusLayoutType& stimulusLayoutType, bool bSound, bool bISS)
{
	if (stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		return createPipelineSingle(id, stimulusSettings, stimRoot, bSound, bISS);
	}
	else if (stimulusLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		return NULL;
		createPipelineLeftRight(id, stimulusSettings, stimRoot, bSound, bISS);
	}
	else
	{
		throw HGMMException(std::string("Unknown stimulus layout type: ") + C_STR(stimulusLayoutType.name()));
	}
}


HPipeline *HPipeline::createPipelineSingle (int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, bool bSound, bool bISS)
{
	HPipeline *pipeline = NULL;

	// is center stimulus a solid color or a file?
//	if (stimulusSettings.getCenterStimulusInfo().isColor() || stimulusSettings.getCenterStimulusInfo().isBackground())
//	{
//		return new HPipelineColor(id, stimulusSettings.getCenterStimulusInfo().getColor(), bISS);
//	}
	return pipeline;
}

HPipeline *HPipeline::createPipelineLeftRight(int id, const Habit::StimulusSettings& stimulusSettings, const QDir& stimRoot, bool bSound, bool bISS)
{
	HPipeline *pipeline = NULL;
	return pipeline;
}
