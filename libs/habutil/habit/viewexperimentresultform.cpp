#include "viewexperimentresultform.h"
#include "resultsvieweroptions.h"
#include "resultviewerform.h"
#include "exportmultipleresults.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

ViewExperimentResultForm::ViewExperimentResultForm(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

ViewExperimentResultForm::~ViewExperimentResultForm()
{

}

void ViewExperimentResultForm::onViewExperimentResults()
{
	QMessageBox msgBox;
	msgBox.setText("View Experiment Results not implemented.");
	msgBox.exec();
}

void ViewExperimentResultForm::onExportMultipleResults()
{
	QMessageBox msgBox;
	msgBox.setText("Export Multiple Results not implemented.");
	msgBox.exec();
}
