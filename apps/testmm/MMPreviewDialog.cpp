/*
 * MMPreviewDialog.cpp
 *
 *  Created on: Jun 1, 2015
 *      Author: Oakeslab
 */


#include "MMPreviewDialog.h"
#include "HVIPlayer.h"
#include "HAudioPlayer.h"

#include <QtGui>

using namespace Habit;
using namespace GUILib;

MMPreviewDialog::MMPreviewDialog(const ExperimentSettings& settings, const QDir& dirStimRoot, QWidget *parent)
: QDialog(parent)
, m_settings(settings)
{
	m_pmm = new HMediaManager(false);
	connect(m_pmm, SIGNAL(agStarted(int)), this, SLOT(onAGStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(onStimStarted(int)));
	connect(m_pmm, SIGNAL(cleared()), this, SLOT(onCleared()));

	// HBoxLayout to hold the players, even if there's just 1
	QHBoxLayout *hbox = new QHBoxLayout;

	StimulusDisplayInfo info(m_settings.getStimulusDisplayInfo());
	if (info.getUseISS())
	{
		HPlayer *p = new HAudioPlayer(-1, NULL, dirStimRoot);
		m_pmm->addPlayer(HPlayerPositionType::Sound, p, -1);
		//hbox->addWidget(p);
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

	// Populate the media manager AFTER players have been added to the MM!
	populateMM();
	m_pmodel = new HPStimulusSettingsListModel(m_pmm->pmap());

	m_pListView = new QListView(this);
	m_pListView->setModel(m_pmodel);
	QHBoxLayout *stimLayout = new QHBoxLayout;
	stimLayout->addWidget(m_pListView);

	connect(m_pListView, SIGNAL(activated(const QModelIndex&)), this, SLOT(itemActivated(const QModelIndex&)));

	QHBoxLayout *buttons = new QHBoxLayout;
	QPushButton* playButton = new QPushButton(tr("Play"));
	QPushButton* clearButton = new QPushButton(tr("Clear"));
	buttons->addStretch(1);
	buttons->addWidget(playButton);
	buttons->addWidget(clearButton);

    connect(playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearClicked()));

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addLayout(stimLayout);
    vbox->addLayout(hbox);
    vbox->addLayout(buttons);
    setLayout(vbox);
}

void MMPreviewDialog::populateMM()
{
	// Need to know if AG is used. If it is, add attention getter settings to media manager
	// YOW!
	//	Habit::AttentionGetterSettings ags(m_settings.getAttentionGetterSettings());
	//	if (ags.isAttentionGetterUsed()) m_pmm->addAG(ags.getAttentionGetterStimulus());

	if (m_settings.getAttentionGetterSettings().isAttentionGetterUsed())
	{
		m_pmm->addAG(m_settings.getAttentionGetterSettings().getAttentionGetterStimulus());
	}

	// add stimuli for each phase.
	if (m_settings.getPreTestPhaseSettings().getIsEnabled())
		m_pmm->addStimuli(m_settings.getPreTestStimuliSettings());
	if (m_settings.getHabituationPhaseSettings().getIsEnabled())
		m_pmm->addStimuli(m_settings.getHabituationStimuliSettings());
	if (m_settings.getTestPhaseSettings().getIsEnabled())
		m_pmm->addStimuli(m_settings.getTestStimuliSettings());
}



void MMPreviewDialog::playClicked()
{
	// what row is selected?
	QModelIndexList selectedIndexes = m_pListView->selectionModel()->selectedIndexes();
	if (selectedIndexes.size() == 1)
	{
		m_pmm->stim(m_pmodel->getStimulusKey(selectedIndexes.at(0).row()));
	}
}

void MMPreviewDialog::itemActivated(const QModelIndex& index)
{
	m_pmm->stim(m_pmodel->getStimulusKey(index.row()));
}


void MMPreviewDialog::clearClicked()
{
	m_pmm->clear();
}

void MMPreviewDialog::onAGStarted()
{
	qDebug() << "AG Started";
}

void MMPreviewDialog::onStimStarted(int i)
{
	qDebug() << "Stim started " << i;
}

void MMPreviewDialog::onCleared()
{
	qDebug() << "Stim cleared";
}
