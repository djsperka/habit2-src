/*
 *  HControlPanel.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HControlPanel.h"
#include "HPhase.h"
#include "HTrial.h"
#include "HPhaseCriteriaUtil.h"
#include "HMediaStatusWidget.h"
#include "HExperimentStatusWidget.h"
#include "HTrialGenerator.h"
#include "HElapsedTimer.h"
#include "maindao.h"
#include <QtCore/QTDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

using namespace GUILib;


HControlPanel::HControlPanel(const Habit::ExperimentSettings& exptSettings, HEventLog& log, const Habit::RunSettings& runSettings, HMediaManager *pmm, QWidget* w)
: QDialog(w)
, m_experimentSettings(exptSettings)
, m_log(log)
, m_runSettings(runSettings)
, m_pmm(pmm)
, m_psm(NULL)
{
	// generate gui elements and make it look pretty
	// There is a dependency in createExperiment to one of the ui components (stop trials button).
	// I should eliminate that, but for now we wait on connections() until expt is created.
	components();
	doLayout();
	m_pbStopTrials->setEnabled(false);
	m_pbNextTrial->setEnabled(false);

#ifndef NEWFANGLED_CREATE_EXPERIMENT
	// Generate the state machine and associated machinery that make the experiment run
	createExperiment(log);
#else

#endif


	// connections after expt created
	connections();
}

HControlPanel::~HControlPanel()
{
	qDebug() << "HControlPanel::~HControlPanel()";
	delete m_pmm;
}

void HControlPanel::setStateMachine(HStateMachine *psm)
{
	m_psm = psm;

	// connect the state machine's finished() signal to this dialog's close() slot
	connect(m_psm, SIGNAL(finished()), this, SLOT(onExperimentFinished()));
	connect(m_psm, SIGNAL(started()), this, SLOT(onExperimentStarted()));

	// Set some slots to update text labels in the control panel
	connect(&m_psm->experiment(), SIGNAL(phaseStarted(QString)), this, SLOT(onPhaseStarted(QString)));
	connect(&m_psm->experiment(), SIGNAL(trialStarted(int, int)), this, SLOT(onTrialStarted(int, int)));

	connect(&m_psm->experiment().getLookDetector(), SIGNAL(lookingDirection(QString)), this, SLOT(onLookingDirection(QString)));
}

void HControlPanel::components()
{
	this->setObjectName("HControlPanel");

	m_pMediaStatusWidget = new HMediaStatusWidget(m_experimentSettings.getStimulusDisplayInfo().getStimulusLayoutType(), m_experimentSettings.getControlBarOptions().isCurrentStimulusDisplayed(), this);
	m_pExperimentStatusWidget = new HExperimentStatusWidget(m_experimentSettings.getName(), m_experimentSettings.getControlBarOptions().isCurrentExperimentDisplayed(), this);

	qDebug() << "current stim? " << m_experimentSettings.getControlBarOptions().isCurrentStimulusDisplayed() << " current phase? " << m_experimentSettings.getControlBarOptions().isCurrentExperimentDisplayed();

	m_pbStartTrials = new QPushButton(tr("Start Trials"));
	m_pbNextTrial = new QPushButton(tr("Next Trial"));
	m_pbStopTrials = new QPushButton(tr("Stop Trials"));
}

void HControlPanel::connections()
{
	connect(m_pbStartTrials, SIGNAL(clicked()), this, SLOT(onStartTrials()));
	connect(m_pbNextTrial, SIGNAL(clicked()), this, SLOT(onNextTrial()));
	connect(m_pbStopTrials, SIGNAL(clicked()), this, SLOT(onStopTrials()));

	// connections for media status widget made here
	m_pMediaStatusWidget->setMediaManager(m_pmm);
}

void HControlPanel::doLayout()
{
	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(m_pbStartTrials);
	buttonsLayout->addWidget(m_pbNextTrial);
	buttonsLayout->addWidget(m_pbStopTrials);
	
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_pExperimentStatusWidget);
	mainLayout->addWidget(m_pMediaStatusWidget);
	mainLayout->addLayout(buttonsLayout);
	mainLayout->addStretch();
}


#ifndef NEWFANGLED_CREATE_EXPERIMENT  /* NEWFANGLED_CREATE_EXPERIMENT is defined */
void HControlPanel::createExperiment(HEventLog& log)
{
	HPhase* psPreTest = (HPhase *)NULL;
	HPhase* psHabituation = (HPhase *)NULL;
	HPhase* psTest = (HPhase *)NULL;
	HPhaseCriteria* pcritPreTest = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritHabituation = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritTest = (HPhaseCriteria *)NULL;
	HLookDetector* pld = (HLookDetector *)NULL;

	// Create look detector. Type of look detector is buried in the experiment settings; what we receive is a generic
	// HLookDetector* .
	pld = createLookDetector(m_experimentSettings, log, this);

	// connect attention() and look() signals to a slot so we can forward the info to the event log
	connect(pld, SIGNAL(attention()), this, SLOT(onAttention()));
	connect(pld, SIGNAL(look(HLook)), this, SLOT(onLook(HLook)));

	// Create media manager.
	// The stimuli configured for each phase are pulled using the experiment settings. 
	// One by one the stimuli are added to the media manager's player. As each stim is added, 
	// its ordinal position in the player's set of stim is paired with the StimulusSettings 
	// object, and the pair is stored in the StimulusSettingsList objects l1, l2, l3.
	m_pmm = createMediaManager(m_experimentSettings, this);

	// Need to know if AG is used. If it is, add attention getter settings to media manager
	if (m_experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed())
	{
		m_pmm->addAG(m_experimentSettings.getAttentionGetterSettings().getAttentionGetterStimulus());
	}

	// add stimuli for each phase.
	if (m_experimentSettings.getPreTestPhaseSettings().getIsEnabled())
		m_pmm->addStimuli(m_experimentSettings.getPreTestStimuliSettings());
	if (m_experimentSettings.getHabituationPhaseSettings().getIsEnabled())
		m_pmm->addStimuli(m_experimentSettings.getHabituationStimuliSettings());
	if (m_experimentSettings.getTestPhaseSettings().getIsEnabled())
		m_pmm->addStimuli(m_experimentSettings.getTestStimuliSettings());

	// Construct state machine.
	m_psm = new QStateMachine();
	
	// connect the state machine's finished() signal to this dialog's close() slot
	connect(m_psm, SIGNAL(finished()), this, SLOT(onExperimentFinished()));
	connect(m_psm, SIGNAL(started()), this, SLOT(onExperimentStarted()));

	// This is a single super-state that holds all the phases.
	HExperiment* sExperiment = new HExperiment(log, *m_pmm, *pld);
	m_psm->addState(sExperiment);
	m_psm->setInitialState(sExperiment);
	QFinalState* sFinal = new QFinalState;
	m_psm->addState(sFinal);
	sExperiment->addTransition(sExperiment, SIGNAL(finished()), sFinal);
 	//QObject::connect(sExperiment, SIGNAL(playStim(int)), m_pmm, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()

 	//TEMPsExperiment->addTransition(m_pbStopTrials, SIGNAL(clicked()), sFinal);

 	// Create phases.
	// Each HPhase gets a list of stimuli, as pairs of <int, StimulusSettings>, and these are used in order as the
	// stimuli for that phase. The <int> part is passed to the media manager as the key to play that stimulus.
	//
	if (m_experimentSettings.getPreTestPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = m_experimentSettings.getPreTestPhaseSettings();
		QList< QPair<int, QString> > list;
		QList<unsigned int> stimidListInitial;
		QList< QPair<int, QString> > stimidListOrdered;

		// Get the list of ALL stim keys for the PreTest stimuli.
		// These are in the order that they were added to the stim list, which is the same as the
		// order they are pulled from the DB.
		// Any randomization or other ordering will come later.
		m_pmm->getContextStimList(HStimContext::PreTestPhase, stimidListInitial);

		// Now get the order list from HRunSettings.
		// This order list should consist of numbers from 0...n-1, where
		// n is the number of stimuli configured for this phase.
		if (!m_runSettings.getPretestOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse Pretest order (" << m_runSettings.getPretestOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), m_runSettings.isPretestRandomized(), m_runSettings.getPretestRandomizeMethod()==1);
		for (unsigned int i=0; i<ps.getNTrials(); i++)
		{
			// Note: prior to version 2.1.0, the line below (with the "-1") was used.
			// That was because the way orders were specified had used 1-based indexing.
			// With the addition of configured orders I've done away with 1-based in lieu
			// of regular old 0-based indices to eliminate a LOT of confusion in
			// manipulating the orders.
			//stimidListOrdered.append(stimidListInitial.at(list.at(htg.next()) - 1));
			int itmp = htg.next();
			stimidListOrdered.append(QPair<int, QString>(stimidListInitial.at(list.at(itmp).first), list.at(itmp).second));
		}
		pcritPreTest = new HPhaseFixedNCriteria(ps.getNTrials());
		psPreTest = new HPhase(*sExperiment, pcritPreTest, log, stimidListOrdered, ps, m_experimentSettings.getHLookSettings(), m_experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed());

	}

	if (m_experimentSettings.getHabituationPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = m_experimentSettings.getHabituationPhaseSettings();
		QList< QPair<int, QString> > list;
		QList<unsigned int> stimidListInitial;
		QList< QPair<int, QString> > stimidListOrdered;

		m_pmm->getContextStimList(HStimContext::HabituationPhase, stimidListInitial);

		if (!m_runSettings.getHabituationOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse habituation order (" << m_runSettings.getHabituationOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), m_runSettings.isHabituationRandomized(), m_runSettings.getHabituationRandomizeMethod()==1);
		for (unsigned int i=0; i<ps.getNTrials(); i++)
		{
			// Note: prior to version 2.1.0, the line below (with the "-1") was used.
			// That was because the way orders were specified had used 1-based indexing.
			// With the addition of configured orders I've done away with 1-based in lieu
			// of regular old 0-based indices to eliminate a LOT of confusion in
			// manipulating the orders.
			//stimidListOrdered.append(stimidListInitial.at(list.at(htg.next()) - 1));
			int itmp = htg.next();
			stimidListOrdered.append(QPair<int, QString>(stimidListInitial.at(list.at(itmp).first), list.at(itmp).second));
		}
		
		// Create habituation criteria object. 
		pcritHabituation = createPhaseCriteria(m_experimentSettings.getHabituationSettings(), ps.getNTrials());
		psHabituation = new HPhase(*sExperiment, pcritHabituation, log, stimidListOrdered, ps, m_experimentSettings.getHLookSettings(), m_experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed());
	}
	
	if (m_experimentSettings.getTestPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = m_experimentSettings.getTestPhaseSettings();
		QList< QPair<int, QString> > list;
		QList<unsigned int> stimidListInitial;
		QList< QPair<int, QString> > stimidListOrdered;

		m_pmm->getContextStimList(HStimContext::TestPhase, stimidListInitial);
		if (!m_runSettings.getTestOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse test order (" << m_runSettings.getTestOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), m_runSettings.isTestRandomized(), m_runSettings.getTestRandomizeMethod()==1);
		for (unsigned int i=0; i<ps.getNTrials(); i++)
		{
			// Note: prior to version 2.1.0, the line below (with the "-1") was used.
			// That was because the way orders were specified had used 1-based indexing.
			// With the addition of configured orders I've done away with 1-based in lieu
			// of regular old 0-based indices to eliminate a LOT of confusion in
			// manipulating the orders.
			//stimidListOrdered.append(stimidListInitial.at(list.at(htg.next()) - 1));
			int itmp = htg.next();
			stimidListOrdered.append(QPair<int, QString>(stimidListInitial.at(list.at(itmp).first), list.at(itmp).second));
		}
		
		pcritTest = new HPhaseFixedNCriteria(ps.getNTrials());
		psTest = new HPhase(*sExperiment, pcritTest, log, stimidListOrdered, ps, m_experimentSettings.getHLookSettings(), m_experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed());
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


	// Store the stimulus settings events in the log
	QMapIterator<unsigned int, const Habit::StimulusSettings *> it(m_pmm->pmap());
	while (it.hasNext())
	{
		it.next();
		log.append(new HStimulusSettingsEvent(*it.value(), it.key()));
	}
	
	// Set some slots to update text labels in the control panel
	connect(sExperiment, SIGNAL(phaseStarted(QString)), this, SLOT(onPhaseStarted(QString)));
	connect(sExperiment, SIGNAL(trialStarted(int, int)), this, SLOT(onTrialStarted(int, int)));

}
#endif

void HControlPanel::onPhaseStarted(QString phaseName)
{
	// The object name(s) of the phase objects has to be set to the phase name!
	m_pExperimentStatusWidget->setPhase(phaseName);
}

void HControlPanel::onTrialStarted(int trialindex, int repeatindex)
{
	m_pExperimentStatusWidget->setTrial(trialindex, repeatindex);
	return;
}

void HControlPanel::onLookingDirection(QString sLookingDirection)
{
	m_pExperimentStatusWidget->setLooking(sLookingDirection);
	return;
}

void HControlPanel::onAttention()
{
	m_log.append(new HAttentionEvent(HElapsedTimer::elapsed()));
}

void HControlPanel::onLook(HLook l)
{
	m_log.append(new HLookEvent(l, HElapsedTimer::elapsed()));
}

void HControlPanel::onStartTrials()
{
	Q_ASSERT(m_psm);

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
	m_log.append(new HAbortTrialEvent(HElapsedTimer::elapsed()));
	m_psm->postEvent(new HAbortTrialQEvent());
}

void HControlPanel::onStopTrials()
{
	m_log.append(new HExperimentQuitEvent(HElapsedTimer::elapsed()));
	m_psm->stop();
	m_pmm->clear();
	accept();
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

void HControlPanel::keyPressEvent(QKeyEvent *e)
{
    if(e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
    else
    {
    	onStopTrials(); 	// this will lead to accept()
    }
}

void HControlPanel::closeEvent(QCloseEvent* e)
{
	onStopTrials();
	Q_UNUSED(e);
}

void HControlPanel::onExperimentStarted()
{
	// post event to log to have offset of elapsed timer at exp start.
	// Obviously this is not to be taken as an absolute offset, but for
	// the purposes of reliability it will be useful.
	m_log.append(new HExperimentStartEvent(HElapsedTimer::elapsed()));

	// Update Control panel status
	m_pExperimentStatusWidget->ui->labelStatus->setText(QString("Running"));
}

