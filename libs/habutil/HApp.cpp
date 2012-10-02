/*
 *  HApp.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HApp.h"

#include "experimentsettingsform.h"
#include "resultsvieweroptions.h"
#include "experimentresultsform.h"
#include "runsettingsform.h"
#include "experimentsettings.h"
#include "HControlPanel.h"
#include "HOutputGenerator.h"
#include "exportmultipleresults.h"
#include "viewexperimentresultform.h"
#include "reliabilitiesform.h"
#include "reliabilitysettings.h"
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtGui/QApplication>

using namespace Habit;

HApp::HApp(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
    ui.setupUi(this);
	setWindowTitle(QString("%1 - %2").arg("Habit").arg(QCoreApplication::instance()->applicationVersion()));
}

void HApp::runReliability()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Experient File"),
													"", tr("Experient Result File (*.res)"));
	if (!fileName.isNull() && !fileName.isEmpty())
	{
		GUILib::ReliabilitiesForm form(this);
		if (form.exec() == QDialog::Accepted)
		{
			HOutputGenerator::initialize();
			HOutputGenerator::instance()->load(fileName);
			RunSettings runSettings = HOutputGenerator::instance()->getRunSettings();
			SubjectSettings subjectSettings = HOutputGenerator::instance()->getSubjectSettings();
			HControlPanel habitControlPanel(subjectSettings, runSettings, this);
			
			if (habitControlPanel.exec() != QDialog::Accepted)
				return;
			QMessageBox box;
			box.setText("Save the results of this reliability?");
			box.setWindowTitle("End of reliability");
			box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			if (box.exec() == QMessageBox::Yes)
			{
				QString fileName = QFileDialog::getSaveFileName(this, tr("Save Reliability Result"),
																"", tr("Experient Result File (*.res)"));
				if (!fileName.isNull() && !fileName.isEmpty())
				{
					Habit::ReliabilitySettings reliabilitySettings;
					reliabilitySettings.setObserver(form.getObserver());
					reliabilitySettings.setDate(form.getDate());
					reliabilitySettings.setComment(form.getComment());
					HOutputGenerator::instance()->setSettingsFileName("Typed");
					HOutputGenerator::instance()->setResultsFileName(QFileInfo(fileName).fileName());
					HOutputGenerator::instance()->setResultsType("RELIABILITY RUN");
					HOutputGenerator::instance()->setReliabilitiesSettings(reliabilitySettings);
					
					HOutputGenerator::instance()->save(fileName);
					GUILib::ExperimentResultsForm experForm(subjectSettings, runSettings, this);
					experForm.exec();
				}
			}
		}
	}
}

void HApp::newExperiment()
{
    Habit::ExperimentSettings experimentSettings;
    GUILib::ExperimentSettingsForm experimentSettingsForm(experimentSettings, this);
    if(experimentSettingsForm.exec() == 1) {
        
    }
}

void HApp::runExperimentResults()
{
	ViewExperimentResultForm form(this);
	form.exec();
}

void HApp::runSavedExperiment()
{
    GUILib::RunSettingsForm runSettingsForm(this/* runSettings*/);
	
	// Initialize output generator
	HOutputGenerator::initialize();
	if(runSettingsForm.exec() == QDialog::Accepted) {
		HControlPanel habitControlPanel(runSettingsForm.getSubjectSettings(), runSettingsForm.getRunSettings(), this);
		if (habitControlPanel.exec() != QDialog::Accepted)
			return;
		QMessageBox box;
		box.setText("Save the results of this experiment?");
		box.setWindowTitle("End of experiment");
		box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		if (box.exec() == QMessageBox::Yes)
		{
			QString fileName = QFileDialog::getSaveFileName(this, tr("Save Experient Result"),
															"", tr("Experient Result File (*.res)"));
			if (!fileName.isNull() && !fileName.isEmpty())
			{
				HOutputGenerator::instance()->setSettingsFileName("Typed");
				HOutputGenerator::instance()->setResultsFileName(QFileInfo(fileName).fileName());
				HOutputGenerator::instance()->setResultsType("ORIGINAL RUN");
				HOutputGenerator::instance()->save(fileName);
				GUILib::ExperimentResultsForm resultForm(runSettingsForm.getSubjectSettings(), runSettingsForm.getRunSettings(), this);
				resultForm.exec();
			}
		}
    }
}

HApp::~HApp()
{
	
}
