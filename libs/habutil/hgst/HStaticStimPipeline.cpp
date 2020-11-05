/*
 * HAGStimPipeline.cpp
 *
 *  Created on: Dec 26, 2017
 *      Author: dan
 */

#include <hgst/HStaticStimPipeline.h>

HStaticStimPipeline::HStaticStimPipeline(int id, const Habit::StimulusSettings& stimulusSettings, const Habit::StimulusDisplayInfo& info, const PPTWIdMap& widMap, const QDir& stimRoot, QObject *parent)
: HStimPipeline(id, stimulusSettings, info, widMap, stimRoot, parent)
{
}

HStaticStimPipeline::~HStaticStimPipeline()
{
}

void HStaticStimPipeline::cleanup()
{
	// do nothing. call forceCleanup() at end of prog. Otherwise this pipeline always stays initialized & ready
	qDebug() << "HStaticStimPipeline::cannot cleanup id " << id() << " this pipeline is static. Call forceCleanup().";
}

void HStaticStimPipeline::forceCleanup()
{
	qDebug() << "HStaticStimPipeline::forceCleanup id " << id();
	HStimPipeline::cleanup();
}
