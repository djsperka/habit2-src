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
#include <QtCore/QtDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

using namespace GUILib;


HControlPanel::HControlPanel(const Habit::ExperimentSettings& exptSettings, HEventLog& log, const Habit::RunSettings& runSettings, HGMM *pmm, QWidget* w)
: QDialog(w)
, m_experimentSettings(exptSettings)
, m_log(log)
, m_runSettings(runSettings)
, m_pmm(pmm)
, m_psm(NULL)
{
	// generate gui elements and make it look pretty
	components();
	doLayout();
	m_pbStopTrials->setEnabled(false);
	m_pbNextTrial->setEnabled(false);

	// connections used to be after expt created, still down here.
	connections();
}

HControlPanel::~HControlPanel()
{
}

void HControlPanel::setStateMachine(HStateMachine *psm)
{
	m_psm = psm;

	// connect the state machine's finished() signal to this dialog's close() slot
	connect(m_psm, SIGNAL(finished()), this, SLOT(onExperimentFinished()));
	connect(m_psm, SIGNAL(started()), this, SLOT(onExperimentStarted()));

	// Set some slots to update text labels in the control panel
	connect(&m_psm->experiment(), SIGNAL(phaseStarted(QString, int)), this, SLOT(onPhaseStarted(QString, int)));
	connect(&m_psm->experiment(), SIGNAL(trialStarted(int, unsigned int, unsigned int)), this, SLOT(onTrialStarted(int, unsigned int, unsigned int)));

	connect(&m_psm->experiment().getLookDetector(), SIGNAL(lookingDirection(QString)), this, SLOT(onLookingDirection(QString)));
}

void HControlPanel::components()
{
	this->setObjectName("HControlPanel");

	m_pMediaStatusWidget = new HMediaStatusWidget(m_experimentSettings.getStimulusDisplayInfo().getStimulusLayoutType(), m_experimentSettings.getControlBarOptions().isCurrentStimulusDisplayed(), this);
//	m_pExperimentStatusWidget = new HExperimentStatusWidget(
//			m_experimentSettings.getName(),
//			m_experimentSettings.getControlBarOptions().isCurrentExperimentDisplayed(),
//			m_experimentSettings.getControlBarOptions().isCurrentStimulusDisplayed(), this);
	m_pExperimentStatusWidget = new HExperimentStatusWidget(m_experimentSettings.getName(), m_experimentSettings.getControlBarOptions().isCurrentExperimentDisplayed(), this);

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

void HControlPanel::onPhaseStarted(QString phaseName, int context)
{
	qDebug() << "onPhaseStarted " << phaseName << " " << context;
	m_pExperimentStatusWidget->setPhase(phaseName);
}

void HControlPanel::onTrialStarted(int context, unsigned int trialindex, unsigned int repeatindex)
{
	m_pExperimentStatusWidget->setTrial(trialindex, repeatindex);
	return;
}

void HControlPanel::onNextTrial()
{
	m_log.append(new HAbortTrialEvent(HElapsedTimer::elapsed()));
	m_psm->postEvent(new HAbortTrialQEvent());
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

void HControlPanel::onStopTrials()
{
	//qDebug() << "HControlPanel::onStopTrials()";
	m_log.append(new HExperimentQuitEvent(HElapsedTimer::elapsed()));
	//m_psm->stop();
	//m_pmm->clear();
	//qDebug() << "HControlPanel::onStopTrials() - callaccept";
	accept();
}

void HControlPanel::onExperimentFinished()
{
	m_log.append(new HExperimentEndEvent(HElapsedTimer::elapsed()));
//	if (m_pmm)
//	{
//		delete m_pmm;
//		m_pmm = NULL;
//	}
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
	// Dump position info re: Full screen startup
	//
	qDebug() << "HControlPanel::onExperimentStarted(): screen geometry of dlg is " << this->geometry();

	// post event to log to have offset of elapsed timer at exp start.
	// Obviously this is not to be taken as an absolute offset, but for
	// the purposes of reliability it will be useful.
	m_log.append(new HExperimentStartEvent(HElapsedTimer::elapsed()));

	// Update Control panel status
	m_pExperimentStatusWidget->ui->labelStatus->setText(QString("Running"));
}

