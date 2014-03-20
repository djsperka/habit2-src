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
#include "HPhaseCriteriaUtil.h"
//#include "HKeypadLookDetector.h"
#include "HLookDetectorUtil.h"
#include "HTrialGenerator.h"
#include "HElapsedTimer.h"
#include "maindao.h"
#include <QtCore/QTDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>


HControlPanel::HControlPanel(HEventLog& log, const Habit::RunSettings& runSettings, QWidget* w)
: QDialog(w)
, m_log(log)
, m_runSettings(runSettings)
#ifdef MADE_LOCAL
, m_psPreTest(0)
, m_psHabituation(0)
, m_psTest(0)
#endif
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
}


void HControlPanel::createExperiment(HEventLog& log)
{
	HPhase* psPreTest = (HPhase *)NULL;
	HPhase* psHabituation = (HPhase *)NULL;
	HPhase* psTest = (HPhase *)NULL;
	HPhaseCriteria* pcritPreTest = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritHabituation = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritTest = (HPhaseCriteria *)NULL;


	// First make sure all info is loaded from db. I'd rather this were done before HControlPanel were instantiated, 
	// but this is how it was originally written so there. 
	loadFromDB();
	
	// For each phase, save the StimulusSettings and the index at which they are added to the players.
	// The index will be used by the phases to request stim be played.
	Habit::IdStimulusSettingsPairList idspList1, idspList2, idspList3;

	// These will hold stim numbers for all stimuli
	Habit::IdStimulusSettingsPairList idStimPairList;

	// Need to know if AG is used
	Habit::AttentionGetterSettings ags = m_experimentSettings.getAttentionGetterSettings();


	
	// Create media manager. Note we are not passing a parent! 
	// The stimuli configured for each phase are pulled using the experiment settings. 
	// One by one the stimuli are added to the media manager's player. As each stim is added, 
	// its ordinal position in the player's set of stim is paired with the StimulusSettings 
	// object, and the pair is stored in the StimulusSettingsList objects l1, l2, l3.
	m_pmm = createMediaManager(m_experimentSettings, NULL, idspList1, idspList2, idspList3);
	
	// Save the lists of stimuli for use in updating labels.
	populateSSMap(ags, idspList1, idspList2, idspList3);
	
	// Connect media manager signals to slots here so we can update display labels.
	connect(m_pmm, SIGNAL(agStarted(int)), this, SLOT(onAGStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(onStimStarted(int)));
	connect(m_pmm, SIGNAL(cleared()), this, SLOT(onCleared()));
	
	// Create look detector
	m_pld = createLookDetector(m_experimentSettings, log, this);

	// connect attention() and look() signals to a slot so we can forward the info to the event log
	connect(m_pld, SIGNAL(attention()), this, SLOT(onAttention()));
	connect(m_pld, SIGNAL(look(HLook)), this, SLOT(onLook(HLook)));
	
	// Construct state machine.
	m_psm = new QStateMachine();
	
	// connect the state machine's finished() signal to this dialog's close() slot
	connect(m_psm, SIGNAL(finished()), this, SLOT(onExperimentFinished()));
	connect(m_psm, SIGNAL(started()), this, SLOT(onExperimentStarted()));

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
	
	// Create phases.
	// Each HPhase gets a list of stimuli, as pairs of <int, StimulusSettings>, and these are used in order as the
	// stimuli for that phase. The <int> part is passed to the media manager as the key to play that stimulus.
	//
	if (m_experimentSettings.getPreTestPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = m_experimentSettings.getPreTestPhaseSettings();
		QList<int> list;
		if (!m_runSettings.getPretestOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse Pretest order (" << m_runSettings.getPretestOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), m_runSettings.isPretestRandomized(), m_runSettings.getPretestRandomizeMethod()==1);
		idStimPairList.clear();
		for (unsigned int i=0; i<ps.getNTrials(); i++)
		{
			idStimPairList.append(idspList1.at(list.at(htg.next()) - 1));
		}
		pcritPreTest = new HPhaseFixedNCriteria(ps.getNTrials());
		psPreTest = new HPhase(*sExperiment, pcritPreTest, log, idStimPairList, ps, m_experimentSettings.getHLookSettings(), ags.isAttentionGetterUsed());

	}

	if (m_experimentSettings.getHabituationPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = m_experimentSettings.getHabituationPhaseSettings();
		QList<int> list;
		if (!m_runSettings.getHabituationOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse habituation order (" << m_runSettings.getHabituationOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), m_runSettings.isHabituationRandomized(), m_runSettings.getHabituationRandomizeMethod()==1);
		idStimPairList.clear();
		for (unsigned int i=0; i<ps.getNTrials(); i++)
		{
			idStimPairList.append(idspList2.at(list.at(htg.next()) - 1));
		}
		
		// Create habituation criteria object. 
		pcritHabituation = createPhaseCriteria(m_experimentSettings.getHabituationSettings(), ps.getNTrials());
		psHabituation = new HPhase(*sExperiment, pcritHabituation, log, idStimPairList, ps, m_experimentSettings.getHLookSettings(), ags.isAttentionGetterUsed());
	}
	
	if (m_experimentSettings.getTestPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = m_experimentSettings.getTestPhaseSettings();
		QList<int> list;
		if (!m_runSettings.getTestOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse test order (" << m_runSettings.getTestOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), m_runSettings.isTestRandomized(), m_runSettings.getTestRandomizeMethod()==1);
		idStimPairList.clear();
		for (unsigned int i=0; i<ps.getNTrials(); i++)
		{
			idStimPairList.append(idspList3.at(list.at(htg.next()) - 1));
		}
		
		pcritTest = new HPhaseFixedNCriteria(ps.getNTrials());
		psTest = new HPhase(*sExperiment, pcritTest, log, idStimPairList, ps, m_experimentSettings.getHLookSettings(), ags.isAttentionGetterUsed());
	}

	
	// Now assemble the experiment.
	QState* plast = NULL;
	if (psPreTest)
	{
		if (plast)
		{
			plast->addTransition(plast, SIGNAL(finished()), psPreTest);
			plast = psPreTest;
		}
		else 
		{
			sExperiment->setInitialState(psPreTest);
			plast = psPreTest;
		}
	}
	if (psHabituation)
	{
		if (plast)
		{
			plast->addTransition(plast, SIGNAL(finished()), psHabituation);
			plast = psHabituation;
		}
		else 
		{
			sExperiment->setInitialState(psHabituation);
			plast = psHabituation;
		}
	}
	if (psTest)
	{
		if (plast)
		{
			plast->addTransition(plast, SIGNAL(finished()), psTest);
			plast = psTest;
		}
		else 
		{
			sExperiment->setInitialState(psTest);
			plast = psTest;
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
	if (psPreTest)
		psPreTest->assignProperty(m_labelCurrentPhaseValue, "text", "PreTest");
	if (psHabituation)
		psHabituation->assignProperty(m_labelCurrentPhaseValue, "text", "Habituation");
	if (psTest)
		psTest->assignProperty(m_labelCurrentPhaseValue, "text", "Test");
	
}

void HControlPanel::populateSSMap(const Habit::AttentionGetterSettings& ags, const Habit::IdStimulusSettingsPairList& idsp1, const Habit::IdStimulusSettingsPairList& idsp2, const Habit::IdStimulusSettingsPairList& idsp3)
{
	m_mapSS.clear();
	if (ags.isAttentionGetterUsed())
	{
		m_mapSS[0] = ags.getAttentionGetterStimulus();
	}
	for (Habit::IdStimulusSettingsPairList::const_iterator it = idsp1.begin(); it != idsp1.end(); it++)
	{
		m_mapSS.insert(it->first, it->second);
	}
	for (Habit::IdStimulusSettingsPairList::const_iterator it = idsp2.begin(); it != idsp2.end(); it++)
	{
		m_mapSS.insert(it->first, it->second);
	}
	for (Habit::IdStimulusSettingsPairList::const_iterator it = idsp3.begin(); it != idsp3.end(); it++)
	{
		m_mapSS.insert(it->first, it->second);
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
	Q_UNUSED(i);
	//Habit::StimulusSettings ss = m_mapSS[i];
	//QString s("Running (" + ss.getName() + ")");
	m_labelAttentionGetterStatusValue->setText("Idle");
	m_labelCurrentStimulusValue->setText("Running");
	//updateFileStatusLabels(ss);
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
	if (l.direction() == HLookDirection::LookLeft)
	{
		m_labelLookStatusValue->setText("Look Left");
	}
	else if (l.direction() == HLookDirection::LookCenter)
	{
		m_labelLookStatusValue->setText("Look Center");
	}
	else if (l.direction() == HLookDirection::LookRight)
	{
		m_labelLookStatusValue->setText("Look Right");
	}
	else if (l.direction() == HLookDirection::LookAway)
	{
		m_labelLookStatusValue->setText("Look Away");
	}
	else
	{
		qWarning("HControlPanel::onLook: UnknownLookDirection");
		m_labelLookStatusValue->setText("ERROR: Unknown dir");
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
	
	/*
	 * Set control buttons enabled/disabled as necessary
	 */
	
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
	m_psm->postEvent(new HAbortTrialEvent());
}

void HControlPanel::onStopTrials()
{
	m_psm->stop();
	connect(m_pmm, SIGNAL(cleared()), this, SLOT(onExperimentFinished()));
	m_pmm->clear();
}

void HControlPanel::onExperimentFinished()
{
	m_log.append(new HExperimentEndEvent(HElapsedTimer::elapsed()));
	if (m_pmm)
	{
		delete m_pmm;
		m_pmm = NULL;
	}
	accept();
}

void HControlPanel::onExperimentStarted()
{
	// post event to log to have offset of elapsed timer at exp start.
	// Obviously this is not to be taken as an absolute offset, but for
	// the purposes of reliability it will be useful.
	m_log.append(new HExperimentStartEvent(HElapsedTimer::elapsed()));
}

