#include "habitapp.h"
#include "experimentsettingsform.h"
#include "resultsvieweroptions.h"
#include "experimentresultsform.h"
#include "runsettingsform.h"
#include "experimentsettings.h"
#include "habitcontrolpanel.h"
#include "outputgenerator.h"
#include "exportmultipleresults.h"
#include "viewexperimentresultform.h"
#include "reliabilitiesform.h"
#include "reliabilitysettings.h"
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtGui/QApplication>

#if _DONT_USE_IN_HABIT_2_0

HabitApp::HabitApp(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);
	setWindowTitle(QString("%1 - %2").arg("Habit").arg(QCoreApplication::instance()->applicationVersion()));
    //ui.menuBar->addMenu(tr("File"));
    //ui.menuBar->addMenu(tr("Edit"));

  /*  QMenu* experimentMenu =  ui.menuBar->addMenu(tr("Experiment"));
    QAction* newExperimentAction = experimentMenu->addAction(tr("Experiments editor"));
    newExperimentAction->setShortcuts(QKeySequence::New);
    newExperimentAction->setStatusTip(tr("Create new experiment"));
    connect(newExperimentAction, SIGNAL(triggered()), this, SLOT(newExperiment()));

    QMenu* runExperimentMenu = ui.menuBar->addMenu(tr("Run"));
	QAction* runSavedExperimentAction = runExperimentMenu->addAction(tr("Run Saved Experiment"));
    runSavedExperimentAction->setStatusTip(tr("Run saved experiment"));
    connect(runSavedExperimentAction, SIGNAL(triggered()), this, SLOT(runSavedExperiment()));

    //ui.menuBar->addMenu(tr("Trials"));

    QMenu* runResultsMenu = ui.menuBar->addMenu(tr("Results"));
	QAction* runExperimentResultsAction = runResultsMenu->addAction(tr("Open Data File..."));
	runExperimentResultsAction->setStatusTip(tr(""));
    connect(runExperimentResultsAction, SIGNAL(triggered()), this, SLOT(runExperimentResults()));
	QAction* resultsViewerOptionsAction = runResultsMenu->addAction(tr("Results Viewer Options..."));
	resultsViewerOptionsAction->setStatusTip(tr(""));
    connect(resultsViewerOptionsAction, SIGNAL(triggered()), this, SLOT(resultsViewerOptions()));

	resize(300, 50);*/
}

void HabitApp::runReliability()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Experient File"),
								"", tr("Experient Result File (*.res)"));
	if (!fileName.isNull() && !fileName.isEmpty())
	{
		GUILib::ReliabilitiesForm form(this);
		if (form.exec() == QDialog::Accepted)
		{
			Habit::OutputGenerator output;
			output.load(fileName);
			GUILib::HabitControlPanel habitControlPanel(output.getSubjectSettings(), output.getRunSettings(), this);
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
					Habit::OutputGenerator output_ = habitControlPanel.getOutputGenerator();
					output_.setSubjectInformation(output.getSubjectSettings());
					output_.setRunSettings(output.getRunSettings());
					output_.setSettingsFileName("Typed");
					output_.setResultsFileName(QFileInfo(fileName).fileName());
					output_.setResultsType("RELIABILITY RUN");
					output_.setReliabilitiesSettings(reliabilitySettings);
					
					output_.save(fileName);
					GUILib::ExperimentResultsForm experForm(output_.getSubjectSettings(), output_.getRunSettings(),
						output_, this);
					experForm.exec();
				}
			}
		}
	}
}

void HabitApp::newExperiment()
{
    Habit::ExperimentSettings experimentSettings;
    GUILib::ExperimentSettingsForm experimentSettingsForm(experimentSettings, this);
    if(experimentSettingsForm.exec() == 1) {
        
    }
}

void HabitApp::runExperimentResults()
{
	ViewExperimentResultForm form(this);
	form.exec();
}

void HabitApp::runSavedExperiment()
{
    //Habit::RunSettings runSettings;
    GUILib::RunSettingsForm runSettingsForm(this/* runSettings*/);
	if(runSettingsForm.exec() == QDialog::Accepted) {
		GUILib::HabitControlPanel habitControlPanel(runSettingsForm.getSubjectSettings(), runSettingsForm.getRunSettings(), this);
		if (habitControlPanel.exec() != QDialog::Accepted)
			return;
		Habit::OutputGenerator output_ = habitControlPanel.getOutputGenerator();
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
				output_.setSettingsFileName("Typed");
				output_.setResultsFileName(QFileInfo(fileName).fileName());
				output_.setResultsType("ORIGINAL RUN");
				output_.save(fileName);
				GUILib::ExperimentResultsForm resultForm(runSettingsForm.getSubjectSettings(), runSettingsForm.getRunSettings(), output_, this);
				resultForm.exec();
			}
		}
    }
}

HabitApp::~HabitApp()
{

}

#endif