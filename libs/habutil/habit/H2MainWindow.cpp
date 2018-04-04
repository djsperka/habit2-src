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
#include <QListIterator>
#include <QtGlobal>
#if QT_VERSION < 0x050000
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif


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
#include "HMediaManagerUtil.h"
#include "HLookDetectorUtil.h"
#include "HTestingInputWrangler.h"
#include "HDBException.h"
#include "HExperimentNameDialog.h"
#include <hgst/HStimulusWidget.h>

using namespace GUILib;
using namespace Habit;


GUILib::H2MainWindow::H2MainWindow(bool bDefaultTestRun, bool bShowTestingIcon, bool bEditTemplates, bool bStimInDialog)
: QMainWindow()
, m_bTestRunIsDefault(bDefaultTestRun)
, m_bShowTestingIcon(bShowTestingIcon)
, m_bEditTemplates(bEditTemplates)
, m_bStimInDialog(bStimInDialog)
{
	m_pExperimentListWidget = new GUILib::HExperimentListWidget(this, true, m_bEditTemplates);
	setCentralWidget(m_pExperimentListWidget);
    createActions();
    createToolBars();

    m_pLabelStatusBar = new QLabel(QString("Current workspace: %1").arg(habutilGetWorkspaceDir().absolutePath()));
    statusBar()->addWidget(m_pLabelStatusBar);
    setWindowTitle(QString("%1 (v%2)").arg("Habit2").arg(QCoreApplication::instance()->applicationVersion()));

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
	try
	{
		settings.loadFromDB(expt);
		HExperimentMain *exptMain = new HExperimentMain(settings, this);
		exptMain->exec();
		delete exptMain;
	}
	catch (const Habit::HDBException& e)
	{
		QMessageBox::warning(this, "Habit database error", e.what());
		qCritical() << e.what();
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
    m_actionResults->setStatusTip(tr("Open results file viewer"));
    connect(m_actionResults, SIGNAL(triggered()), this, SLOT(openResultsFile()));

    m_actionPreferences = new QAction(QIcon(":/resources/gear_wheel.png"), tr("&Preferences"), this);
    //m_actionRun->setShortcuts(QKeySequence::New);
    m_actionPreferences->setStatusTip(tr("Edit Habit preferences for this computer"));
    connect(m_actionPreferences, SIGNAL(triggered()), this, SLOT(editPreferences()));

    if (m_bShowTestingIcon)
    {
		m_actionTesting = new QAction(QIcon(":/resources/testing.png"), tr("&Testing"), this);
		m_actionTesting->setStatusTip(tr("Load a testing file and test phase settings."));
		connect(m_actionTesting, SIGNAL(triggered()), this, SLOT(testExperiment()));
    }
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
    if (m_bShowTestingIcon)
    {
    	m_pToolBar->addAction(m_actionTesting);
    }
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


#if 0
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
#endif

void GUILib::H2MainWindow::newExperiment()
{
	Habit::ExperimentSettings settings;
	QString sName;
	QString sDefault;	// initial/default name

	// Create default experiment name for the lazy
	sDefault = getExperimentNewName();

	HExperimentNameDialog dlg(m_pExperimentListWidget->experimentList(), sDefault, this);
	if (dlg.exec() == QDialog::Accepted)
	{
		// Using a template?
		if (dlg.isTemplateChosen())
		{
			Habit::ExperimentSettings templateSettings;
			templateSettings.loadFromDB(dlg.getTemplateChosen());
			settings = templateSettings.clone(dlg.getNewValue());
		}
		else
		{
			settings.setName(dlg.getNewValue());
			// Make sure to have a decent stimulus layout
			settings.getStimulusDisplayInfo();
		}
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
		try
		{
			settings.loadFromDB(expt);
			// Show experiment main window
			HExperimentMain *exptMain = new HExperimentMain(settings, this);
			exptMain->exec();
			delete exptMain;
		}
		catch (const Habit::HDBException& e)
		{
			QMessageBox::warning(this, "Habit database error", e.what());
			qCritical() << e.what();
		}
	}
}

void GUILib::H2MainWindow::importExperiment()
{
	QString filename = QFileDialog::getOpenFileName(NULL, "Import experiment file", habutilGetWorkspaceDir().absolutePath(), "Habit Export Files (*.hbx)");
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






bool GUILib::H2MainWindow::checkExperimentSettings(const Habit::ExperimentSettings& settings, QStringList& sProblems, bool bCheckMonitors)
{
	bool b = true;
	sProblems.clear();
	int iControl = habutilGetMonitorID(HPlayerPositionType::Control);
	int iLeft = habutilGetMonitorID(HPlayerPositionType::Left);
	int iCenter = habutilGetMonitorID(HPlayerPositionType::Center);
	int iRight = habutilGetMonitorID(HPlayerPositionType::Right);
	const HStimulusLayoutType& layoutType = settings.getStimulusDisplayInfo().getStimulusLayoutType();

	if (bCheckMonitors)
	{
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
	}

	// iterate over phases that are enabled
	QListIterator<Habit::HPhaseSettings> iterator = settings.phaseIterator();
	while (iterator.hasNext())
	{
		const Habit::HPhaseSettings& ps = iterator.next();
		if (ps.getIsEnabled())
		{
			QListIterator<Habit::StimulusSettings> it(ps.stimuli().stimuli());
			while (it.hasNext())
			{
				const StimulusSettings& ss = it.next();
				if (!habutilStimulusFilesFound(ss, layoutType))
				{
					b = false;
					sProblems.append(QString("%1 stimulus \"%2\" file(s) not found.\n").arg(ps.getName()).arg(ss.getName()));
				}
			}
		}
	}
	return b;
}

void GUILib::H2MainWindow::testExperiment()
{
	run(true);
}

void GUILib::H2MainWindow::runExperiment()
{
	run(false);
}

void GUILib::H2MainWindow::run(bool bTestInput)
{
	Habit::ExperimentSettings experimentSettings;
	HEventLog eventLog;
	HGMM *pMediaManager = NULL;
	bool bStimInDialog = false;	// can be set on command line, or in RunSettingsDialog
	QString expt = m_pExperimentListWidget->selectedExperiment();	// the experiment to run
	if (expt.isEmpty())
	{
		qCritical() << "No experiment selected! RUN not allowed when no selection made - check HExperimentListWidget and H2MainWindow";
	}
	else
	{
		try
		{
			experimentSettings.loadFromDB(m_pExperimentListWidget->selectedExperiment());
		}
		catch (const Habit::HDBException& e)
		{
			QMessageBox::critical(this, "Cannot load experiment", "Cannot load experiment from database!");
			qCritical() << "Cannot load experiment \"" << m_pExperimentListWidget->selectedExperiment() << "\" from database.";
			qCritical() << e.what();
			return;
		}

		// pre-flight check. Verify that monitors have been configured, etc.
		QStringList sProblems;

		// HACK false here is 'checkMonitors' - so the check skips looking at monitor preferences.
		// see OTHER HACK below
		if (!H2MainWindow::checkExperimentSettings(experimentSettings, sProblems, false))
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

		// m_pRunSettingsDialog has no parent -- DELETE
		m_pRunSettingsDialog = new GUILib::HRunSettingsDialog(experimentSettings, m_bTestRunIsDefault, NULL);
		int i = m_pRunSettingsDialog->exec();
		if (i == QDialog::Accepted)
		{
			bStimInDialog = m_bStimInDialog || m_pRunSettingsDialog->isDisplayStimInWindow();
			if (bStimInDialog)
				pMediaManager = createMediaManager(experimentSettings, 320, 240);
			else
				pMediaManager = createMediaManager(experimentSettings);

			// m_ControlPanel has no parent -- DELETE
			m_pControlPanel = new HControlPanel(experimentSettings, eventLog, m_pRunSettingsDialog->getRunSettings(), pMediaManager, NULL);

			// m_pld has no parent -- DELETE BEFORE m_pControlPanel
			m_pld = createLookDetector(experimentSettings, eventLog, m_pControlPanel);

			// state machine has no parent -- DELETE
			m_psm = createExperiment(this, m_pRunSettingsDialog->getRunSettings(), experimentSettings, m_pld, pMediaManager, eventLog, bTestInput);

			// set state machine and dialog title
			m_pControlPanel->setStateMachine(m_psm);
			m_pControlPanel->setWindowTitle(m_pRunSettingsDialog->getRunLabel());


			// If there is a testing input file (which simulates mouse clicks for look/look away), load it now.
			HTestingInputWrangler *pWrangler;
			if (bTestInput)
			{
				pWrangler = new HTestingInputWrangler();
				pWrangler->enable(m_pld, &m_psm->experiment());
				// Get input file

#if QT_VERSION >= 0x050000
				QString selectedFileName = QFileDialog::getOpenFileName(NULL, "Select LD testing input file", QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0], "(*.txt)");
#else
				QString selectedFileName = QFileDialog::getOpenFileName(NULL, "Select LD testing input file", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "(*.txt)");
#endif

				qDebug() << "Selected input file " << selectedFileName;
				QFile file(selectedFileName);
				if (!pWrangler->load(file))
				{
					QMessageBox::critical(this, "Cannot load input file", "Cannot load testing input file.");
					return;
				}
			}

			QDialog *pStimulusDisplayDialog  = NULL;
			if (bStimInDialog)
			{
				pStimulusDisplayDialog = pMediaManager->createStimulusWidget();
				qDebug() << "stim display dialog min " << pStimulusDisplayDialog->minimumWidth() << "x" << pStimulusDisplayDialog->minimumHeight();
				//pStimulusDisplayDialog->setGeometry(0, 0, pStimulusDisplayDialog->minimumWidth(), pStimulusDisplayDialog->minimumHeight());
				pStimulusDisplayDialog->show();
			}
			else
			{
				adaptVideoWidgets(pMediaManager);
			}

			// This is where the experiment is actually run.
			int cpStatus = m_pControlPanel->exec();

			// delete the video widgets.
			// TODO: This should be guarded against cases where the widgets are owned by something else -
			// like when they are contained in another widget -
			// in which case they will be deleted when that thing is deleted.

			qDebug() << "pMediaManager->stop()";
			pMediaManager->stop();
			if (pStimulusDisplayDialog)
				delete pStimulusDisplayDialog;
			else
			{
				qDebug() << " delete widgets";
				// must explicitly delete the stim widgets
				HStimulusWidget *w;
				w = pMediaManager->getHStimulusWidget(HPlayerPositionType::Center);
				if (w) delete w;
				w = pMediaManager->getHStimulusWidget(HPlayerPositionType::Left);
				if (w) delete w;
				w = pMediaManager->getHStimulusWidget(HPlayerPositionType::Right);
				if (w) delete w;
				qDebug() << " delete widgets done";
			}


			if (cpStatus == QDialog::Accepted)
			{
				HResults* results = new HResults(experimentSettings, m_pRunSettingsDialog->getRunSettings(),
						m_pRunSettingsDialog->getSubjectSettings(), eventLog,
						m_pRunSettingsDialog->getRunLabel(),
						QCoreApplication::instance()->applicationVersion());

				// Always save results. No option here.
				qDebug() << "save results";
				QDir dir (habutilGetResultsDir(m_pExperimentListWidget->selectedExperiment()));
				QString filename(dir.absoluteFilePath(QString("%1.hab").arg(m_pRunSettingsDialog->getRunLabel())));
				qDebug() << "Saving results to " << filename;
				if (!results->save(filename))
				{
					qCritical() << "Error - cannot save data to file " << filename;
				}
				QString filenameCSV(dir.absoluteFilePath(QString("%1.csv").arg(m_pRunSettingsDialog->getRunLabel())));
				if (!results->saveToCSV(filenameCSV))
				{
					qCritical() << "Error - cannot save data to csv file " << filenameCSV;
				}

				// reset the mm
				HGMM::instance().reset();

				// display results
				HResultsDialog dialog(*results, this);
				dialog.exec();

				delete results;

				// clean up stuff
				qDebug() << "Cleaning up...";
				delete m_psm;
				delete m_pld;
				delete m_pControlPanel;
				//delete pMediaManager;
				delete m_pRunSettingsDialog;
				qDebug() << "Cleaning up...done";
			}
		}
	}
}

// moved to HGMM
//
//QDialog* GUILib::H2MainWindow::createStimulusWidget(HGMM *pmm)
//{
//	QDialog *pDialog = new QDialog;
//	QHBoxLayout *hbox = new QHBoxLayout;
//	qDebug() << "createStimulusWidget";
//	if (pmm->getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
//	{
//		qDebug() << "createStimulusWidget - single";
//		HStimulusWidget *video = pmm->getHStimulusWidget(HPlayerPositionType::Center);
//		connect(pmm, SIGNAL(stimulusChanged()), video->getHVideoWidget(), SLOT(stimulusChanged()));
//		hbox->addWidget(video);
//		pDialog->setLayout(hbox);
//		pDialog->setMinimumSize(320, 240);
//	}
//	else if (pmm->getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
//	{
//		HStimulusWidget *videoLeft = pmm->getHStimulusWidget(HPlayerPositionType::Left);
//		HStimulusWidget *videoRight = pmm->getHStimulusWidget(HPlayerPositionType::Right);
//		connect(pmm, SIGNAL(stimulusChanged()), videoLeft->getHVideoWidget(), SLOT(stimulusChanged()));
//		connect(pmm, SIGNAL(stimulusChanged()), videoRight->getHVideoWidget(), SLOT(stimulusChanged()));
//		hbox->addWidget(videoLeft);
//		hbox->addWidget(videoRight);
//		pDialog->setLayout(hbox);
//		qDebug() << "createStimulusWidget - L/R";
//		//pDialog->setMinimumSize(640, 240);	// double wide - can do better than this
//	}
//	return pDialog;
//}


void GUILib::H2MainWindow::adaptVideoWidgets(HGMM *pmm)
{
	if (pmm->getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
#if 0
		HStimulusWidget *player = pmm->getHStimulusWidget(HPlayerPositionType::Center);
		QRect rect = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Center));
		player->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		player->move(rect.x(), rect.y());
		player->setGeometry(rect);
		player->showFullScreen();
		qDebug() << "Player index " << habutilGetMonitorID(HPlayerPositionType::Center) << " moved to rect " << rect;
#endif
		pmm->getHStimulusWidget(HPlayerPositionType::Center)->showFullScreen();
	}
	else if (pmm->getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
#if 0
		HStimulusWidget *playerLeft = pmm->getHStimulusWidget(HPlayerPositionType::Left);
		QRect rectLeft = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Left));
		playerLeft->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		playerLeft->move(rectLeft.x(), rectLeft.y());
		playerLeft->setGeometry(rectLeft);
		playerLeft->showFullScreen();
		qDebug() << "Player index " << habutilGetMonitorID(HPlayerPositionType::Left) << " moved to rect " << rectLeft;

		HStimulusWidget *playerRight = pmm->getHStimulusWidget(HPlayerPositionType::Right);
		QRect rectRight = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Right));
		playerRight->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		playerRight->move(rectRight.x(), rectRight.y());
		playerRight->setGeometry(rectRight);
		playerRight->showFullScreen();
		qDebug() << "Player index " << habutilGetMonitorID(HPlayerPositionType::Right) << " moved to rect " << rectRight;
#endif
		pmm->getHStimulusWidget(HPlayerPositionType::Left)->showFullScreen();
		pmm->getHStimulusWidget(HPlayerPositionType::Right)->showFullScreen();
	}
	return;

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

	HExperimentNameDialog dlg(m_pExperimentListWidget->experimentList(), sDefault, this);
	if (dlg.exec() == QDialog::Accepted)
	{
		Habit::ExperimentSettings settings;
		try
		{
			settings.loadFromDB(m_pExperimentListWidget->selectedExperiment());
			Habit::ExperimentSettings cloned = settings.clone(dlg.getNewValue());
			HExperimentMain *exptMain = new HExperimentMain(cloned, this);
			if (exptMain->exec() == QDialog::Accepted)
			{
				m_pExperimentListWidget->reload();
			}
		}
		catch (const Habit::HDBException& e)
		{
			QMessageBox::critical(this, "Cannot load experiment", "Cannot load experiment from database!");
			qCritical() << "Cannot load experiment \"" << m_pExperimentListWidget->selectedExperiment() << "\" from database.";
			qCritical() << e.what();
			return;
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
		try
		{
			settings.loadFromDB(expt);

			settings.deleteFromDB();
			qDebug() << "Experiment " << expt << " deleted from database. Results files were NOT deleted.";
			m_pExperimentListWidget->reload();
		}
		catch (const HDBException& e)
		{
			// TODO rollback()
			QMessageBox::warning(this, "Delete Experiment", QString(e.what()));
			qCritical() << e.what();
		}
	}
}

void GUILib::H2MainWindow::workspaceChanged()
{
	m_pLabelStatusBar->setText(QString("Current workspace: %1").arg(habutilGetWorkspaceDir().absolutePath()));
	habutilInitWorkspace();
	m_pExperimentListWidget->reload();
}

void GUILib::H2MainWindow::editPreferences()
{
	// Open preferences dialog
	HGlobalPreferencesDialog *pPrefDialog = new HGlobalPreferencesDialog(this);
	connect(pPrefDialog, SIGNAL(workspaceChanged()), this, SLOT(workspaceChanged()));
	if (pPrefDialog->exec() == QDialog::Accepted)
	{
		qDebug() << "Preferences updated";
	}
	disconnect(pPrefDialog, SIGNAL(workspaceChanged()), this, SLOT(workspaceChanged()));
}

