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
#include "HPhaseSettings.h"
#include "H2MainWindow.h"
#include "HGMM.h"
#include "HDBException.h"
#include "HMediaManagerUtil.h"
#include "HExperimentUtil.h"
#include "HWorkspaceUtil.h"
#include "HStimuliSettingsWidget.h"
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
	if (!GUILib::H2MainWindow::checkExperimentSettings(settings, sProblems, m_pFullScreen->isChecked()))
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

	if (m_pSSWidget->isChecked())
	{
		// Select phase
		//QMessageBox::information(this, "ifno", "select phase");

		bool ok;
		QString item = QInputDialog::getItem(this, tr("Select phase"), "Phase", settings.getPhaseNames(), 0, false, &ok);
		if (ok && !item.isEmpty())
		{
			qDebug() << "Got phase " << item;
			//Habit::HPhaseSettings ps = settings.phaseValue(item);
			//GUILib::HStimuliSettingsWidget *pssw = new GUILib::HStimuliSettingsWidget(item, ps.stimuli(), settings.getStimulusDisplayInfo(), this);
			// display just that widget? Need a means of escape...
		}
	}
	else
	{
		GUILib::HRunSettingsDialog *pRunSettingsDialog = new GUILib::HRunSettingsDialog(settings, true, NULL);
		int i = pRunSettingsDialog->exec();
		if (i != QDialog::Accepted) return;

		// initialize mm, populate with stimuli and ag
		QDir rootDir = habutilGetStimulusRootDir();
		m_pmm = &HGMM::instance();
		m_pmm->reset(settings, rootDir);

		//
		// connect to media manager signals
		connect(this, SIGNAL(stim(unsigned int)), m_pmm, SLOT(stim(unsigned int)));
		connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
		connect(m_pmm, SIGNAL(screen(int, QString)), this, SLOT(screen(int, QString)));

		// clear indices and names
		m_stimIndices.clear();
		m_stimNames.clear();

		// Now populate the media manager with default stim
		m_stimIndices.append(m_pmm->getDefaultKey());
		m_stimNames.append(QString("default"));

		// save ag key if needed
		bool bAGUsed = (settings.getAttentionGetterSettings().isAttentionGetterUsed() || settings.getAttentionGetterSettings().isFixedISI());


		QListIterator<Habit::HPhaseSettings> phaseIterator = settings.phaseIterator();
		while (phaseIterator.hasNext())
		{
			const Habit::HPhaseSettings& ps = phaseIterator.next();
			QList< QPair<int, QString> > stimidListOrdered;

			// First index is background
			m_stimIndices.append(m_pmm->getBackgroundKey());
			m_stimNames.append("Background");
			if (ps.getIsEnabled())
			{
				getOrderedStimidList(m_pmm, ps, pRunSettingsDialog->getRunSettings(), stimidListOrdered);

				qDebug() << "stimid list for phase " << ps.getName() << " seqno " << ps.getSeqno();
				QListIterator< QPair<int, QString> > it(stimidListOrdered);
				while (it.hasNext())
				{
					// if ag is used, put it in the list
					if (bAGUsed)
					{
						m_stimIndices.append(m_pmm->getAGKey());
						m_stimNames.append(QString("AG: %1").arg(m_pmm->getStimulusSettings(m_pmm->getAGKey()).getName()));
					}

					QPair<int, QString> p = it.next();
					qDebug() << "phase " << ps.getName() << " stim " << p.first << " name " << m_pmm->getStimulusSettings(p.first).getName();
					m_stimIndices.append(p.first);
					m_stimNames.append(QString("%1:%2").arg(ps.getName()).arg(m_pmm->getStimulusSettings(p.first).getName()));
				}
				qDebug() << "stimid list for phase " << ps.getName() << " seqno " << ps.getSeqno() << " DONE";
			}
		}

		// get display widget(s)
		if (settings.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
			m_pmm->setWidgets(new HStimulusWidget(settings.getStimulusDisplayInfo(), 1024, 768));
		else
			m_pmm->setWidgets(new HStimulusWidget(settings.getStimulusDisplayInfo(), 1024, 768), new HStimulusWidget(settings.getStimulusDisplayInfo(), 1024, 768));

		QDialog *d = m_pmm->createStimulusWidget();

		// Now create a controller and exec() it
		TestMMController controller(m_stimNames, d);
		connect(&controller, SIGNAL(playItem(unsigned int)), this, SLOT(playItem(unsigned int)));
		connect(&controller, SIGNAL(stopItem()), this, SLOT(stopItem()));
		controller.exec();


		// clean up
		//		m_stimIndices.clear();
		//		m_stimNames.clear();
		//		delete m_pmm;
		m_pmm->reset();
		m_pmm = NULL;
	}
}


void TestMMDialog::playItem(unsigned int uistim)
{
	qDebug() << "playItem(" << uistim << ") : " << m_stimIndices[uistim];
	emit stim(m_stimIndices[uistim]);
}

void TestMMDialog::stopItem()
{
	qDebug() << "stopItem()";
	emit stim(m_stimIndices[0]);
}


void TestMMDialog::stimStarted(int index)
{
	qDebug() << "started index " << index;
}

void TestMMDialog::screen(int screenid, QString filename)
{
	qDebug() << "screen " << screenid << " " << filename;
}
