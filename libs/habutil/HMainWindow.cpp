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
#include <QMessageBox>
#include <QFileDialog>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QApplication>

using namespace Habit;

HMainWindow::HMainWindow(QWidget *parent, Qt::WindowFlags flags)
: QMainWindow(parent, flags)
{
    ui.setupUi(this);
	setWindowTitle(QString("%1 - %2").arg("Habit").arg(QCoreApplication::instance()->applicationVersion()));
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

#if 0
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
#endif
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

	if(runSettingsForm.exec() == QDialog::Accepted) {
		HEventLog log;
		HControlPanel habitControlPanel(log, runSettingsForm.getRunSettings(), this);
		if (habitControlPanel.exec() != QDialog::Accepted)
			return;
		HResults* results = new HResults(habitControlPanel.getExperimentSettings(), runSettingsForm.getRunSettings(),
				runSettingsForm.getSubjectSettings(), log, QCoreApplication::instance()->applicationVersion());

		// display results
		HResultsDialog dialog(*results, this);
		dialog.exec();

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
    }
}

