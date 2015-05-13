/*
 * H2MainWindow.cpp
 *
 *  Created on: Sep 5, 2014
 *      Author: Oakeslab
 */




#include <QtGui>
#include <QtDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegExp>

#include "H2MainWindow.h"
#include "HExperimentListWidget.h"
#include "experimentsettings.h"
#include "HExperimentMain.h"
#include "HGlobalPreferencesDialog.h"
#include "HWorkspaceUtil.h"
#include "HRunSettingsDialog.h"
#include "HControlPanel.h"
#include "HEventLog.h"
#include "HResultsDialog.h"
#include "HStimulusUtil.h"
#include "HResultsExplorerDialog.h"

using namespace GUILib;
using namespace Habit;


GUILib::H2MainWindow::H2MainWindow(bool bTestRun)
: QMainWindow()
, m_bTestRunDefault(bTestRun)
{
	m_pExperimentListWidget = new GUILib::HExperimentListWidget();
	setCentralWidget(m_pExperimentListWidget);
    createActions();
    createToolBars();

    setWindowTitle(QString("%1 - %2").arg("Habit").arg(QCoreApplication::instance()->applicationVersion()));

    connect(m_pExperimentListWidget, SIGNAL(experimentActivated(QString)), this, SLOT(experimentActivated(QString)));
    connect(m_pExperimentListWidget->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
    		this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));

    // dialog should open with no selection; clear it just in case
    m_pExperimentListWidget->clearSelection();

    setUnifiedTitleAndToolBarOnMac(true);
 }


void GUILib::H2MainWindow::experimentActivated(QString expt)
{
	// Get experiment settings
	Habit::ExperimentSettings settings;
	if (Habit::ExperimentSettings::load(settings, expt))
	{
		HExperimentMain *exptMain = new HExperimentMain(settings, this);
		exptMain->exec();
	}
}

void GUILib::H2MainWindow::closeEvent(QCloseEvent *event)
{
	event->accept();
}

void GUILib::H2MainWindow::createActions()
{
    m_actionNew = new QAction(QIcon(":/resources/plus.png"), tr("&New"), this);
    m_actionNew->setShortcuts(QKeySequence::New);
    m_actionNew->setStatusTip(tr("Create a new experiment"));
    connect(m_actionNew, SIGNAL(triggered()), this, SLOT(newExperiment()));

	m_actionEdit = new QAction(QIcon(":/resources/edit.png"), tr("&Edit"), this);
	//m_actionEdit->setShortcuts(QKeySequence::);
	m_actionEdit->setStatusTip(tr("Edit settings of selected experiment"));
    connect(m_actionEdit, SIGNAL(triggered()), this, SLOT(editExperiment()));

    m_actionClone = new QAction(QIcon(":/resources/copy.png"), tr("&Clone"), this);
    //cloneAct->setShortcuts(QKeySequence::Copy);
    m_actionClone->setStatusTip(tr("Clone the selected experiment"));
    connect(m_actionClone, SIGNAL(triggered()), this, SLOT(cloneExperiment()));

    m_actionDelete = new QAction(QIcon(":/resources/delete.png"), tr("&Delete"), this);
    m_actionDelete->setShortcuts(QKeySequence::Delete);
    m_actionDelete->setStatusTip(tr("Delete selected experiment"));
    connect(m_actionDelete, SIGNAL(triggered()), this, SLOT(deleteExperiment()));

    m_actionImport = new QAction(QIcon(":/resources/import.png"), tr("&Import"), this);
    m_actionImport->setStatusTip(tr("Import an experiment"));
    connect(m_actionImport, SIGNAL(triggered()), this, SLOT(importExperiment()));

    m_actionRun = new QAction(QIcon(":/resources/run.png"), tr("&Run"), this);
    //m_actionRun->setShortcuts(QKeySequence::New);
    m_actionRun->setStatusTip(tr("Run selected experiment"));
    connect(m_actionRun, SIGNAL(triggered()), this, SLOT(runExperiment()));

    m_actionResults = new QAction(QIcon(":/resources/results.png"), tr("Results"), this);
    m_actionResults->setStatusTip(tr("Open results file"));
    connect(m_actionResults, SIGNAL(triggered()), this, SLOT(openResultsFile()));

    m_actionPreferences = new QAction(QIcon(":/resources/gear_wheel.png"), tr("&Preferences"), this);
    //m_actionRun->setShortcuts(QKeySequence::New);
    m_actionPreferences->setStatusTip(tr("Edit Habit preferences for this computer"));
    connect(m_actionPreferences, SIGNAL(triggered()), this, SLOT(editPreferences()));

}


void GUILib::H2MainWindow::createToolBars()
{
    m_pToolBar = addToolBar(tr("Experiment"));
    m_pToolBar->addAction(m_actionNew);
    m_pToolBar->addAction(m_actionEdit);
    m_pToolBar->addAction(m_actionClone);
    m_pToolBar->addAction(m_actionImport);
    m_pToolBar->addAction(m_actionDelete);
    m_pToolBar->addAction(m_actionRun);
    m_pToolBar->addAction(m_actionResults);
    m_pToolBar->addAction(m_actionPreferences);
}



QString GUILib::H2MainWindow::getExperimentCopyName(const QString& expt)
{
	QString copy = QString("%1 - Copy");
	QStringList exptList = m_pExperimentListWidget->experimentList();
	int count = 0;

	copy = copy.arg(expt);	// this is the experiment name if no copies found

	QStringListIterator it(exptList);
	while (it.hasNext())
	{
		QString tmp = it.next();
		if (tmp.contains(copy))
		{
			count++;
		}
	}

	if (count>0) copy = QString("%1 - Copy(%2)").arg(expt).arg(count+1);

	return copy;
}

QString GUILib::H2MainWindow::getExperimentImportName(const QString& expt)
{
	QString import = QString("%1 - Import");
	QStringList exptList = m_pExperimentListWidget->experimentList();
	int count = 0;

	import = import.arg(expt);	// this is the experiment name if no copies found

	QStringListIterator it(exptList);
	while (it.hasNext())
		if (it.next().contains(import)) count++;

	if (count>0) import = QString("%1 - Import(%2)").arg(expt).arg(count+1);

	return import;
}


QString GUILib::H2MainWindow::getExperimentNewName()
{
	QString newExpt = QString("New Experiment");
	QString re = QString("^(New Experiment.*)$");
	QStringList exptList = m_pExperimentListWidget->experimentList();
	int count = 0;

	QStringListIterator it(exptList);
	while (it.hasNext())
		if (it.next().contains(re)) count++;

	if (count>0) newExpt = QString("New Experiment (%2)").arg(count+1);

	return newExpt;
}



bool GUILib::H2MainWindow::inputExperimentName(QString& newName, const QString defaultName)
{
	bool bOK = false;
	newName = QInputDialog::getText(this, "Enter Experiment Name", "Experiment Name:", QLineEdit::Normal, defaultName, &bOK);
	newName = newName.trimmed();
    if (bOK)
    {
    	// Validate experiment name
    	if (newName.isEmpty())
		{
			QMessageBox::warning(this, "Bad Experiment Name", "The experiment name is empty.");
			bOK = false;
		}
		else if (!newName.contains(QRegExp("^[a-zA-Z0-9-]([a-zA-Z0-9- ()]*[a-zA-Z0-9-()])?$")))
		{
			QMessageBox::warning(this, "Bad Experiment Name", "The experiment name can only contain letters, numbers, and the characters \"-\", \"(\", and \")\". Spaces are allowed in the interior of the name, but are trimmed from the beginning and end.");
			bOK = false;
		}
    }
	return bOK;
}

void GUILib::H2MainWindow::newExperiment()
{
	Habit::ExperimentSettings settings;
	QString sName;
	QString sDefault;	// initial/default name

	// Create default experiment name for the lazy
	sDefault = getExperimentNewName();

	if (inputExperimentName(sName, sDefault))
	{
		settings.setName(sName);
		HExperimentMain *exptMain = new HExperimentMain(settings, this);
		exptMain->exec();

		// Get experiment list widget to update itself
		m_pExperimentListWidget->reload();
	}
}

void GUILib::H2MainWindow::editExperiment()
{
	QString expt = m_pExperimentListWidget->selectedExperiment();
	if (expt.isEmpty())
	{
		qCritical() << "No experiment selected! Edit not allowed when no selection made - check HExperimentListWidget and H2MainWindow";
	}
	else
	{
		Habit::ExperimentSettings settings;
		if (Habit::ExperimentSettings::load(settings, expt))
		{
			// Show experiment main window
			HExperimentMain *exptMain = new HExperimentMain(settings, this);
			exptMain->exec();
		}
	}
}

void GUILib::H2MainWindow::importExperiment()
{
	QString filename = QFileDialog::getOpenFileName(NULL, "Import experiment file", habutilGetResultsDir().absolutePath(), "Habit Export Files (*.hbx)");
	qDebug() << "Got filename for import: " << filename;

	// Open and read the file
	ExperimentSettings settings;

	if (ExperimentSettings::load(filename, settings))
	{
		// first check that the experiment name is not a duplicate
		QString expt = getExperimentImportName(settings.getName());
		settings.setName(expt);

		// Show experiment main window
		HExperimentMain *exptMain = new HExperimentMain(settings, this);
		exptMain->exec();

		// Get experiment list widget to update itself
		m_pExperimentListWidget->reload();
	}

}



void GUILib::H2MainWindow::openResultsFile()
{
#if 0
	QString filename = QFileDialog::getOpenFileName(NULL, "Open Results File", habutilGetResultsDir().absolutePath(), "Habit Results Files (*.hab)");
	if (!filename.isNull())
	{
		showResultsFile(filename);
	}
#endif
	HResultsExplorerDialog dlg(habutilGetResultsDir(), this);
	dlg.exec();
}

void GUILib::H2MainWindow::showResultsFile(const QString filename)
{
	HResults* results = HResults::load(filename);
	if (!results)
	{
		QMessageBox msgBox;
		msgBox.setText("Cannot load data.");
		msgBox.exec();
	}
	else
	{
		HResultsDialog dialog(*results, NULL);
		dialog.exec();
		delete results;
	}
	return;
}






bool GUILib::H2MainWindow::checkExperimentSettings(const Habit::ExperimentSettings& settings, QStringList& sProblems)
{
	bool b = true;
	sProblems.clear();
	int iControl = habutilGetMonitorID(HPlayerPositionType::Control);
	int iLeft = habutilGetMonitorID(HPlayerPositionType::Left);
	int iCenter = habutilGetMonitorID(HPlayerPositionType::Center);
	int iRight = habutilGetMonitorID(HPlayerPositionType::Right);
	const HStimulusLayoutType& layoutType = settings.getStimulusDisplayInfo().getStimulusLayoutType();

	// Control monitor specified?
	if (iControl < 0)
	{
		b = false;
		sProblems.append("No control monitor specified. Check preferences.");
	}

	// Check stimulus layout type, then check preferences for monitor assignments.
	if (layoutType == HStimulusLayoutType::HStimulusLayoutUnknown)
	{
		b = false;
		sProblems.append("Stimulus layout not set. Check preferences.");
	}
	else if (layoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		// Must have a setting for the center stim.
		if (iCenter < 0)
		{
			b = false;
			sProblems.append("Stimulus layout type is \"single\". Please specify \"Center Monitor\" in preferences.");
		}
		else if (iCenter == iControl)
		{
			b = false;
			sProblems.append("Center and Control monitors are the same! Check preferences.");
		}
	}
	else if (layoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		if (iLeft < 0)
		{
			b = false;
			sProblems.append("Stimulus layout type is \"left/right\". Please specify \"Left Monitor\" in preferences.");
		}
		else if (iLeft == iControl)
		{
			b = false;
			sProblems.append("Left and Control monitors are the same! Check preferences.");
		}
		if (iRight < 0)
		{
			b = false;
			sProblems.append("Stimulus layout type is \"left/right\". Please specify \"Right Monitor\" in preferences.");
		}
		else if (iRight == iControl)
		{
			b = false;
			sProblems.append("Right and Control monitors are the same! Check preferences.");
		}
		if (iRight == iLeft)
		{
			b = false;
			sProblems.append("Left and Right monitors are the same. Check preferences.");
		}
	}

	// check pretest stimuli
	if (settings.getPreTestPhaseSettings().getIsEnabled())
	{
		Habit::StimuliSettings stimuli = settings.getPreTestStimuliSettings();
		QListIterator<Habit::StimulusSettings> it(stimuli.stimuli());
		while (it.hasNext())
		{
			Habit::StimulusSettings ss = it.next();
			if (!habutilStimulusFilesFound(ss, layoutType))
			{
				b = false;
				sProblems.append(QString("PreTest stimulus \"%1\" file(s) not found.\n").arg(ss.getName()));
			}
		}
	}

	// check habituation stimuli
	if (settings.getHabituationPhaseSettings().getIsEnabled())
	{
		Habit::StimuliSettings stimuli = settings.getHabituationStimuliSettings();
		QListIterator<Habit::StimulusSettings> it(stimuli.stimuli());
		while (it.hasNext())
		{
			Habit::StimulusSettings ss = it.next();
			if (!habutilStimulusFilesFound(ss, layoutType))
			{
				b = false;
				sProblems.append(QString("Habituation stimulus \"%1\" file(s) not found.\n").arg(ss.getName()));
			}
		}
	}

	// check test stimuli
	if (settings.getTestPhaseSettings().getIsEnabled())
	{
		Habit::StimuliSettings stimuli = settings.getTestStimuliSettings();
		QListIterator<Habit::StimulusSettings> it(stimuli.stimuli());
		while (it.hasNext())
		{
			Habit::StimulusSettings ss = it.next();
			if (!habutilStimulusFilesFound(ss, layoutType))
			{
				b = false;
				sProblems.append(QString("Test stimulus \"%1\" file(s) not found.\n").arg(ss.getName()));
			}
		}
	}

	return b;
}



void GUILib::H2MainWindow::runExperiment()
{
	QString expt = m_pExperimentListWidget->selectedExperiment();	// the experiment to run
	if (expt.isEmpty())
	{
		qCritical() << "No experiment selected! RUN not allowed when no selection made - check HExperimentListWidget and H2MainWindow";
	}
	else
	{
		Habit::ExperimentSettings settings;
		if (!Habit::ExperimentSettings::load(settings, expt))
		{
			QMessageBox::critical(this, "Cannot load experiment", "Cannot load experiment from database!");
			qCritical() << "Cannot load experiment \"" << expt << "\" from database.";
			return;
		}

		// pre-flight check. Verify that monitors have been configured, etc.
		QStringList sProblems;
		if (!H2MainWindow::checkExperimentSettings(settings, sProblems))
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

		GUILib::HRunSettingsDialog dlg(settings, m_bTestRunDefault, this);
		int i = dlg.exec();
		if (i == QDialog::Accepted)
		{
			HEventLog log;
			HControlPanel habitControlPanel(settings, log, dlg.getRunSettings(), this);
			// set dialog title
			habitControlPanel.setWindowTitle(dlg.getRunLabel());
			if (habitControlPanel.exec() != QDialog::Accepted)
				return;
			HResults* results = new HResults(settings, dlg.getRunSettings(),
					dlg.getSubjectSettings(), log,
					dlg.getRunLabel(),
					QCoreApplication::instance()->applicationVersion());

			// display results
			HResultsDialog dialog(*results, this);
			dialog.exec();

			// Always save results. No option here.
			//QDir dir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
			QDir dir (habutilGetResultsDir(expt));
			QString filename(dir.absoluteFilePath(QString("%1.hab").arg(dlg.getRunLabel())));
			if (!results->save(filename))
			{
				qCritical() << "Error - cannot save data to file " << filename;
			}
			QString filenameCSV(dir.absoluteFilePath(QString("%1.csv").arg(dlg.getRunLabel())));
			if (!results->saveToCSV(filenameCSV))
			{
				qCritical() << "Error - cannot save data to csv file " << filenameCSV;
			}
		}
	}
}




void GUILib::H2MainWindow::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected);

	// if selected has anything, enable buttons, otherwise disable.
	if (selected.count() > 0)
	{
		// disable edit, clone buttons
		m_actionEdit->setEnabled(true);
		m_actionDelete->setEnabled(true);
		m_actionClone->setEnabled(true);
	}
	else
	{
		// disable edit, clone buttons
		m_actionEdit->setEnabled(false);
		m_actionDelete->setEnabled(false);
		m_actionClone->setEnabled(false);
	}
}

void GUILib::H2MainWindow::cloneExperiment()
{
	QString sName, sDefault;

	// Create default experiment name for the lazy
	sDefault = getExperimentCopyName(m_pExperimentListWidget->selectedExperiment());

	if (inputExperimentName(sName, sDefault))
	{
		Habit::ExperimentSettings settings;
		if (Habit::ExperimentSettings::load(settings, m_pExperimentListWidget->selectedExperiment()))
		{
			Habit::ExperimentSettings cloned = settings.clone(sName);
			HExperimentMain *exptMain = new HExperimentMain(cloned, this);
			if (exptMain->exec() == QDialog::Accepted)
			{
				m_pExperimentListWidget->reload();
			}
		}
	}
}

void GUILib::H2MainWindow::deleteExperiment()
{
	QString expt = m_pExperimentListWidget->selectedExperiment();
	if (QMessageBox::question(this, "Delete Experiment", QString("Are you sure you want to delete experiment \"%1\"?").arg(expt),
			QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Yes)
	{
		Habit::ExperimentSettings settings;
		if (Habit::ExperimentSettings::load(settings, expt))
		{
			if (settings.deleteFromDB())
			{
				qDebug() << "Experiment " << expt << " deleted from database. Results files were NOT deleted.";
				m_pExperimentListWidget->reload();
			}
			else
			{
				QMessageBox::warning(this, "Delete Experiment", "There was an error deleting the experiment, check log file.");
			}
		}
	}
}


void GUILib::H2MainWindow::editPreferences()
{
	// Open preferences dialog
	HGlobalPreferencesDialog *pPrefDialog = new HGlobalPreferencesDialog(this);
	connect(pPrefDialog, SIGNAL(workspaceChanged()), m_pExperimentListWidget, SLOT(reload()));
	if (pPrefDialog->exec() == QDialog::Accepted)
	{
		qDebug() << "Preferences updated";
	}
	disconnect(pPrefDialog, SIGNAL(workspaceChanged()), m_pExperimentListWidget, SLOT(reload()));
}
