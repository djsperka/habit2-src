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
#include "HPhaseSettings.h"
#include "H2MainWindow.h"
#include "HMediaManager.h"
#include "HDBException.h"
#include "HMediaManagerUtil.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDebug>
#include <QMessageBox>
#include <QList>
#include <QListIterator>


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

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    vbox->addWidget(buttonBox);

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

	// pre-flight check. Verify that monitors have been configured, etc.
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


	// Create media manager for this experiment
	// TODO memory leak here if open multiple times?
	m_pmm = createMediaManager(settings);

	// connect to media manager signals
	connect(this, SIGNAL(stim(int)), m_pmm, SLOT(stim(int)));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	connect(m_pmm, SIGNAL(screen(int, QString)), this, SLOT(screen(int, QString)));

	if (m_pmm->pmap().contains(HMediaManager::backgroundKey))
	{
		m_stimIndices.append(HMediaManager::backgroundKey);
		m_stimNames.append("BACKGROUND");
	}

	// populate (from createExperiment())
	// Need to know if AG is used. If it is, add attention getter settings to media manager
	if (settings.getAttentionGetterSettings().isAttentionGetterUsed() || settings.getAttentionGetterSettings().isFixedISI())
	{
		m_pmm->addAG(settings.getAttentionGetterSettings().getAttentionGetterStimulus());

		m_stimIndices.append(HMediaManager::agKey);
		m_stimNames.append(settings.getAttentionGetterSettings().getAttentionGetterStimulus().getName());
	}


	QListIterator<Habit::HPhaseSettings> phaseIterator = settings.phaseIterator();
	while (phaseIterator.hasNext())
	{
		const Habit::HPhaseSettings& ps = phaseIterator.next();
//		QList< QPair<int, QString> > list;
		QList<unsigned int> stimidListInitial;
//		QList< QPair<int, QString> > stimidListOrdered;

		if (ps.getIsEnabled())
		{
			m_pmm->addStimuli(ps.stimuli(), ps.getSeqno());

			QList<unsigned int> stimidListInitial;
			m_pmm->getContextStimList(ps.getSeqno(), stimidListInitial);

			QListIterator<unsigned int> it(stimidListInitial);
			unsigned int uistim;
			while (it.hasNext())
			{
				uistim = it.next();
//				qDebug() << "phase " << ps.getName() << " stim " << uistim << " name " << m_pmm->pmap()[uistim]->getName();
				m_stimIndices.append(uistim);
				m_stimNames.append(QString("%1:%2").arg(ps.getName()).arg(m_pmm->pmap()[uistim]->getName()));
			}
		}
	}

	QListIterator<unsigned int> itIndices(m_stimIndices);
	QListIterator<QString> itNames(m_stimNames);

	while (itIndices.hasNext())
	{
		qDebug() << itIndices.next() << " " << itNames.next();
	}


	// Now create a controller and exec() it
	TestMMController controller(m_stimNames);
	connect(&controller, SIGNAL(playItem(unsigned int)), this, SLOT(playItem(unsigned int)));
	connect(&controller, SIGNAL(stopItem()), this, SLOT(stopItem()));
	controller.exec();


	// clean up
	m_stimIndices.clear();
	m_stimNames.clear();
	delete m_pmm;

}


void TestMMDialog::playItem(unsigned int uistim)
{
	emit stim(m_stimIndices[uistim]);
}

void TestMMDialog::stopItem()
{
	emit stim(m_stimIndices[0]);
}


void TestMMDialog::stimStarted(int index)
{
	qDebug() << "started index " << index;
	qDebug() << "pmap " << m_pmm->pmap()[index]->getName();
}

void TestMMDialog::screen(int screenid, QString filename)
{
	qDebug() << "screen " << screenid << " " << filename;
}
