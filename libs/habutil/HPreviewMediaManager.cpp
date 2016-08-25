/*
 * HPreviewMediaManager.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: dan
 */

#include "HPreviewMediaManager.h"

HPreviewMediaManager::HPreviewMediaManager(const HStimulusLayoutType& layoutType)
: HMediaManager(false)
, m_pLayoutType(&layoutType)
{};


void HPreviewMediaManager::stim(int i)
{
	m_pendingStartSignal = true;
	m_pendingStimNumber = i;
	if (*m_pLayoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		if (m_players.contains(HPlayerPositionType::Center))
			m_players[HPlayerPositionType::Center]->play(i);
		if (m_players.contains(HPlayerPositionType::Sound))
			m_players[HPlayerPositionType::Sound]->play(i);
	}
	else if (*m_pLayoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		if (m_players.contains(HPlayerPositionType::Left))
			m_players[HPlayerPositionType::Left]->play(i);
		if (m_players.contains(HPlayerPositionType::Right))
			m_players[HPlayerPositionType::Right]->play(i);
		if (m_players.contains(HPlayerPositionType::Sound))
			m_players[HPlayerPositionType::Sound]->play(i);
	}
}


