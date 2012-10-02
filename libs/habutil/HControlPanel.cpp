/*
 *  HControlPanel.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HControlPanel.h"
#include "HOutputGenerator.h"
#include "HMediaManagerUtil.h"
#include "HPhase.h"
#include "HKeypadLookDetector.h"
#include "HTrialGenerator.h"
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
, m_psPreTest(0)
, m_psHabituation(0)
, m_psTest(0)
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
	//connect(m_pbStopTrials, SIGNAL(clicked()), this, SLOT(onStopTrials()));
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


void HControlPanel::closeEvent(QCloseEvent* e)
{
	qDebug("HControlPanel::closeEvent");
	if (m_pmm)
	{
		delete m_pmm;
		m_pmm = NULL;
	}
	e->accept();
}


void HControlPanel::createExperiment()
{
	// First make sure all info is loaded from db. I'd rather this were done before HControlPanel were instantiated, 
	// but this is how it was originally written so there. 
	loadFromDB();
	
	// These will hold stim numbers for stimuli available to each phase.
	Habit::StimulusSettingsList l1, l2, l3;

	// These will hold stim numbers for the trials in each phase.
	Habit::StimulusSettingsList lTrials;
	
	// Create media manager. Note we are not passing a parent! 
	m_pmm = createMediaManager(m_experimentSettings, NULL, l1, l2, l3);
	
	// Get info for creating look detector and phase(s). 
	Habit::DesignSettings ds = m_experimentSettings.getDesignSettings();
	Habit::TrialsInfo tiPreTest = ds.getPretestTrialsInfo();
	Habit::TrialsInfo tiHabituation = ds.getHabituationTrialsInfo();
	Habit::TrialsInfo tiTest = ds.getTestTrialsInfo();
	Habit::AttentionGetterSettings ags = m_experimentSettings.getAttentionGetterSettings();
	
	// Create look detector
	int lookTimeMS = tiPreTest.getLookTimes() * 100;
	int lookAwayTimeMS = tiHabituation.getLookTimes() * 100;
	int noLookTimeMS = tiTest.getLookTimes() * 100;
	m_pld = new HKeypadLookDetector(lookTimeMS, lookAwayTimeMS, this);
	
	
	// Construct state machine.
	m_psm = new QStateMachine();
	
	// connect the state machine's finished() signal to this dialog's close() slot
	connect(m_psm, SIGNAL(finished()), this, SLOT(close()));
		
	// This is a single super-state that holds all the phases.
	HExperimentState* sExperiment = new HExperimentState();
	m_psm->addState(sExperiment);
	m_psm->setInitialState(sExperiment);
	QFinalState* sFinal = new QFinalState;
	m_psm->addState(sFinal);
	sExperiment->addTransition(sExperiment, SIGNAL(finished()), sFinal);
 	QObject::connect(sExperiment, SIGNAL(playStim(int)), m_pmm, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()

	// transition from experiment on the "Stop Trials" button being clicked()
	sExperiment->addTransition(m_pbStopTrials, SIGNAL(clicked()), sFinal);
	
	// Create phases	
	if (tiPreTest.getNumberOfTrials() > 0)
	{
		HTrialGenerator htg(l1.size(), m_runSettings.isPretestRandomized(), m_runSettings.getPretestRandomizeMethod()==1);
		lTrials.clear();
		for (unsigned int i=0; i<tiPreTest.getNumberOfTrials(); i++)
		{
			lTrials.append(l1.at(htg.next()));
		}

		m_psPreTest = new HPhase(lTrials, m_pmm, m_pld, tiPreTest.getLength() * 100, noLookTimeMS, tiPreTest.getType() == Habit::TrialsInfo::eFixedLength, ags.isAttentionGetterUsed(), sExperiment);
	}

	if (tiHabituation.getNumberOfTrials() > 0)
	{
		HTrialGenerator htg(l2.size(), m_runSettings.isHabituationRandomized(), m_runSettings.getHabituationRandomizeMethod()==1);
		lTrials.clear();
		for (unsigned int i=0; i<tiHabituation.getNumberOfTrials(); i++)
		{
			lTrials.append(l2.at(htg.next()));
		}
		
		m_psHabituation = new HPhase(lTrials, m_pmm, m_pld, tiHabituation.getLength() * 100, noLookTimeMS, tiHabituation.getType() == Habit::TrialsInfo::eFixedLength, ags.isAttentionGetterUsed(), sExperiment);
	}
	
	if (tiTest.getNumberOfTrials() > 0)
	{
		HTrialGenerator htg(l3.size(), m_runSettings.isTestRandomized(), m_runSettings.getTestRandomizeMethod()==1);
		lTrials.clear();
		for (unsigned int i=0; i<tiTest.getNumberOfTrials(); i++)
		{
			lTrials.append(l3.at(htg.next()));
		}
		
		m_psTest = new HPhase(lTrials, m_pmm, m_pld, tiTest.getLength() * 100, noLookTimeMS, tiTest.getType() == Habit::TrialsInfo::eFixedLength, ags.isAttentionGetterUsed(), sExperiment);
	}

	
	// Now assemble the experiment.
	QState* plast = NULL;
	if (m_psPreTest)
	{
		if (plast)
		{
			plast->addTransition(plast, SIGNAL(finished()), m_psPreTest);
			plast = m_psPreTest;
		}
		else 
		{
			sExperiment->setInitialState(m_psPreTest);
			plast = m_psPreTest;
		}
	}
	if (m_psHabituation)
	{
		if (plast)
		{
			plast->addTransition(plast, SIGNAL(finished()), m_psHabituation);
			plast = m_psHabituation;
		}
		else 
		{
			sExperiment->setInitialState(m_psHabituation);
			plast = m_psHabituation;
		}
	}
	if (m_psTest)
	{
		if (plast)
		{
			plast->addTransition(plast, SIGNAL(finished()), m_psTest);
			plast = m_psTest;
		}
		else 
		{
			sExperiment->setInitialState(m_psTest);
			plast = m_psTest;
		}
	}

	QFinalState* sExperimentFinal = new QFinalState(sExperiment);
	if (plast)
	{
		plast->addTransition(plast, SIGNAL(finished()), sExperimentFinal);
	}
	else 
	{
		// This is a trivial case
		sExperiment->setInitialState(sExperimentFinal);
	}
	
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
	
	HOutputGenerator::instance()->setExperiment(m_runSettings.getExperimentId());
	HOutputGenerator::instance()->setRunSettings(m_runSettings);
	HOutputGenerator::instance()->setSubjectInformation(m_subjectSettings);

	m_psm->start();
}

void HControlPanel::onNextTrial()
{
	qDebug("HControlPanel::onNextTrial()");
	m_psm->postEvent(new HAbortTrialEvent());
}

void HControlPanel::onStopTrials()
{
	qDebug("HControlPanel::onStopTrials()");
	m_psm->stop();
}
