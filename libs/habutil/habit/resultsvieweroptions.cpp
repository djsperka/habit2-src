#include "resultsvieweroptions.h"
#include "maindao.h"

#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

namespace GUILib {

ResultsViewerOptions::ResultsViewerOptions(QWidget* w) 
	: QDialog(w)
{
	createComponents();
	makeConnections();
	doLayout();
	initializeValuesFromDB();
}

ResultsViewerOptions::~ResultsViewerOptions() {
}

void ResultsViewerOptions::createComponents() {
	createExperimentHeaderSection();
	createReliabilityInfoSection();
	createPhasesSection();
	createExperimentSummarySection();
	createTrialInfoSection();
	createControlButtons();
}

void ResultsViewerOptions::createExperimentHeaderSection() {
	subjectName = new QCheckBox(tr("Subject Name"));
	birthDate = new QCheckBox(tr("Subject Birthdate"));
	testDate = new QCheckBox(tr("Test Date"));
	observer = new QCheckBox(tr("Observer"));
	condition = new QCheckBox(tr("Condition"));
	cellNo = new QCheckBox(tr("Cell No."));
	comments = new QCheckBox(tr("Comments"));
}

void ResultsViewerOptions::createReliabilityInfoSection() {
	ongFileName = new QCheckBox(tr("Ong. File Name"));
	relDate = new QCheckBox(tr("Rel. Date"));
	relObserver = new QCheckBox(tr("Rel. Observer"));
	relComments = new QCheckBox(tr("Rel. Comments"));
}

void ResultsViewerOptions::createPhasesSection() {
	pretest = new QCheckBox(tr("Pretest"));
	habituation = new QCheckBox(tr("Habituation"));
	test = new QCheckBox(tr("Test"));
}

void ResultsViewerOptions::createExperimentSummarySection() {
	experimentSummary = new QCheckBox(tr("Experiment Summary"));
	lookTimeline = new QCheckBox(tr("Look Timeline"));
}

void ResultsViewerOptions::createTrialInfoSection() {
	listWidget = new QListWidget();
	attentionLatencies = new QCheckBox(tr("Attention Latencies"));
	attentionOnTime = new QCheckBox(tr("Attention On Times"));
	leftMonitorLookTime = new QCheckBox(tr("Left Monitor Look Times"));
	rightMonitorLookTime = new QCheckBox(tr("Right Monitor Look Times"));
	centerMonitorLookTime = new QCheckBox(tr("Center Monitor Look Times"));
	totalLookTime = new QCheckBox(tr("Total Look Times"));
	trialDurations = new QCheckBox(tr("Trial Durations"));
	numberLeftLooks = new QCheckBox(tr("Number of Left Looks"));
	numberCenterLooks = new QCheckBox(tr("Number of Center Looks"));
	numberRightLooks = new QCheckBox(tr("Number of Right Looks"));
	totalNumberLooks = new QCheckBox(tr("Total Number of Looks"));
	leftMonitorFilenames = new QCheckBox(tr("Left Monitor Filenames"));
	centerMonitorFilenames = new QCheckBox(tr("Center Monitor Filenames"));
	rightMonitorFilenames = new QCheckBox(tr("Right Monitor Filenames"));
	independentSoundFilenames = new QCheckBox(tr("Independent Sound Filenames"));
	stimulusNames = new QCheckBox(tr("Stimulus Names"));

}

void ResultsViewerOptions::createControlButtons() {
	cancel = new QPushButton(tr("Cancel"));
	ok = new QPushButton(tr("OK"));
}

void ResultsViewerOptions::makeConnections() {
	connect(cancel, SIGNAL(clicked()), this, SLOT(on_cancel()));
	connect(ok, SIGNAL(clicked()), this, SLOT(on_ok()));
}

void ResultsViewerOptions::on_cancel() {
	//close();
	reject();
}

void ResultsViewerOptions::on_ok() {
	writeValuesToDB();
	//close();
	accept();
}

void ResultsViewerOptions::doLayout() {
	doExperimentHeaderLayout();
	doReliabilityInfoLayout();
	doPhasesLayout();
	doExperimentSummaryLayout();
	doTrialInformationLayout();
	doMainLayout();
}

void ResultsViewerOptions::doExperimentHeaderLayout() {
	experimentHeaderGroup = new QGroupBox(tr("Select which experiment header information to export"));
	QVBoxLayout* exp = new QVBoxLayout(experimentHeaderGroup);
	QHBoxLayout* hb1 = new QHBoxLayout();
	hb1->addWidget(subjectName);
	hb1->addWidget(birthDate);
	hb1->addWidget(testDate);

	QHBoxLayout* hb2 = new QHBoxLayout();
	hb2->addWidget(observer);
	hb2->addWidget(condition);
	hb2->addWidget(cellNo);
	hb2->addWidget(comments);

	exp->addLayout(hb1);
	exp->addLayout(hb2);
}

void ResultsViewerOptions::doReliabilityInfoLayout() {
	reliabilityInfoGroup = new QGroupBox(tr("Select which reliability info to export"));
	QHBoxLayout* hb = new QHBoxLayout(reliabilityInfoGroup);
	hb->addWidget(ongFileName);
	hb->addWidget(relDate);
	hb->addWidget(relObserver);
	hb->addWidget(relComments);
}

void ResultsViewerOptions::doPhasesLayout() {
	phasesGroup = new QGroupBox(tr("Select which phases to display"));
	QHBoxLayout* hb = new QHBoxLayout(phasesGroup);
	hb->addWidget(pretest);
	hb->addWidget(habituation);
	hb->addWidget(test);
}

void ResultsViewerOptions::doExperimentSummaryLayout() {
	experimentSummaryGroup = new QGroupBox(tr("Select which experiment summary information to display"));
	QHBoxLayout* hb = new QHBoxLayout(experimentSummaryGroup);
	hb->addWidget(experimentSummary);
	hb->addWidget(lookTimeline);
}

void ResultsViewerOptions::doTrialInformationLayout() {
	trialInfoGroup = new QGroupBox(tr("Select the information to display for each trial"));
	QHBoxLayout* hb = new QHBoxLayout(trialInfoGroup);
	QVBoxLayout* vb1 = new QVBoxLayout();
	vb1->addWidget(attentionLatencies);
	vb1->addWidget(attentionOnTime);
	vb1->addWidget(trialDurations);
	vb1->addWidget(leftMonitorFilenames);
	vb1->addWidget(centerMonitorFilenames);
	vb1->addWidget(rightMonitorFilenames);
	vb1->addWidget(independentSoundFilenames);
	vb1->addWidget(stimulusNames);
	vb1->addStretch(1);
	QVBoxLayout* vb2 = new QVBoxLayout();
	vb2->addWidget(leftMonitorLookTime);
	vb2->addWidget(centerMonitorLookTime);
	vb2->addWidget(rightMonitorLookTime);
	vb2->addWidget(totalLookTime);
	vb2->addWidget(numberLeftLooks);
	vb2->addWidget(numberCenterLooks);
	vb2->addWidget(numberRightLooks);
	vb2->addWidget(totalNumberLooks);
	vb2->addStretch(1);
	hb->addLayout(vb1);
	hb->addLayout(vb2);

}

void ResultsViewerOptions::doMainLayout()
{
	//QVBoxLayout* vb = new QVBoxLayout();
	//vb->addWidget(selectInfoLabel, 0, Qt::AlignLeft);
	//vb->addWidget(listWidget);

	QHBoxLayout* controls = new QHBoxLayout();
	controls->addWidget(cancel);
	controls->addWidget(ok);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(experimentHeaderGroup);
	mainLayout->addWidget(reliabilityInfoGroup);
	mainLayout->addWidget(phasesGroup);
	mainLayout->addWidget(experimentSummaryGroup);
	mainLayout->addWidget(trialInfoGroup);
	mainLayout->addLayout(controls);
	mainLayout->setAlignment(controls, Qt::AlignRight);
}

Habit::ResultViewerSettings ResultsViewerOptions::getConfigurationObject() {
	Habit::ResultViewerSettings r;
	r.setCellNoShown(cellNo->isChecked());
	r.setCommentsShown(comments->isChecked());
	r.setConditionShown(condition->isChecked());
	r.setExperimentSummaryShown(experimentSummary->isChecked());
	r.setHabituationShown(habituation->isChecked());
	r.setLookTimelineShown(lookTimeline->isChecked());
	r.setObserverShown(observer->isChecked());
	r.setOngFileShown(ongFileName->isChecked());
	r.setPretestShown(pretest->isChecked());
	r.setRelDateShown(relDate->isChecked());
	r.setSubjectBirthdateShown(birthDate->isChecked());
	r.setSubjectNameShown(subjectName->isChecked());
	r.setTestDateShown(testDate->isChecked());
	r.setRelObserverShown(relObserver->isChecked());
	r.setRelCommentsShown(relComments->isChecked());
	r.setTestShown(test->isChecked());
	r.setAttentionLatenciesShown(attentionLatencies->isChecked());
	r.setAttentionOnTimesShown( attentionOnTime->isChecked());
	r.setLeftMonitorLookTimesShown( leftMonitorLookTime->isChecked());
	r.setRightMonitorLookTimesShown( rightMonitorLookTime->isChecked());
	r.setCenterMonitorLookTimesShown( centerMonitorLookTime->isChecked());
	r.setTotalLookTimesShown( totalLookTime->isChecked());
	r.setTrialDurationsShown( trialDurations->isChecked());
	r.setNumberOfLeftLooksShown( numberLeftLooks->isChecked());
	r.setNumberOfCenterLooksShown( numberCenterLooks->isChecked());
	r.setNumberOfRightLooksShown( numberRightLooks->isChecked());
	r.setTotalNumberOfLooksShown( totalNumberLooks->isChecked());
	r.setLeftMonitorFilenamesShown( leftMonitorFilenames->isChecked());
	r.setCenterMonitorFilenamesShown( centerMonitorFilenames->isChecked());
	r.setRightMonitorFilenamesShown( rightMonitorFilenames->isChecked());
	r.setIndependentFilenamesShown( independentSoundFilenames->isChecked());
	r.setStimulusNamesShown( stimulusNames->isChecked());
	return r;
}

void ResultsViewerOptions::initializeValuesFromDB() {
	Habit::MainDao dao;
	Habit::ResultViewerSettings settings = dao.getResultViewerOptions();
	cellNo->setChecked(settings.isCellNoShown());
	comments->setChecked(settings.isCommentsShown());
	condition->setChecked(settings.isConditionShown());
	experimentSummary->setChecked(settings.isExperimentSummaryShown());
	habituation->setChecked(settings.isHabituationShown());
	lookTimeline->setChecked(settings.isLookTimelineShown());
	observer->setChecked(settings.isObserverShown());
	ongFileName->setChecked(settings.isOngFileNameShown());
	pretest->setChecked(settings.isPretestShown());
	relDate->setChecked(settings.isRelDateShown());
	birthDate->setChecked(settings.isSubjectBirthdateShown());
	subjectName->setChecked(settings.isSubjectNameShown());
	testDate->setChecked(settings.isTestDateShown());
	relObserver->setChecked(settings.isRelObserverShown());
	relComments->setChecked(settings.isRelCommentsShown());
	test->setChecked(settings.isTestShown());
	attentionLatencies->setChecked(settings.isAttentionLatenciesShown());
	attentionOnTime->setChecked(settings.isAttentionOnTimesShown());
	leftMonitorLookTime->setChecked(settings.isLeftMonitorLookTimesShown());
	rightMonitorLookTime->setChecked(settings.isRightMonitorLookTimesShown());
	centerMonitorLookTime->setChecked(settings.isCenterMonitorLookTimesShown());
	totalLookTime->setChecked(settings.isTotalLookTimesShown());
	trialDurations->setChecked(settings.isTrialDurationsShown());
	numberLeftLooks->setChecked(settings.isNumberOfLeftLooksShown());
	numberCenterLooks->setChecked(settings.isNumberOfCenterLooksShown());
	numberRightLooks->setChecked(settings.isNumberOfRightLooksShown());
	totalNumberLooks->setChecked(settings.isTotalNumberOfLooksShown());
	leftMonitorFilenames->setChecked(settings.isLeftMonitorFilenamesShown());
	centerMonitorFilenames->setChecked(settings.isCenterMonitorFilenamesShown());
	rightMonitorFilenames->setChecked(settings.isRightMonitorFilenamesShown());
	independentSoundFilenames->setChecked(settings.isIndependentFilenamesShown());
	stimulusNames->setChecked(settings.isStimulusNamesShown());

}

void ResultsViewerOptions::writeValuesToDB() {
	Habit::MainDao dao;
	Habit::ResultViewerSettings config = getConfigurationObject();
	dao.addOrUpdateConfigObject(config);
}

} // namespace GUILib