/*
 * HGstMediaManager.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: dan
 */

#include "HGstMediaManager.h"

HGstMediaManager::HGstMediaManager(const HStimulusLayoutType& slType, QGst::Ui::VideoWidget *v, bool bISS)
: HMediaManager()
, m_stimulusLayoutType(slType)
, m_pVideoWidgetCenter(v)
, m_pVideoWidgetLeft(0)
, m_pVideoWidgetRight(0)
{

}

HGstMediaManager::~HGstMediaManager() {
}


void HGstMediaManager::stim(int i)
{
	m_pendingStartSignal = true;
	m_pendingStimNumber = i;
	QMapIterator<HPlayerPositionType, HPlayer *> it(m_players);
	while (it.hasNext())
	{
		it.next().value()->play(i);
	}
}

void HGstMediaManager::clear()
{
	m_pendingClearSignal = true;
	QMapIterator<HPlayerPositionType, HPlayer *> it(m_players);
	while (it.hasNext())
	{
		it.next().value()->clear();
	}
}

unsigned int HGstMediaManager::addAG(const Habit::StimulusSettings& ssAG)
{
	m_mapPStimulusSettings.insert(agKey, &ssAG);
	addStimulusPrivate(agKey, ssAG);

	// Append to context list.
	// This code allows more than one ag....nothing here prevents multiple ags to be added to the list.
	QList<unsigned int> list;
	list.append(agKey);

	// TODO Using -1 as context for attention getters!
	addOrAppendList(-1, list);

	return agKey;
}

unsigned int HGstMediaManager::addStimulusPrivate(unsigned int key, const Habit::StimulusSettings& settings)
{
	// Create pipeline and set to paused.
	// Need to save the pipeline (PipelinePtr), sink (ElementPtr)
	qDebug() << "HGstMediaManager::addStimulusPrivate";
	return key;
}
