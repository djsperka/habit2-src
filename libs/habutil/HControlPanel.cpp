/*
 *  HControlPanel.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HControlPanel.h"
#include "HMediaManagerUtil.h"
#include "HPhase.h"
#include "HKeypadLookDetector.h"
#include "HTrialGenerator.h"
#include "HElapsedTimer.h"
#include "maindao.h"
#include <QtCore/QTDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>


HControlPanel::HControlPanel(HEventLog& log, const Habit::SubjectSettings& ss, const Habit::RunSettings& runSettings, QWidget* w)
: QDialog(w)
, m_log(log)
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
	
	createExperiment(log);

	
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
	Q_UNUSED(e);
#if 0
	qDebug("HControlPanel::closeEvent");
	connect(m_pmm, SIGNAL(cleared()), this, SLOT(accept()));
	m_pmm->clear();
	if (m_pmm)
	{
		delete m_pmm;
		m_pmm = NULL;
	}
	e->accept();
#endif
}


void HControlPanel::createExperiment(HEventLog& log)
{
	// First make sure all info is loaded from db. I'd rather this were done before HControlPanel were instantiated, 
	// but this is how it was originally written so there. 
	loadFromDB();
	
	// These will hold stim numbers for stimuli _available_ to each phase, as configured
	// for the experiment. 
	Habit::StimulusSettingsList l1, l2, l3;

	// These will hold stim numbers for the trials in each phase, as they are configured/randomized
	// on a particular run.
	Habit::StimulusSettingsList lTrials;

	// Get info for creating look detector and phase(s). 
	Habit::DesignSettings ds = m_experimentSettings.getDesignSettings();
	Habit::TrialsInfo tiPreTest = ds.getPretestTrialsInfo();
	Habit::TrialsInfo tiHabituation = ds.getHabituationTrialsInfo();
	Habit::TrialsInfo tiTest = ds.getTestTrialsInfo();
	Habit::AttentionGetterSettings ags = m_experimentSettings.getAttentionGetterSettings();
	
	
	// Create media manager. Note we are not passing a parent! 
	m_pmm = createMediaManager(m_experimentSettings, NULL, l1, l2, l3);
	
	// Save the lists of stimuli for use in updating labels.
	populateSSMap(ags, l1, l2, l3);
	
	// Connect media manager signals to slots here so we can update display labels.
	connect(m_pmm, SIGNAL(agStarted()), this, SLOT(onAGStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(onStimStarted(int)));
	connect(m_pmm, SIGNAL(cleared()), this, SLOT(onCleared()));
	
	// Create look detector
	int lookTimeMS = tiPreTest.getLookTimes() * 100;
	int lookAwayTimeMS = tiHabituation.getLookTimes() * 100;
	int noLookTimeMS = tiTest.getLookTimes() * 100;
	m_pld = new HKeypadLookDetector(lookTimeMS, lookAwayTimeMS, log, this);

	// connect attention() and look() signals to a slot so we can forward the info to the event log
	connect(m_pld, SIGNAL(attention()), this, SLOT(onAttention()));
	connect(m_pld, SIGNAL(look(HLook)), this, SLOT(onLook(HLook)));
	
	// Construct state machine.
	m_psm = new QStateMachine();
	
	// connect the state machine's finished() signal to this dialog's close() slot
	connect(m_psm, SIGNAL(finished()), this, SLOT(onExperimentFinished()));
		
	// This is a single super-state that holds all the phases.
	HExperiment* sExperiment = new HExperiment(log, *m_pmm, *m_pld);
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
		m_pcritPreTest = new HPhaseFixedNCriteria(tiPreTest.getNumberOfTrials());
		m_psPreTest = new HPhase(*sExperiment, m_pcritPreTest, log, lTrials, HPhaseType::PreTest, tiPreTest.getLength() * 100, noLookTimeMS, tiPreTest.getType() == Habit::TrialsInfo::eFixedLength, ags.isAttentionGetterUsed());
	}

	if (tiHabituation.getNumberOfTrials() > 0)
	{
		HTrialGenerator htg(l2.size(), m_runSettings.isHabituationRandomized(), m_runSettings.getHabituationRandomizeMethod()==1);
		lTrials.clear();
		for (unsigned int i=0; i<tiHabituation.getNumberOfTrials(); i++)
		{
			lTrials.append(l2.at(htg.next()));
		}
		
		// Create habituation criteria object. 
		m_pcritHabituation = new HPhaseHabituationCriteria();
		m_psHabituation = new HPhase(*sExperiment, m_pcritHabituation, log, lTrials, HPhaseType::Habituation, tiHabituation.getLength() * 100, noLookTimeMS, tiHabituation.getType() == Habit::TrialsInfo::eFixedLength, ags.isAttentionGetterUsed());
	}
	
	if (tiTest.getNumberOfTrials() > 0)
	{
		HTrialGenerator htg(l3.size(), m_runSettings.isTestRandomized(), m_runSettings.getTestRandomizeMethod()==1);
		lTrials.clear();
		for (unsigned int i=0; i<tiTest.getNumberOfTrials(); i++)
		{
			lTrials.append(l3.at(htg.next()));
		}
		
		m_pcritTest = new HPhaseFixedNCriteria(tiTest.getNumberOfTrials());
		m_psTest = new HPhase(*sExperiment, m_pcritTest, log, lTrials, HPhaseType::Test, tiTest.getLength() * 100, noLookTimeMS, tiTest.getType() == Habit::TrialsInfo::eFixedLength, ags.isAttentionGetterUsed());
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
	
	
	// Finally, text properties of labels can be assigned on entry to various states.
	sExperiment->assignProperty(m_labelExperimentStatusValue, "text", "Running");
	if (m_psPreTest)
		m_psPreTest->assignProperty(m_labelCurrentPhaseValue, "text", "PreTest");
	if (m_psHabituation)
		m_psHabituation->assignProperty(m_labelCurrentPhaseValue, "text", "Habituation");
	if (m_psTest)
		m_psTest->assignProperty(m_labelCurrentPhaseValue, "text", "Test");
	
}

void HControlPanel::populateSSMap(const Habit::AttentionGetterSettings& ags, const Habit::StimulusSettingsList& l1, const Habit::StimulusSettingsList& l2, const Habit::StimulusSettingsList& l3)
{
	if (ags.isAttentionGetterUsed())
	{
		m_mapSS[0] = ags.getAttentionGetterStimulus();
	}
	
	QListIterator< QPair<int, Habit::StimulusSettings> > it1(l1);
	while (it1.hasNext())
	{
		QPair<int, Habit::StimulusSettings> p = it1.next();
		m_mapSS[p.first] = p.second;
	}
	
	QListIterator< QPair<int, Habit::StimulusSettings> > it2(l2);
	while (it2.hasNext())
	{
		QPair<int, Habit::StimulusSettings> p = it2.next();
		m_mapSS[p.first] = p.second;
	}
	QListIterator< QPair<int, Habit::StimulusSettings> > it3(l3);
	while (it3.hasNext())
	{
		QPair<int, Habit::StimulusSettings> p = it3.next();
		m_mapSS[p.first] = p.second;
	}
};

void HControlPanel::updateFileStatusLabels(Habit::StimulusSettings& ss)
{
	
	if (ss.isLeftEnabled())
	{
		m_labelLeftMonitorFileValue->setText(ss.getLeftStimulusInfo().getFileName());
	}
	else 
	{
		m_labelLeftMonitorFileValue->setText("unused");
	}
	
	if (ss.isCenterEnabled())
	{
		m_labelCenterMonitorFileValue->setText(ss.getCenterStimulusInfo().getFileName());
	}
	else 
	{
		m_labelCenterMonitorFileValue->setText("unused");
	}
	
	if (ss.isRightEnabled())
	{
		m_labelRightMonitorFileValue->setText(ss.getRightStimulusInfo().getFileName());
	}
	else 
	{
		m_labelRightMonitorFileValue->setText("unused");
	}
	
	if (ss.isIndependentSoundEnabled())
	{
		m_labelSoundFileValue->setText(ss.getIndependentSoundInfo().getFileName());
	}
	else 
	{
		m_labelSoundFileValue->setText("unused");
	}
	
}

void HControlPanel::onAGStarted()
{
	Habit::StimulusSettings ss = m_mapSS[0];
	m_labelAttentionGetterStatusValue->setText("Running ");
	m_labelCurrentStimulusValue->setText("Idle");
	updateFileStatusLabels(ss);
}

void HControlPanel::onStimStarted(int i)
{
	Habit::StimulusSettings ss = m_mapSS[i];
	QString s("Running (" + ss.getName() + ")");
	m_labelAttentionGetterStatusValue->setText("Idle");
	m_labelCurrentStimulusValue->setText(s);
	updateFileStatusLabels(ss);
}

void HControlPanel::onCleared()
{
	m_labelAttentionGetterStatusValue->setText("Idle");
	m_labelCurrentStimulusValue->setText("Idle");
	m_labelLeftMonitorFileValue->setText("");
	m_labelCenterMonitorFileValue->setText("");
	m_labelRightMonitorFileValue->setText("");
	m_labelSoundFileValue->setText("");
}

void HControlPanel::onAttention()
{
	m_log.append(new HAttentionEvent(HElapsedTimer::elapsed()));
}


void HControlPanel::onLook(HLook l)
{
	m_log.append(new HLookEvent(l, HElapsedTimer::elapsed()));
	switch (l.direction())
	{
		case LookLeft:
			m_labelLookStatusValue->setText("Look Left");
			break;
		case LookCenter:
			m_labelLookStatusValue->setText("Look Center");
			break;
		case LookRight:
			m_labelLookStatusValue->setText("Look Right");
			break;
		case NoLook:
			m_labelLookStatusValue->setText("No Look");
			break;
		case UnknownLookDirection:
			qWarning("HControlPanel::onLook: UnknownLookDirection");
			break;
	}
}
			

void HControlPanel::loadFromDB()
{
	Habit::MainDao dao;
//	qDebug("HControlPanel::loadFromDB(): get exp settings.");
	m_experimentSettings = dao.getExperimentSettingsById(m_runSettings.getExperimentId());
	m_experimentSettings.loadFromDB();
	
#ifdef NO_LONGER_USED
	qDebug() << m_experimentSettings.getPreTestStimuliSettings();
	qDebug() << m_experimentSettings.getHabituationStimuliSettings();
	qDebug() << m_experimentSettings.getTestStimuliSettings();
#endif
	
//	qDebug("HControlPanel::loadFromDB(): get exp settings - Done.");
}
	
	
void HControlPanel::onStartTrials()
{
	
	/*
	 * Set control buttons enabled/disabled as necessary
	 */
	
	qDebug("HControlPanel::onStartTrials()");
	m_pbStopTrials->setEnabled(true);
	m_pbNextTrial->setEnabled(true);
	m_pbStartTrials->setEnabled(false);

	/*
	 * Start the state machine
	 */
	
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
	connect(m_pmm, SIGNAL(cleared()), this, SLOT(onExperimentFinished()));
	m_pmm->clear();
}

void HControlPanel::onExperimentFinished()
{
	if (m_pmm)
	{
		delete m_pmm;
		m_pmm = NULL;
	}
	accept();
}

