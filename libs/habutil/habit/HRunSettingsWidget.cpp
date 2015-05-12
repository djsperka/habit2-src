#include "HRunSettingsWidget.h"
#include "habituationstimulitable.h"
//#include "habitcontrolpanel.h"
#include "stimulisettings.h"
#include "maindao.h"
#include "subjectinformation.h"
#include "runsettings.h"
#include "experimentresultsform.h"

#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QTextEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QListWidgetItem>
#include <QtGui/QMessageBox>
#include <QtCore/QString>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

namespace GUILib {

const int orderEditHeight = 40;

const QString HRunSettingsWidget::SUBJECT_INFORMATION = "Subject Information";
const QString HRunSettingsWidget::HABITUATION_ORDER = "Habituation Order";
const QString HRunSettingsWidget::PRETEST_ORDER = "Pretest Order";
const QString HRunSettingsWidget::TEST_ORDER = "Test Order";

const QString HRunSettingsWidget::TEST_RUN = "Test Run";
//const QString HRunSettingsWidget::EXISTING_RUN = "Existing Run";
const QString HRunSettingsWidget::NEW_RUN = "New Run";

HRunSettingsWidget::HRunSettingsWidget(QWidget* w)
	: QDialog(w)
	, activeExperiment_()
	, subjectSettings_()
	, runSettings_()
	, currentTestOrder_()
{
	createComponents();
	makeConnections();
	doLayout();
	setControlsEnabled(false, false);
	onRunTypeActivation(TEST_RUN);
}

void HRunSettingsWidget::createComponents() {
	runTypeGroup_ = new QGroupBox(tr("Run Settings"));
	runTypeCombo_ = new QComboBox(runTypeGroup_);
	initializeRunTypeCombo();
	detailedLabel_ = new QLabel("A Test run will run through the specified "
								"stimuli according to the design settings "
								"without scripting the results.\n"
								"Use the setting to check the program configuration "
								"before running actual subjects.");
	settingsGroup_ = new QGroupBox();
	settingsLabel_ = new QLabel(tr("Settings"));
	settingsList_ = new QListWidget();
	initializeSettingsList();
	stimuliTable_ = new HabituationStimuliTable(activeExperiment_.getPreTestStimuliSettings());
	stimuliOrderLabel_ = new QLabel(tr("Enter the order to present the stimuli as numbers separated by spaces :"));
	stimuliOrderEdit_ = new QTextEdit();
	stimuliOrderEdit_->setFixedHeight(orderEditHeight);
	randomizeGroup_ = new QGroupBox(tr("Randomize Order"));
	randomizeGroup_->setCheckable(true);
	/*
	 * From the docs for QGroupBox::setCheckable()
	 * 
	 * "If this property is enabled for a group box, it will also be initially checked to ensure that its contents are enabled."
	 * 
	 * This means that by default the checkbox is enabled...which is a pain. Set it to NOT checked. 
	 */ 
	randomizeGroup_->setChecked(false);
	randomizeOrderCombo_ = new QComboBox();
	randomizeOrderCombo_->addItem("Simple randomization");
	randomizeOrderCombo_->addItem("Randomize by blocks");
	experimentsChooserLabel_ = new QLabel(tr("Select Experiment: "));
	experimentBox_ = new QComboBox();
	//experimentBox_->setFixedWidth(150);
	experimentBox_->setAutoCompletion(true);
	initializeExperimentsBox();
	subjectChooserLabel_ = new QLabel(tr("Select Subject: "));
	subjectChooser_ = new QComboBox();
	subjectChooserLabel_->setVisible(false);
	subjectChooser_->setVisible(false);
	//subjectChooser_->setFixedWidth(250);
	subjectChooser_->setAutoCompletion(true);
	initializeSubjects();

	cancelButton_ = new QPushButton(tr("Cancel"));
	doneButton_ = new QPushButton(tr("Start"));
	//editSubjectInformation_ = new QPushButton(tr("Edit subject information"));
}

void HRunSettingsWidget::makeConnections() {
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(close()));	
	connect(doneButton_, SIGNAL(clicked()), this, SLOT(onDone()));	
	connect(settingsList_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onOrderActivation(QListWidgetItem*)));
	connect(settingsList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));
	connect(experimentBox_, SIGNAL(activated(QString)), this, SLOT(onExperimentActivation(QString)));
	//connect(editSubjectInformation_, SIGNAL(clicked()), this, SLOT(onSubjectInformationEdit()));
	connect(runTypeCombo_, SIGNAL(activated(QString)), this, SLOT(onRunTypeActivation(QString)));
	connect(subjectChooser_, SIGNAL(activated(QString)), this, SLOT(onSubjectChosen(QString)));
}

void HRunSettingsWidget::doRunTypeLayout()
{	
	QVBoxLayout* runTypeLayout = new QVBoxLayout();
	runTypeGroup_->setLayout(runTypeLayout);	
	runTypeLayout->addWidget(detailedLabel_);
	runTypeLayout->addWidget(settingsGroup_);
}

void HRunSettingsWidget::doSettingsLayout()
{	
	horizontalLayout_ = new QHBoxLayout();
	settingsGroup_->setLayout(horizontalLayout_);
	QVBoxLayout* settingsLayout = new QVBoxLayout();
	settingsLayout->addWidget(settingsLabel_);
	settingsLayout->addWidget(settingsList_);
	settingsLayout->addStretch();
	horizontalLayout_->addLayout(settingsLayout);
}

void HRunSettingsWidget::doStimuliLayout()
{	
	stackedWgt_ = new QStackedWidget(this);
	subjectInfoWgt_ = new SubjectInformationWidget();
	
	QVBoxLayout* stimuliLayout = new QVBoxLayout();
	
	stimuliLayout->addWidget(stimuliTable_);
	stimuliLayout->addWidget(stimuliOrderLabel_);
	stimuliLayout->addWidget(stimuliOrderEdit_);
	QHBoxLayout* randomizeOrderLayout = new QHBoxLayout();
	stimuliLayout->addLayout(randomizeOrderLayout);
	randomizeOrderLayout->addWidget(randomizeGroup_);
	randomizeOrderLayout->addStretch();
	QHBoxLayout* randomizeLayout = new QHBoxLayout();
	randomizeGroup_->setLayout(randomizeLayout);
	randomizeLayout->addStretch();
	randomizeLayout->addWidget(randomizeOrderCombo_);

	stimuliContainer_ = new QWidget();
	stimuliContainer_->setLayout(stimuliLayout);
	stackedWgt_->addWidget(stimuliContainer_);
	stackedWgt_->addWidget(subjectInfoWgt_);
	horizontalLayout_->addSpacing(30);
	horizontalLayout_->addWidget(stackedWgt_);
}

void HRunSettingsWidget::doButtonsLayout()
{
	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	//buttonsLayout->addWidget(editSubjectInformation_);
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(cancelButton_);
	buttonsLayout->addWidget(doneButton_);
	mainLayout_->addLayout(buttonsLayout);
}

void HRunSettingsWidget::doLayout() {
	QHBoxLayout* runTypeSection = new QHBoxLayout();
	runTypeSection->addWidget(new QLabel(tr("Run type: ")));
	runTypeSection->addWidget(runTypeCombo_);
	runTypeSection->addSpacing(20);
	runTypeSection->addWidget(subjectChooserLabel_);
	runTypeSection->addWidget(subjectChooser_);
	runTypeSection->addStretch();
	
	QHBoxLayout* experimentSection = new QHBoxLayout();
	experimentSection->addWidget(experimentsChooserLabel_);
	experimentSection->addWidget(experimentBox_);
	experimentSection->addStretch();

	QVBoxLayout* topControls = new QVBoxLayout();
	topControls->addLayout(runTypeSection);
	topControls->addSpacing(20);
	topControls->addLayout(experimentSection);

	mainLayout_ = new QVBoxLayout(this);
	mainLayout_->addLayout(topControls);
	mainLayout_->addWidget(runTypeGroup_);
	mainLayout_->setAlignment(experimentSection, Qt::AlignLeft);
	doRunTypeLayout();
	doSettingsLayout();
	doStimuliLayout();
	doButtonsLayout();
}

HRunSettingsWidget::~HRunSettingsWidget()
{
}

QString HRunSettingsWidget::getRunLabel()
{
	if (runTypeCombo_->currentText() == NEW_RUN)
	{
		return QString("%1_%2_%3").arg(activeExperiment_.getName()).arg(subjectSettings_.getSubjectName()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmm"));
	}
	else
	{
		return QString("%1_TEST_%2").arg(activeExperiment_.getName()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmm"));
	}
}

void HRunSettingsWidget::onDone()
{
	int experimentId = activeExperiment_.getId();
	runSettings_.setExperimentId(experimentId);
	initializeRunSettings(currentTestOrder_);
	if(isInformationCorrect()) {
		Habit::MainDao dao;
		if(runTypeCombo_->currentText() == NEW_RUN) {
			dao.insertSubject(&subjectSettings_);
			runSettings_.setSubjectId(subjectSettings_.getId());
			dao.insertRunSettings(&runSettings_);
		} 
		accept();
		

	}
}

bool areStimulusIndicesOk(const QString order, const Habit::StimuliSettings& ss) {
	bool result = true;
	QList<QPair<int, QString> > list;
	result = Habit::RunSettings::getOrderFromString(list, order);
	if (result)
	{
		QListIterator<QPair<int, QString> > it(list);
		while (it.hasNext() && result)
		{
			QPair<int, QString> p = it.next();
			result = (p.first > 0 && p.first <= ss.stimuli().size());
		}
	}
	return result;
}

bool HRunSettingsWidget::isInformationCorrect()
{
	QString hOrder = runSettings_.getHabituationOrder();
	QString pOrder = runSettings_.getPretestOrder();
	QString tOrder = runSettings_.getTestOrder();
	subjectSettings_ = subjectInfoWgt_->getConfigurationObject();

	// Check PreTest order
	if (activeExperiment_.getPreTestPhaseSettings().getIsEnabled() &&
			(pOrder.isEmpty() || !areStimulusIndicesOk(pOrder, activeExperiment_.getPreTestStimuliSettings())))
	{
		QMessageBox::warning(this, "Invalid pretest order", "Pretest order is invalid, indices must be separated by spaces or commas and must correspond with the table indices.");
		return false;
	}

	// Check Habituation order
	if (activeExperiment_.getHabituationPhaseSettings().getIsEnabled() &&
			(hOrder.isEmpty() || !areStimulusIndicesOk(hOrder, activeExperiment_.getHabituationStimuliSettings())))
	{
		QMessageBox::warning(this, "Invalid habituation order", "Habituation order is invalid, indices must be separated by spaces or commas and must correspond with the table indices.");
		return false;
	}

	// Check Habituation order
	if (activeExperiment_.getTestPhaseSettings().getIsEnabled() &&
			(tOrder.isEmpty() || !areStimulusIndicesOk(tOrder, activeExperiment_.getTestStimuliSettings())))
	{
		QMessageBox::warning(this, "Invalid test order", "Test order is invalid, indices must be separated by spaces or commas and must correspond with the table indices.");
		return false;
	}

	// Check subject info if this is not a test run
	if((runTypeCombo_->currentText() == NEW_RUN ) && !subjectSettings_.isOk())
	{
		QMessageBox::warning(this, "Invalid subject information", "Subject information is invalid.  Subject name and observer name are required.");
		return false;
	}

	if ((runTypeCombo_->currentText() == NEW_RUN) && !subjectInfoWgt_->validate())
	{
		return false;
	}

	return true;
}

void HRunSettingsWidget::onOrderActivation(QListWidgetItem* item) {
	Q_ASSERT(0 != item);
	QString orderName = item->text();
	if (orderName == SUBJECT_INFORMATION)
	{
		stackedWgt_->setCurrentIndex(1);
	}
	else
	{
		stackedWgt_->setCurrentIndex(0);
		if(currentTestOrder_.isEmpty()) {
			currentTestOrder_ = orderName;
		}
		initializeRunSettings(currentTestOrder_);
		initializeStimuliTable(orderName);
		currentTestOrder_ = orderName;
	}
}

void HRunSettingsWidget::onCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	qDebug() << "onCurrentItemChanged(" << (current ? current->text() : "NULL") << ", " << (previous ? previous->text() : "NULL");
}


void HRunSettingsWidget::initializeRunSettings(const QString& orderName)  {
	if(orderName == HABITUATION_ORDER) {
		runSettings_.setHabituationOrder(stimuliOrderEdit_->toPlainText());
		runSettings_.setHabituationRandomized(randomizeGroup_->isChecked());
		runSettings_.setHabituationRandomizeMethod(randomizeOrderCombo_->currentIndex());
	} else
	if(orderName == PRETEST_ORDER) {
		runSettings_.setPretestOrder(stimuliOrderEdit_->toPlainText());
		runSettings_.setPretestRandomized(randomizeGroup_->isChecked());
		runSettings_.setPretestRandomizeMethod(randomizeOrderCombo_->currentIndex());
	} else
	if(orderName == TEST_ORDER) {
		runSettings_.setTestOrder(stimuliOrderEdit_->toPlainText());
		runSettings_.setTestRandomized(randomizeGroup_->isChecked());
		runSettings_.setTestRandomizeMethod(randomizeOrderCombo_->currentIndex());
	}
}

void HRunSettingsWidget::initializeStimuliTable(const QString& orderName) {
	if(orderName == HABITUATION_ORDER) {
		settingsList_->setCurrentRow(settingsList_->count()-2);
		stimuliTable_->initializeTable(activeExperiment_.getHabituationStimuliSettings());
		stimuliOrderEdit_->setText(runSettings_.getHabituationOrder());
		randomizeOrderCombo_->setCurrentIndex(runSettings_.getHabituationRandomizeMethod());
		randomizeGroup_->setChecked(runSettings_.isHabituationRandomized());
	} else
	if(orderName == PRETEST_ORDER) {
		settingsList_->setCurrentRow(settingsList_->count()-3);
		stimuliTable_->initializeTable(activeExperiment_.getPreTestStimuliSettings());
		stimuliOrderEdit_->setText(runSettings_.getPretestOrder());
		randomizeOrderCombo_->setCurrentIndex(runSettings_.getPretestRandomizeMethod());
		randomizeGroup_->setChecked(runSettings_.isPretestRandomized());
	} else
	if(orderName == TEST_ORDER) {
		settingsList_->setCurrentRow(settingsList_->count()-1);
		stimuliTable_->initializeTable(activeExperiment_.getTestStimuliSettings());
		stimuliOrderEdit_->setText(runSettings_.getTestOrder());
		randomizeOrderCombo_->setCurrentIndex(runSettings_.getTestRandomizeMethod());
		randomizeGroup_->setChecked(runSettings_.isTestRandomized());
	}
}

void HRunSettingsWidget::initializeSubjects() {
	Habit::MainDao dao;
	QStringList s = dao.getAllSubjectsNames();
	subjectChooser_->addItems(s);
	subjectChooser_->setCurrentIndex(-1);
}

void HRunSettingsWidget::onExperimentActivation(const QString& experimentName) {
	Q_UNUSED(experimentName);
	Habit::MainDao dao;
	activeExperiment_.setName(experimentBox_->currentText());
	dao.getExperimentSettingsByName(&activeExperiment_);
	activeExperiment_.loadFromDB();
	setControlsEnabled(true, false);
	initializeRunSettings(TEST_ORDER);
	initializeStimuliTable(TEST_ORDER);
	initializeRunSettings(HABITUATION_ORDER);
	initializeStimuliTable(HABITUATION_ORDER);
	initializeRunSettings(PRETEST_ORDER);
	initializeStimuliTable(PRETEST_ORDER);
	currentTestOrder_ = PRETEST_ORDER;
}

void HRunSettingsWidget::onSubjectInformationEdit() {
	SubjectInformation ss(subjectSettings_);
	int result = ss.exec();
	if(result == QMessageBox::Accepted) {
		subjectSettings_ = ss.getConfigurationObject();
	}
}

void HRunSettingsWidget::onRunTypeActivation(const QString& runType)  {
	if(runType == NEW_RUN || runType == TEST_RUN) {
		subjectChooser_->setEnabled(false);
		subjectChooserLabel_->setEnabled(false);
		experimentBox_->setEnabled(true);
		onExperimentActivation(experimentBox_->currentText());
		experimentsChooserLabel_->setEnabled(true);
		subjectChooser_->setCurrentIndex(-1);
	}

#if TEMP_HACK
	settingsList_->clear();
	if (runType == NEW_RUN)
	{
		settingsList_->addItem(new QListWidgetItem(SUBJECT_INFORMATION));
		settingsList_->addItem(new QListWidgetItem(PRETEST_ORDER));
		settingsList_->addItem(new QListWidgetItem(HABITUATION_ORDER));
		settingsList_->addItem(new QListWidgetItem(TEST_ORDER));
	}
	else
	{
		settingsList_->addItem(new QListWidgetItem(PRETEST_ORDER));
		settingsList_->addItem(new QListWidgetItem(HABITUATION_ORDER));
		settingsList_->addItem(new QListWidgetItem(TEST_ORDER));
	}
	settingsList_->setCurrentRow(0);
	if (runType == NEW_RUN)
		stackedWgt_->setCurrentIndex(1);
	else
		stackedWgt_->setCurrentIndex(0);
#else
	if (runType == NEW_RUN)
	{
		QListWidgetItem* pitem = settingsList_->item(0);
		pitem->setFlags(Qt::ItemIsSelectable);
		stackedWgt_->setCurrentIndex(1);
		qDebug() << "onRunTypeActivation(" << runType << "): set item " << pitem->text() << " selectable";
	}
	else
	{
		QListWidgetItem* pitem = settingsList_->item(0);
		pitem->setFlags(Qt::NoItemFlags);
		stackedWgt_->setCurrentIndex(0);
		qDebug() << "onRunTypeActivation(" << runType << "): set item " << pitem->text() << " noItemFlags";
	}


#endif
}

void HRunSettingsWidget::onSubjectChosen(const QString& subj) {
	Habit::MainDao dao;
	subjectSettings_ = dao.getSubjectSettings(subj);
	runSettings_ = dao.getRunSettingsBySubject(subjectSettings_);
	activeExperiment_ = dao.getExperimentSettingsById(runSettings_.getExperimentId());
	activeExperiment_.loadFromDB();
	int indx = experimentBox_->findText(activeExperiment_.getName());
	experimentBox_->setCurrentIndex(indx);
	initializeStimuliTable(HABITUATION_ORDER);
	initializeStimuliTable(PRETEST_ORDER);
	initializeStimuliTable(TEST_ORDER);
	setControlsEnabled(true, true);
}

void HRunSettingsWidget::setControlsEnabled(bool isEnabled, bool isSubjInfoEnabled) {
	settingsGroup_->setEnabled(isEnabled);
	if (!isEnabled)
	{
		settingsList_->selectionModel()->clearSelection();
		stimuliTable_->clearContents();
		stimuliTable_->setRowCount(0);
	}
	else
	{
		// If a phase is not enabled, make it unselectable
	}
	randomizeGroup_->setEnabled(isEnabled);
 	doneButton_->setEnabled(isEnabled);
	//editSubjectInformation_->setEnabled(isEnabled);
	experimentBox_->setEnabled(isEnabled);
	experimentsChooserLabel_->setEnabled(isEnabled);
	subjectChooser_->setEnabled(isSubjInfoEnabled);
	subjectChooserLabel_->setEnabled(isSubjInfoEnabled);
}

void HRunSettingsWidget::initializeSettingsList()
{
	Q_ASSERT(0 != settingsList_);
	settingsList_->addItem(new QListWidgetItem(SUBJECT_INFORMATION));
	settingsList_->addItem(new QListWidgetItem(PRETEST_ORDER));
	settingsList_->addItem(new QListWidgetItem(HABITUATION_ORDER));
	settingsList_->addItem(new QListWidgetItem(TEST_ORDER));
}

void HRunSettingsWidget::initializeExperimentsBox() {
	Habit::MainDao dao;
	Q_ASSERT(0 != experimentBox_);
	QStringList sl = dao.getAllExperimentNames();
	if(!sl.isEmpty()) {
		experimentBox_->addItems(sl);
		experimentBox_->setCurrentIndex(0);
	} else {
		QMessageBox::warning(this, "No any configured experiments found", "Before running experiment please create experiment configuration");
		close();
	}
}

void HRunSettingsWidget::initializeRunTypeCombo() {
	Q_ASSERT(0 != runTypeCombo_);
	runTypeCombo_->addItem(TEST_RUN);
	//runTypeCombo_->addItem(EXISTING_RUN);
	runTypeCombo_->addItem(NEW_RUN);
}

}
