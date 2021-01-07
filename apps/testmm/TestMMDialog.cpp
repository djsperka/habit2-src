/*
 * TestMMDialog.cpp
 *
 *  Created on: Feb 22, 2017
 *      Author: dan
 */

#include "TestMMDialog.h"
#include "TestMMController.h"
#include "HExperimentListWidget.h"
#include "experimentsettings.h"
#include "HRunSettingsDialog.h"
#include "HStimulusPreviewWidget.h"
#include "HPhaseSettings.h"
#include "H2MainWindow.h"
#include "HGMM.h"
#include "HDBException.h"
#include "HMediaManagerUtil.h"
#include "HExperimentUtil.h"
#include "HWorkspaceUtil.h"
#include "HStimuliSettingsWidget.h"
#include "HLoggerObject.h"
#include "HLoggerWidget.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QList>
#include <QListIterator>
#include <QProcess>


TestMMDialog::TestMMDialog(QWidget *parent)
: QDialog(parent)
{
	components();
}

TestMMDialog::~TestMMDialog()
{

}


void TestMMDialog::components()
{
	QVBoxLayout *vbox = new QVBoxLayout;
	GUILib::HExperimentListWidget *list = new GUILib::HExperimentListWidget(this, true, true);
	vbox->addWidget(list);
	connect(list, SIGNAL(experimentActivated(const QString&)), this, SLOT(experimentActivated(const QString&)));
	m_pFullScreen = new QCheckBox("Use full screen (using current preferences)?");
	vbox->addWidget(m_pFullScreen);
	m_pSSWidget = new QCheckBox("Use stimuli settings widget");
	vbox->addWidget(m_pSSWidget);
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    vbox->addWidget(buttonBox);
    vbox->addWidget(new HLoggerWidget(this));

    this->setLayout(vbox);
}

void TestMMDialog::experimentActivated(QString expt)
{
	qDebug() << "Got expt " << expt;

	Habit::ExperimentSettings settings;
	try
	{
		settings.loadFromDB(expt);
	}
	catch (const Habit::HDBException& e)
	{
		QMessageBox::critical(this, "Cannot load experiment", "Cannot load experiment from database!");
		qCritical() << "Cannot load experiment \"" << expt << "\" from database.";
		qCritical() << e.what();
		return;
	}

	// pre-flight check.
	QStringList sProblems;
	if (!GUILib::H2MainWindow::checkExperimentSettings(settings, sProblems))
	{
		QMessageBox msgBox;
		msgBox.setText("This experiment cannot be run.");
		msgBox.setInformativeText("Experiment settings are not complete.");
		msgBox.setDetailedText(sProblems.join("\n"));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();
		return;
	}

	GUILib::HRunSettingsDialog *pRunSettingsDialog = new GUILib::HRunSettingsDialog(settings, true, NULL);
	int i = pRunSettingsDialog->exec();
	if (i != QDialog::Accepted) return;



	m_pmm = createMediaManager(settings.getStimulusDisplayInfo(), 320, 240);

#if 0
	// initialize mm, populate with stimuli and ag
	QDir rootDir = habutilGetStimulusRootDir();
	m_pmm = new HGMM()
	m_pmm->reset(settings, rootDir);
#endif


	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	connect(m_pmm, SIGNAL(agStarted(int)), this, SLOT(agStarted(int)));
	connect(m_pmm, SIGNAL(screen(int, QString)), this, SLOT(screen(int, QString)));

	// clear indices and names
	m_stimIndices.clear();
	m_stimNames.clear();

	// The media manager should have a default stim.
	m_stimIndices.append(m_pmm->getDefaultKey());
	m_stimNames.append(QString("default"));

	// Is there an attention getter?
	if (settings.getAttentionGetterSettings().isAttentionGetterUsed() || settings.getAttentionGetterSettings().isFixedISI())
	{
		m_stimIndices.append(m_pmm->addStimulus(settings.getAttentionGetterSettings().getAttentionGetterStimulus(), -1));
		m_stimNames.append(QString("AG: %1").arg(settings.getAttentionGetterSettings().getAttentionGetterStimulus().getName()));
	}

	// Now each phase, but only if enabled
	QListIterator<Habit::HPhaseSettings> phaseIterator = settings.phaseIterator();
	while (phaseIterator.hasNext())
	{
		const Habit::HPhaseSettings& ps = phaseIterator.next();
		QList< QPair<int, QString> > stimidListOrdered;

		if (ps.getIsEnabled())
		{
			Habit::PhaseRunSettings prs(pRunSettingsDialog->getRunSettings().map().value(ps.getSeqno()));
			Habit::HStimulusSettingsList sslist(getOrderOfStimuli(prs, ps.habituationSettings().getNTrials(), ps.stimuli()));
			for (Habit::StimulusSettingsListIterator it = sslist.begin(); it!=sslist.end(); it++)
			{
				m_stimIndices.append(m_pmm->addStimulus(*it, ps.getSeqno()));
				m_stimNames.append(it->getName());
				qDebug() << "Stim added " << it->getName();
			}
		}
	}


	// Now create a controller and exec() it
	TestMMController controller(m_stimNames, m_pmm->createStimulusWidget());
	connect(&controller, SIGNAL(playItem(unsigned int)), this, SLOT(playItem(unsigned int)));
	connect(m_pmm, SIGNAL(stimStarted(int)), &controller, SLOT(stimStarted(int)));
	connect(m_pmm, SIGNAL(agStarted(int)), &controller, SLOT(stimStarted(int)));
	controller.exec();


	// clean up
	//		m_stimIndices.clear();
	//		m_stimNames.clear();
	//		delete m_pmm;
	delete m_pmm;
	m_pmm = NULL;
}


void TestMMDialog::playItem(unsigned int uistim)
{
	qDebug() << "playItem(" << uistim << ") : " << m_stimIndices[uistim] << " stim";
	m_pmm->stim(m_stimIndices[uistim]);
}

void TestMMDialog::stopItem()
{
	qDebug() << "stopItem()";
	m_pmm->background();
	//m_pmm->defaultStim();
}


void TestMMDialog::stimStarted(int index)
{
	qDebug() << "TestMMDialog::stimStarted(" << index << ")";
}

void TestMMDialog::agStarted(int index)
{
	qDebug() << "TestMMDialog::agStarted(" << index << ")";
}

void TestMMDialog::screen(int screenid, QString filename)
{
	qDebug() << "screen " << screenid << " " << filename;
}
