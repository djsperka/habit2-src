/*
 * HStimulusPreviewWidget.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: Oakeslab
 */


#include "HStimulusPreviewWidget.h"
#include "HVIPlayer.h"
#include "HAudioPlayer.h"

GUILib::HStimulusPreviewWidget::HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, const QDir& dirStimRoot, QWidget *parent)
: QWidget(parent)
{
	// HBoxLayout to hold the players, even if there's just 1
	QHBoxLayout *hbox = new QHBoxLayout;

	m_pmm = new HMediaManager(false);
	if (info.getUseISS())
	{
		HPlayer *p = new HAudioPlayer(-1, NULL, dirStimRoot);
		m_pmm->addPlayer(HPlayerPositionType::Sound, p, -1);
	}

	if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HPlayer *p = new HVIPlayer(-1, NULL, dirStimRoot, info.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, info.getBackGroundColor());
		m_pmm->addPlayer(HPlayerPositionType::Center, p, -1);
		hbox->addWidget(p);
	}
	else if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HPlayer *pl = new HVIPlayer(-1, NULL, dirStimRoot, info.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, info.getBackGroundColor());
		m_pmm->addPlayer(HPlayerPositionType::Left, pl, -1);
		hbox->addWidget(pl);
		HPlayer *pr = new HVIPlayer(-1, NULL, dirStimRoot, info.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, info.getBackGroundColor());
		m_pmm->addPlayer(HPlayerPositionType::Right, pr, -1);
		hbox->addWidget(pr);
	}

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addLayout(hbox);
    setLayout(vbox);

}

void GUILib::HStimulusPreviewWidget::preview(const Habit::StimulusSettings& stimulus)
{
	unsigned int stimid = m_pmm->addStimulus(stimulus);
	m_pmm->stim(stimid);
}

void GUILib::HStimulusPreviewWidget::clear()
{
	m_pmm->clear();
}



