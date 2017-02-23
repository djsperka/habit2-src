/*
 * HPreviewMediaManager.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: dan
 */

#include "HPreviewMediaManager.h"
#include <QHBoxLayout>
#include <QStackedWidget>

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


void HPreviewMediaManager::addPlayers(HPlayer *p0, HPlayer *p1l, HPlayer *p1r, HPlayer *psound)
{
	QHBoxLayout *hbox0 = new QHBoxLayout;
	addPlayer(HPlayerPositionType::Center, p0, -1);
	hbox0->addWidget(p0);

	QHBoxLayout *hbox1 = new QHBoxLayout;
	addPlayer(HPlayerPositionType::Left, p1l, -1);
	hbox1->addWidget(p1l);
	addPlayer(HPlayerPositionType::Right, p1r, -1);
	hbox1->addWidget(p1r);

	if (psound)
		addPlayer(HPlayerPositionType::Sound, psound, -1);

	m_pstack = new QStackedWidget;

	QWidget *w0 = new QWidget;
	w0->setLayout(hbox0);
	m_pstack->addWidget(w0);

	QWidget *w1 = new QWidget;
	w1->setLayout(hbox1);
	m_pstack->addWidget(w1);
}

