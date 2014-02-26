/*
 *  HMainWindow.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HMainWindow.h"
#include "HEventLog.h"
#include "experimentsettingsform.h"
#include "resultsvieweroptions.h"
#include "experimentresultsform.h"
#include "runsettingsform.h"
#include "experimentsettings.h"
#include "HControlPanel.h"
#include "exportmultipleresults.h"
#include "viewexperimentresultform.h"
#include "reliabilitiesform.h"
#include "reliabilitysettings.h"
#include "HResultsDialog.h"
#include "HResultsWidget.h"
#include "HDBUtil.h"
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtGui/QApplication>
#include <QMenu>
#include <QAction>
#include <QDir>

using namespace Habit;

HMainWindow::HMainWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
    ui.setupUi(this);
	setWindowTitle(QString("%1 - %2").arg("Habit").arg(QCoreApplication::instance()->applicationVersion()));
	createActions();
	createMenus();
}

void HMainWindow::createActions()
 {
	m_openDBAct = new QAction(tr("&Open Experiment Database File..."), this);
	m_openDBAct->setShortcuts(QKeySequence::Open);
	m_openDBAct->setStatusTip(tr("Open or create another experiment database"));
	connect(m_openDBAct, SIGNAL(triggered()), this, SLOT(openDBFile()));
 }

void HMainWindow::openDBFile()
{
	if (selectDB())
	{
		openDB();
	}
}

void HMainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_openDBAct);
}


void HMainWindow::runReliability()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Results File"),
													"", tr("Habit Results File (*.hab)"));
	if (!fileName.isNull() && !fileName.isEmpty())
	{
		GUILib::ReliabilitiesForm form(this);
		if (form.exec() == QDialog::Accepted)
		{
			HEventLog log;
			
			qWarning("RELIABILITY IS BROKEN AND, WELL, UNRELIABLE!");
			RunSettings runSettings;
			SubjectSettings subjectSettings;
			HControlPanel habitControlPanel(log, runSettings, this);
			
			if (habitControlPanel.exec() != QDialog::Accepted)
				return;
			QMessageBox box;
			box.setText("Save the results of this reliability?");
			box.setWindowTitle("End of reliability");
			box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::Yes)
			{
				QString fileName = QFileDialog::getSaveFileName(this, tr("Save Reliability Result"),
																"", tr("Experiment Result File (*.res)"));
				if (!fileName.isNull() && !fileName.isEmpty())
				{
					Habit::ReliabilitySettings reliabilitySettings;
					reliabilitySettings.setObserver(form.getObserver());
					reliabilitySettings.setDate(form.getDate());
					reliabilitySettings.setComment(form.getComment());

					GUILib::ExperimentResultsForm experForm(subjectSettings, runSettings, this);
					experForm.exec();
				}
			}
		}
	}
}

void HMainWindow::newExperiment()
{
    Habit::ExperimentSettings experimentSettings;
    GUILib::ExperimentSettingsForm experimentSettingsForm(experimentSettings, this);
    if(experimentSettingsForm.exec() == 1) {
        
    }
}

void HMainWindow::runExperimentResults()
{
	QString filename = QFileDialog::getOpenFileName(NULL, "Open File", "~/Desktop", "Habit Results Files (*.hab)");
	if (!filename.isNull())
	{
		showResultsFile(filename);
	}
}

void HMainWindow::showResultsFile(const QString filename)
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

void HMainWindow::runSavedExperiment()
{
    GUILib::RunSettingsForm runSettingsForm(this/* runSettings*/);

    m_openDBAct->setEnabled(false);
	if(runSettingsForm.exec() == QDialog::Accepted) {
		HEventLog log;
		HControlPanel habitControlPanel(log, runSettingsForm.getRunSettings(), this);
		// set dialog title
		habitControlPanel.setWindowTitle(runSettingsForm.getRunLabel());
		if (habitControlPanel.exec() != QDialog::Accepted)
			return;
		HResults* results = new HResults(habitControlPanel.getExperimentSettings(), runSettingsForm.getRunSettings(),
				runSettingsForm.getSubjectSettings(), log, QCoreApplication::instance()->applicationVersion());

		// display results
		HResultsDialog dialog(*results, this);
		dialog.exec();


#if OPTIONAL_FILE_SAVE
		QMessageBox box;
		box.setText("Save the results of this experiment?");
		box.setWindowTitle("End of experiment");
		box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		if (box.exec() == QMessageBox::Yes)
		{
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save Experiment Result"),
															"", tr("Experiment Result File (*.hab)"));
			if (!fileName.isNull() && !fileName.isEmpty())
			{
				if (!results->save(fileName))
				{
					qCritical() << "Error - cannot save data to file " << fileName;
				}
			}
		}
#else
		// Always save results. No option here.
		QDir dir(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
		QString filename(dir.absoluteFilePath(QString("%1.hab").arg(runSettingsForm.getRunLabel())));
		if (!results->save(filename))
		{
			qCritical() << "Error - cannot save data to file " << filename;
		}
		QString filenameCSV(dir.absoluteFilePath(QString("%1.csv").arg(runSettingsForm.getRunLabel())));
		if (!results->saveToCSV(filenameCSV))
		{
			qCritical() << "Error - cannot save data to csv file " << filenameCSV;
		}

#endif
    }
    m_openDBAct->setEnabled(true);
}

