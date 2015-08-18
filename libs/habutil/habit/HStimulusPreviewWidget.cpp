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

	// set up label for stim name and buttons for advancing stim through orders.
	m_pbNext = new QPushButton(">");
	m_pbPrev = new QPushButton("<");
	m_labelStimName = new QLabel("Stimulus: ");
	QHBoxLayout *hlb = new QHBoxLayout;
	hlb->addWidget(m_labelStimName, 1, Qt::AlignCenter);
	hlb->addStretch(1);
	hlb->addWidget(m_pbPrev);
	hlb->addWidget(m_pbNext);

	connect(m_pbPrev, SIGNAL(clicked()), this, SLOT(prevClicked()));
	connect(m_pbNext, SIGNAL(clicked()), this, SLOT(nextClicked()));

	// Now do the layout for the entire thing
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addLayout(hbox);
    vbox->addLayout(hlb);
    setLayout(vbox);

}

GUILib::HStimulusPreviewWidget::~HStimulusPreviewWidget()
{
	m_pmm->clear();
	delete m_pmm;
}

void GUILib::HStimulusPreviewWidget::prevClicked()
{
	m_idListCurrent--;
	updateNavigation();
	m_pmm->stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::nextClicked()
{
	m_idListCurrent++;
	updateNavigation();
	m_pmm->stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::preview(const Habit::StimulusSettings& stimulus)
{
	unsigned int stimid;
	m_pmm->clear();
	stimid = m_pmm->addStimulus(stimulus);
	updateNavigation(stimulus.getName());
	m_pmm->stim(stimid);
}

void GUILib::HStimulusPreviewWidget::updateNavigation(QString stimName)
{
	m_pbPrev->setEnabled(false);
	m_pbNext->setEnabled(false);
	m_labelStimName->setText(QString("Stimulus: %1").arg(stimName));
}

void GUILib::HStimulusPreviewWidget::updateNavigation()
{
	if (m_idListCurrent > 0)
		m_pbPrev->setEnabled(true);
	else
		m_pbPrev->setEnabled(false);
	if (m_idListCurrent < (m_idList.count()-1))
		m_pbNext->setEnabled(true);
	else
		m_pbNext->setEnabled(false);

	QString stimName = m_pmm->getStimulusSettings(m_idList[m_idListCurrent]).getName();
	m_labelStimName->setText(QString("Stimulus (%1/%2): %3").arg(m_idListCurrent+1).arg(m_idList.count()).arg(stimName));
}


void GUILib::HStimulusPreviewWidget::preview(const Habit::HStimulusSettingsList& stimuli, QList< QPair<int, QString> > list)
{
	unsigned int stimid;
	QPair<int, QString> p;
	m_pmm->clear();
	m_idList.clear();
	foreach(p, list)
	{
		stimid = m_pmm->addStimulus(stimuli.at(p.first));
		m_idList.append(stimid);
		//qDebug() << "add stim index " << p.first << " label " << p.second << " stimid " << stimid;
	}
	m_idListCurrent = 0;
	updateNavigation();
	m_pmm->stim(m_idList[m_idListCurrent]);
}

void GUILib::HStimulusPreviewWidget::clear()
{
	m_pmm->clear();
}



