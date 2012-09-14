/*
 *  HControlPanel.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HControlPanel.h"
#include "maindao.h"
#include <QtCore/QTDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>


HControlPanel::HControlPanel(const Habit::SubjectSettings& ss, const Habit::RunSettings& runSettings, QWidget* w)
: QDialog(w)
, m_runSettings(runSettings)
, m_subjectSettings(ss)
{
	
	// generate gui elements and make it look pretty

	createComponents();
	makeConnections();
	doLayout();
	m_pbStopTrials->setEnabled(false);
	m_pbNextTrial->setEnabled(false);
	
	// Generate the state machine and associated machinery that make the experiment run 
	
	createExperiment();

}

void HControlPanel::createComponents()
{
	m_labelExperimentStatus = new QLabel(tr("Experiment Status:"));
	m_labelLookStatus = new QLabel(tr("Look Status:"));
	m_labelAttentionGetterStatus = new QLabel(tr("A.G. Status"));
	m_labelCurrentPhase = new QLabel(tr("Current Phase:"));
	m_labelCurrentStimulus = new QLabel(tr("Current Stimulus:"));
	m_labelLeftMonitorFile = new QLabel(tr("Left Monitor File:"));
	m_labelCenterMonitorFile = new QLabel(tr("Center Monitor File:"));
	m_labelRightMonitorFile = new QLabel(tr("Right Monitor File:"));
	m_labelSoundFile = new QLabel(tr("Sound File:"));
	
	m_labelCurrentPhaseValue = new QLabel(); m_labelCurrentPhaseValue->setMaximumWidth(250);
	m_labelCurrentStimulusValue = new QLabel(); m_labelCurrentStimulusValue->setMaximumWidth(250);
	m_labelLeftMonitorFileValue = new QLabel(); m_labelLeftMonitorFileValue->setMaximumWidth(250);
	m_labelCenterMonitorFileValue = new QLabel(); m_labelCenterMonitorFileValue->setMaximumWidth(250);
	m_labelRightMonitorFileValue = new QLabel(); m_labelRightMonitorFileValue->setMaximumWidth(250);
	m_labelSoundFileValue = new QLabel(); m_labelSoundFileValue->setMaximumWidth(250);
	m_labelExperimentStatusValue = new QLabel(); m_labelExperimentStatusValue->setMaximumWidth(250);
	m_labelLookStatusValue = new QLabel(); m_labelLookStatusValue->setMaximumWidth(250);
	m_labelAttentionGetterStatusValue = new QLabel(); m_labelAttentionGetterStatusValue->setMaximumWidth(250);
	
	m_pbStartTrials = new QPushButton(tr("Start Trials"));
	m_pbNextTrial = new QPushButton(tr("Next Trial"));
	m_pbStopTrials = new QPushButton(tr("Stop Trials"));
}

void HControlPanel::makeConnections() 
{
	connect(m_pbStartTrials, SIGNAL(clicked()), this, SLOT(onStartTrials()));
	connect(m_pbNextTrial, SIGNAL(clicked()), this, SLOT(onNextTrial()));
	connect(m_pbStopTrials, SIGNAL(clicked()), this, SLOT(onStopTrials()));
}

void HControlPanel::doLayout()
{
	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(m_pbStartTrials);
	buttonsLayout->addWidget(m_pbNextTrial);
	buttonsLayout->addWidget(m_pbStopTrials);
	
	QGridLayout* gl = new QGridLayout();
	gl->addWidget(m_labelExperimentStatus, 0, 0);
	gl->addWidget(m_labelLookStatus, 1, 0);
	gl->addWidget(m_labelAttentionGetterStatus, 2, 0);
	gl->addWidget(m_labelCurrentPhase, 3, 0);
	gl->addWidget(m_labelCurrentStimulus, 4, 0);
	gl->addWidget(m_labelLeftMonitorFile, 5, 0);
	gl->addWidget(m_labelCenterMonitorFile, 6, 0);
	gl->addWidget(m_labelRightMonitorFile, 7, 0);
	gl->addWidget(m_labelSoundFile, 8, 0);
	
	gl->addWidget(m_labelExperimentStatusValue, 0, 1);
	gl->addWidget(m_labelLookStatusValue, 1, 1);
	gl->addWidget(m_labelAttentionGetterStatusValue, 2, 1);
	gl->addWidget(m_labelCurrentPhaseValue, 3, 1);
	gl->addWidget(m_labelCurrentStimulusValue, 4, 1);
	gl->addWidget(m_labelLeftMonitorFileValue, 5, 1);
	gl->addWidget(m_labelCenterMonitorFileValue, 6, 1);
	gl->addWidget(m_labelRightMonitorFileValue, 7, 1);
	gl->addWidget(m_labelSoundFileValue, 8, 1);
	
	QHBoxLayout* hb = new QHBoxLayout();
	hb->addLayout(gl);
	hb->addStretch();
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addLayout(buttonsLayout);
	mainLayout->addLayout(hb);
	mainLayout->addStretch();
}

void HControlPanel::createExperiment()
{
	// First make sure all info is loaded from db. I'd rather this were done before HControlPanel were instantiated, 
	// but this is how it was originally written so there. 
	loadFromDB();
	
	// Now create media manager
	m_pmm = createMediaManager();

	
	
}

void HControlPanel::loadFromDB()
{
	Habit::MainDao dao;
	m_experimentSettings = dao.getExperimentSettingsById(m_runSettings.getExperimentId());
	m_experimentSettings.loadFromDB();
}
	
	
void HControlPanel::onStartTrials()
{
	qDebug("HControlPanel::onStartTrials()");
	m_pbStopTrials->setEnabled(true);
	m_pbNextTrial->setEnabled(true);
	m_pbStartTrials->setEnabled(false);
}

void HControlPanel::onNextTrial()
{
	qDebug("HControlPanel::onNextTrial()");
}

void HControlPanel::onStopTrials()
{
	qDebug("HControlPanel::onStopTrials()");
}


HabitMediaManager* HControlPanel::createMediaManager()
{
	// Create the media manager. its an empty shell at this point - we will add players for the screens later.....
	m_pmm = new HabitMediaManager();
	
	// Will a player for 
	
	return m_pmm;
}


#if 0
void TestPhaseDialog::goClicked()
{
	bool bValue;
	qDebug() << "goClicked()" << lineeditLook->text().toInt(&bValue) <<  " " << lineeditLookAway->text().toInt(&bValue) << endl;
	checkboxAG->setDisabled(true);
	checkboxFixed->setDisabled(true);
	pushbuttonGo->setDisabled(true);
	lineeditLook->setDisabled(true);
	lineeditLookAway->setDisabled(true);
	lineeditMaxMS->setDisabled(true);
	m_bGoClicked = true;
	
	// create looker object and connect it to our own slot. 
	// note that this connection isn't needed for the trial class to function - we do it to display
	// looks signals only. 
	
	//ld = new HKeypadLookDetector(lineeditLook->text().toInt(&bValue), lineeditLookAway->text().toInt(&bValue), listWidget, true, false, true);
	ld = new HKeypadLookDetector(lineeditLook->text().toInt(&bValue), lineeditLookAway->text().toInt(&bValue), this, true, true, true);
	connect(ld, SIGNAL(look(HLook)), this, SLOT(gotLook(HLook)));
	
	// create media manager
	HabitMediaManager* pmm = createMediaManager();
	
	// Now create a trial, phase and state machine
	HStateMachine* machine = new HStateMachine();
	
	QList<int> stimuli;
	stimuli.append(1);
	stimuli.append(2);
	stimuli.append(3);
	stimuli.append(4);
	
	phase = new HPhase(stimuli, pmm, ld, lineeditMaxMS->text().toInt(&bValue), checkboxFixed->isChecked(), checkboxAG->isChecked());
	machine->addState(phase);
	QFinalState* sFinal = new QFinalState;
	machine->addState(sFinal);
	phase->addTransition(phase, SIGNAL(finished()), sFinal);
	machine->setInitialState(phase);
	machine->start();
	
	return;
};

HabitMediaManager* TestPhaseDialog::createMediaManager()
{
	// create media manager
	HabitMediaManager* pmm = new HabitMediaManager();
	
#if USE_FAKE_PLAYER
	pmm->addPlayer(new HabitNonPlayer(1, this));
#else
	HabitVideoImagePlayer* player1 = new HabitVideoImagePlayer(1, this);
	QRect rect = QApplication::desktop()->screenGeometry(1);
	player1->setGeometry(rect);
	player1->move(rect.x(), rect.y());
	
	//player1->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov", true);
	player1->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov", true);
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/mainecoon.jpg");
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov");
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg");
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/multicolor_box_bubble.mov");
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg");
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg");
	
	pmm->addPlayer(player1);
#if 0
	HabitVideoImagePlayer* player2 = new HabitVideoImagePlayer(2, this);
	rect = QApplication::desktop()->screenGeometry(2);
	player2->setGeometry(rect);
	player2->move(rect.x(), rect.y());
	player2->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov", true);
	//player2->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov");
	player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg");
	player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/multicolor_box_bubble.mov", true);
	player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov", true);
	player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/mainecoon.jpg");
	player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg");
	
	pmm->addPlayer(player2);
#endif
#endif
	
	return pmm;
}


#endif