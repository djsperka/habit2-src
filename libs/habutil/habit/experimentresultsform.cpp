#include "experimentresultsform.h"
#include "subjectinformation.h"
#include "exportmultipleresults.h"
#include "maindao.h"
#include "resultsvieweroptions.h"
#include "resultviewerform.h"
#include "HOutputGenerator.h"

#ifndef USING_QT5
#include <QtGui>
#else
#include <QtWidgets>
#endif


namespace GUILib {

	ExperimentResultsForm::ExperimentResultsForm(const Habit::SubjectSettings& ss, const Habit::RunSettings& rs, QWidget* w)
	: QDialog(w)
	, subjectSettings_(ss)
	, runSettings_(rs)
{
	createComponents();
	setAlignment();
	makeConnections();
	doLayout();
}

ExperimentResultsForm::~ExperimentResultsForm()
{

}

void ExperimentResultsForm::createComponents()
{
	createSubjectInfoSection();
	createSummeryOfResultsSection();
	okButton_ =  new QPushButton(tr("OK"));
	cancelButton_ =  new QPushButton(tr("Cancel"));
}

void ExperimentResultsForm::createSubjectInfoSection()
{
	subjectInformation_ = new QGroupBox(tr("Subject Information"));
	name_ = new QLabel(tr("Name:"));
	birthDate_ = new QLabel(tr("Birth Date:"));
	testDate_ = new QLabel(tr("Test Date:"));
	observer_ = new QLabel(tr("Observer:"));
	condition_ = new QLabel(tr("Condition:"));
	cellNo_ = new QLabel(tr("Cell No.:"));
	comments_ = new QLabel(tr("Comments:"));

	nameValue_ = new QLabel(subjectSettings_.getSubjectName());
	nameValue_->setFixedWidth(200);
	birthDateValue_ = new QLabel(subjectSettings_.getBirthDate().toString());
	birthDateValue_->setFixedWidth(200);
	testDateValue_ = new QLabel(subjectSettings_.getTestDate().toString());
	testDateValue_->setFixedWidth(200);
	observerValue_ = new QLabel(subjectSettings_.getObserver());
	observerValue_->setFixedWidth(200);
	conditionValue_ = new QLabel(subjectSettings_.getCondition());
	conditionValue_->setFixedWidth(200);
	cellNoValue_ = new QLabel(subjectSettings_.getCellNumber());
	cellNoValue_->setFixedWidth(200);
	commentsValue_ = new QLabel(subjectSettings_.getComments());
	commentsValue_->setWordWrap(false);
	commentsValue_->setFixedHeight(commentsValue_->sizeHint().height());
	commentsValue_->setFixedWidth(200);
	editInfo_ = new QPushButton(tr("Edit Subject Information..."));
	viewDetailedResults_ = new QPushButton(tr("View Detailed Results..."));
	exportResults_ = new QPushButton(tr("Export Results to Tab Delimited Text..."));
	exportMultipleResults_ = new QPushButton(tr("Export Multiple Results...")); 
}

void ExperimentResultsForm::initialize() {
	QFontMetrics fm = nameValue_->fontMetrics();
	nameValue_->setText(fm.elidedText(subjectSettings_.getSubjectName(), Qt::ElideRight, nameValue_->maximumWidth()));
	fm = birthDateValue_->fontMetrics();
	birthDateValue_->setText(fm.elidedText(subjectSettings_.getBirthDate().toString(), Qt::ElideRight, nameValue_->maximumWidth()));
	fm = testDateValue_->fontMetrics();
	testDateValue_->setText(fm.elidedText(subjectSettings_.getTestDate().toString(), Qt::ElideRight, nameValue_->maximumWidth()));
	fm = observerValue_->fontMetrics();
	observerValue_->setText(fm.elidedText(subjectSettings_.getObserver(), Qt::ElideRight, nameValue_->maximumWidth()));
	fm = conditionValue_->fontMetrics();
	conditionValue_->setText(fm.elidedText(subjectSettings_.getCondition(), Qt::ElideRight, nameValue_->maximumWidth()));
	fm = cellNoValue_->fontMetrics();
	cellNoValue_->setText(fm.elidedText(subjectSettings_.getCellNumber(), Qt::ElideRight, nameValue_->maximumWidth()));
	fm = commentsValue_->fontMetrics();
	commentsValue_->setText(fm.elidedText(subjectSettings_.getComments(), Qt::ElideRight, nameValue_->maximumWidth()));
}

void ExperimentResultsForm::createSummeryOfResultsSection()
{
	summaryOfResults_ = new QGroupBox(tr("Summary Of Results"));
	experimentName_ = new QLabel(tr("Experiment Name:"));
	settingsFile_ = new QLabel(tr("Settings File:"));
	resultsFile_ = new QLabel(tr("Results File:"));
	runType_ = new QLabel(tr("Run Type:"));
	trialType_ = new QLabel(tr("Trial Type:"));
	habituation_ = new QLabel(tr("Habituation:"));
	habituationResult_ = new QLabel(tr("Habituation Result:"));

//	experimentNameValue_ = new QLabel(HOutputGenerator::instance()->getExperimentName());
//	settingsFileValue_ = new QLabel(HOutputGenerator::instance()->getSettingsFileName());
//	resultsFileValue_ = new QLabel(HOutputGenerator::instance()->getResultsFileName());
//	runTypeValue_ = new QLabel(HOutputGenerator::instance()->getResultsType());
//	trialTypeValue_ = new QLabel(HOutputGenerator::instance()->getTrialType());
//	habituationValue_ = new QLabel(HOutputGenerator::instance()->getHabituation());
//	habituationResultValue_ = new QLabel(HOutputGenerator::instance()->getHabituationResult());
}

void ExperimentResultsForm::makeConnections()
{
	connect(editInfo_, SIGNAL(clicked()),this, SLOT(editSubjectInfo()));
	connect(exportMultipleResults_, SIGNAL(clicked()),this, SLOT(exportMultipleResults()));
	connect(okButton_, SIGNAL(clicked()),this, SLOT(onOk()));
	connect(cancelButton_, SIGNAL(clicked()),this, SLOT(close()));
	connect(viewDetailedResults_, SIGNAL(clicked()), this, SLOT(onViewDetailedResults()));
}

void ExperimentResultsForm::onViewDetailedResults()
{
	GUILib::ResultsViewerOptions dlg(this);
	if (dlg.exec() == QDialog::Accepted)
	{
		Habit::ResultViewerSettings settings = dlg.getConfigurationObject();
		QStringList allResult;
		allResult.append("");
//		HOutputGenerator::instance()->setResultViewerSettings(settings);
//		QStringList list = HOutputGenerator::instance()->toHtml();
		
		qWarning("Experiment results not implemented!");
		QStringList list;
		allResult.append(list);
		allResult.append("");

		ResultViewerForm form;
		form.setLogText(allResult);
		form.exec();
	}
}

void ExperimentResultsForm::exportMultipleResults() 
{
	GUILib::ExportMultipleResults exportMultipleResults(this);
	if(exportMultipleResults.exec() == 1) {
	}
}

void ExperimentResultsForm::editSubjectInfo() 
{
	GUILib::SubjectInformation subjectInfo(subjectSettings_, this);
	subjectInfo.exec();
	subjectSettings_ = subjectInfo.getConfigurationObject();
	initialize();
}

void ExperimentResultsForm::doLayout()
{
	mainLayout_ = new QVBoxLayout(this);
	QHBoxLayout* hbox = new QHBoxLayout();
	mainLayout_->addLayout(hbox);
	QVBoxLayout* vbox = new QVBoxLayout();
	vbox->addWidget(subjectInformation_);
	vbox->addWidget(viewDetailedResults_);
	vbox->addWidget(exportResults_);
	vbox->addWidget(exportMultipleResults_);
	hbox->addLayout(vbox);
	hbox->addWidget(summaryOfResults_);
	doSubjectInfoLayout();
	doSummaryOfResultsLayout();
	doButtonsLayout();
}

void ExperimentResultsForm::doSubjectInfoLayout()
{
	QVBoxLayout* buttonVbox = new QVBoxLayout(subjectInformation_);
	QFormLayout* hbox = new QFormLayout();
	
	hbox->addRow(name_, nameValue_);
	hbox->addRow(birthDate_, birthDateValue_);
	hbox->addRow(testDate_, testDateValue_);
	hbox->addRow(observer_, observerValue_);
	hbox->addRow(condition_, conditionValue_);
	hbox->addRow(cellNo_, cellNoValue_);
	hbox->addRow(comments_, commentsValue_);

	hbox->setSizeConstraint(QLayout::SetMaximumSize);
	buttonVbox->addLayout(hbox);
	buttonVbox->addSpacing(10);
	buttonVbox->addWidget(editInfo_);
	buttonVbox->setAlignment(editInfo_, Qt::AlignRight);
}

void ExperimentResultsForm::setAlignmentToSubjectInfo()
{
	name_->setAlignment(Qt::AlignRight);
	birthDate_->setAlignment(Qt::AlignRight);
	testDate_->setAlignment(Qt::AlignRight);
	observer_->setAlignment(Qt::AlignRight);
	condition_->setAlignment(Qt::AlignRight);
	cellNo_->setAlignment(Qt::AlignRight);
	comments_->setAlignment(Qt::AlignRight);

	nameValue_->setAlignment(Qt::AlignLeft);
	birthDateValue_->setAlignment(Qt::AlignLeft);
	testDateValue_->setAlignment(Qt::AlignLeft);
	observerValue_->setAlignment(Qt::AlignLeft);
	conditionValue_->setAlignment(Qt::AlignLeft);
	cellNoValue_->setAlignment(Qt::AlignLeft);
	commentsValue_->setAlignment(Qt::AlignLeft);
}

void ExperimentResultsForm::setAlignmentToSummaryResults()
{
	experimentName_->setAlignment(Qt::AlignRight);
	settingsFile_->setAlignment(Qt::AlignRight);
	resultsFile_->setAlignment(Qt::AlignRight);
	runType_->setAlignment(Qt::AlignRight); 
	trialType_->setAlignment(Qt::AlignRight);
	habituation_->setAlignment(Qt::AlignRight); 
	habituationResult_->setAlignment(Qt::AlignRight);

	experimentNameValue_->setAlignment(Qt::AlignLeft);
	settingsFileValue_->setAlignment(Qt::AlignLeft);
	resultsFileValue_->setAlignment(Qt::AlignLeft);
	runTypeValue_->setAlignment(Qt::AlignLeft);
	trialTypeValue_->setAlignment(Qt::AlignLeft);
	habituationValue_->setAlignment(Qt::AlignLeft);
	habituationResultValue_->setAlignment(Qt::AlignLeft);
}

void ExperimentResultsForm::setAlignment()
{
	setAlignmentToSubjectInfo();
	setAlignmentToSummaryResults();
}

void ExperimentResultsForm::doSummaryOfResultsLayout()
{
	QVBoxLayout* vbox = new QVBoxLayout(summaryOfResults_);
	QFormLayout* hbox = new QFormLayout();
	//QVBoxLayout* nameVbox = new QVBoxLayout();
	//QVBoxLayout* valueVBox = new QVBoxLayout();
	//hbox->addLayout(nameVbox);
	//hbox->addLayout(valueVBox);
	
	hbox->addRow(experimentName_, experimentNameValue_);
	hbox->addRow(settingsFile_, settingsFileValue_);
	hbox->addRow(resultsFile_, resultsFileValue_);
	
	hbox->addRow(runType_, runTypeValue_);
	
	hbox->addRow(trialType_, trialTypeValue_);
	hbox->addRow(habituation_, habituationValue_);
	hbox->addRow(habituationResult_, habituationResultValue_);
	vbox->addLayout(hbox);
	vbox->addStretch();

}

void ExperimentResultsForm::doButtonsLayout()
{
	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(cancelButton_);
	buttonsLayout->addWidget(okButton_);
	mainLayout_->addLayout(buttonsLayout);
}

void ExperimentResultsForm::onOk() {
	Habit::MainDao dao;
	dao.updateSubject(subjectSettings_);
	close();
}

}
