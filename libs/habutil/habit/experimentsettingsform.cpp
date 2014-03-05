#include "experimentsettingsform.h"
#include "treewizard.h"
#include "monitorsettingsform.h"
#include "controlbaroptionsform.h"
#include "habituationsetupform.h"
#include "stimulusdisplayform.h"
#include "attentionsetupform.h"
#include "habituationsettings.h"
#include "habituationstimuliform.h"
#include "experimentsettings.h"
#include "designsetupform.h"
#include "maindao.h"
#include "connection.h"
#include "cloneexperimentdialog.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QComboBox>
#include <QtGui/QMessageBox>

using namespace GUILib;

const QString ExperimentSettingsForm::NEW_EXPERIMENT = "-- New Experiment --";
const QString ExperimentSettingsForm::CLONE_EXPERIMENT = "-- Clone Experiment --";

ExperimentSettingsForm::ExperimentSettingsForm(const Habit::ExperimentSettings& experimentSettings, QWidget* w)
    : QDialog(w)
    , experimentSettings_(experimentSettings)
{
    treeWizard_ = new GUILib::TreeWizard(0); 
  
    treeWizard_->addNodeItem(tr("General"), 0);
    monitorSettingsForm_ = new MonitorSettingsForm(experimentSettings_.getMonitorSettings());
    treeWizard_->addPageItem(monitorSettingsForm_, tr("Monitor settings"), tr("General"));
    controlBarOptionsForm_ = new ControlBarOptionsForm(experimentSettings_.getControlBarOptions());
    treeWizard_->addPageItem(controlBarOptionsForm_, tr("Control Bar Options"), tr("General"));
    
    treeWizard_->addNodeItem(tr("Experiment"), 0);
	designSetupForm_ = new DesignSetupForm(experimentSettings_.getDesignSettings());
    treeWizard_->addPageItem(designSetupForm_, tr("Design Settings"), tr("Experiment"));
	habituationSetupForm_ = new HabituationSetupForm(experimentSettings_.getHabituationSettings());
    treeWizard_->addPageItem(habituationSetupForm_, tr("Habituation Settings"), tr("Experiment"));
	
    treeWizard_->addNodeItem(tr("Stimuli"), 0);
	stimulusDisplayForm_ = new StimulusDisplayForm(experimentSettings_.getStimulusDisplayInfo());
	treeWizard_->addPageItem(stimulusDisplayForm_, tr("Stimulus Display"), tr("Stimuli"));
	attentionSetup_ = new AttentionSetupForm(experimentSettings_.getAttentionGetterSettings());
    treeWizard_->addPageItem(attentionSetup_, tr("Attention Getting"), tr("Stimuli"));

	preTestStimuliForm_ = new HabituationStimuliForm(experimentSettings_.getPreTestStimuliSettings(), "PreTest Stimuli");
    treeWizard_->addPageItem(preTestStimuliForm_, tr("PreTest Stimuli"), tr("Stimuli"));

	habituationStimuliForm_ = new HabituationStimuliForm(experimentSettings_.getHabituationStimuliSettings(), "Habituation Stimuli");
    treeWizard_->addPageItem(habituationStimuliForm_, tr("Habituation Stimuli"), tr("Stimuli"));

	testStimuliForm_ = new HabituationStimuliForm(experimentSettings_.getTestStimuliSettings(), "Test Stimuli");
    treeWizard_->addPageItem(testStimuliForm_, tr("Test Stimuli"), tr("Stimuli"));
	
    treeWizard_->setCurrentPage(0);
	treeWizard_->setControlsEnabled(false);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(treeWizard_);
	layout->setSizeConstraint(QLayout::SetFixedSize);
    //layout->addWidget(experimentNameGrBx, 0, 0, Qt::AlignTop | Qt::AlignLeft);

    QObject::connect(treeWizard_, SIGNAL(canceled()), this,  SLOT(onCancel()));
    QObject::connect(treeWizard_, SIGNAL(done()), this,  SLOT(onDone()));
	QObject::connect(treeWizard_, SIGNAL(experimentActivated(QString)), this, SLOT(onExperimentChoose(QString)));
	connect(preTestStimuliForm_, SIGNAL(copyStimulusToPretest(Habit::StimulusSettings*)), preTestStimuliForm_, SLOT(onCopyStimulus(Habit::StimulusSettings*)));
	connect(preTestStimuliForm_, SIGNAL(copyStimulusToHabit(Habit::StimulusSettings*)), habituationStimuliForm_, SLOT(onCopyStimulus(Habit::StimulusSettings*)));
	connect(preTestStimuliForm_, SIGNAL(copyStimulusToTest(Habit::StimulusSettings*)), testStimuliForm_, SLOT(onCopyStimulus(Habit::StimulusSettings*)));
	connect(habituationStimuliForm_, SIGNAL(copyStimulusToPretest(Habit::StimulusSettings*)), preTestStimuliForm_, SLOT(onCopyStimulus(Habit::StimulusSettings*)));
	connect(habituationStimuliForm_, SIGNAL(copyStimulusToHabit(Habit::StimulusSettings*)), habituationStimuliForm_, SLOT(onCopyStimulus(Habit::StimulusSettings*)));
	connect(habituationStimuliForm_, SIGNAL(copyStimulusToTest(Habit::StimulusSettings*)), testStimuliForm_, SLOT(onCopyStimulus(Habit::StimulusSettings*)));
	connect(testStimuliForm_, SIGNAL(copyStimulusToPretest(Habit::StimulusSettings*)), preTestStimuliForm_, SLOT(onCopyStimulus(Habit::StimulusSettings*)));
	connect(testStimuliForm_, SIGNAL(copyStimulusToHabit(Habit::StimulusSettings*)), habituationStimuliForm_, SLOT(onCopyStimulus(Habit::StimulusSettings*)));
	connect(testStimuliForm_, SIGNAL(copyStimulusToTest(Habit::StimulusSettings*)), testStimuliForm_, SLOT(onCopyStimulus(Habit::StimulusSettings*)));
	initializeAvailableExperimentSettings();
}

ExperimentSettingsForm::~ExperimentSettingsForm()
{}

Habit::ExperimentSettings GUILib::ExperimentSettingsForm::getConfigurationObject()
{
	Habit::ExperimentSettings settings;
	settings.setId(experimentSettings_.getId());
	settings.setName(experimentSettings_.getName());
	return settings;
}

void GUILib::ExperimentSettingsForm::setControlsActive(bool isActive) {
	treeWizard_->setControlsEnabled(isActive);
}

void GUILib::ExperimentSettingsForm::onExperimentChoose(const QString& name) {
	QComboBox * experimentBox_ = treeWizard_->getExperimentComboBox();
	if(name == NEW_EXPERIMENT) {
		experimentBox_->setEditable(true);
		experimentBox_->setCurrentIndex(-1);
		setConfigurationObject(Habit::ExperimentSettings());
		initialize();
		setControlsActive(true);
	} 
	else if (name == CLONE_EXPERIMENT)
	{
		GUILib::CloneExperimentDialog dlg(this);
		if (dlg.exec() == QDialog::Accepted)
		{
			QString name = dlg.getExperimentName();
			Habit::MainDao dao;
			experimentSettings_.setName(name);
			dao.getExperimentSettingsByName(&experimentSettings_);
			if(experimentSettings_.getId() != (unsigned int)-1) {
				experimentSettings_.loadFromDB();
			}
			experimentSettings_.setName("");
			experimentSettings_.setId(-1);
			experimentBox_->setEditable(true);
			experimentBox_->setCurrentIndex(-1);
			Habit::MonitorSettings monitorSettings = experimentSettings_.getMonitorSettings();
			Habit::ControlBarOptions controlBarOptions = experimentSettings_.getControlBarOptions();
			Habit::DesignSettings designSettings = experimentSettings_.getDesignSettings();
			Habit::HabituationSettings habituationSettings = experimentSettings_.getHabituationSettings();
			Habit::StimulusDisplayInfo stimulusDisplayInfo = experimentSettings_.getStimulusDisplayInfo();
			Habit::AttentionGetterSettings attentionGetterSettings = experimentSettings_.getAttentionGetterSettings();

			monitorSettings.setId(-1);
			controlBarOptions.setId(-1);
			designSettings.setId(-1);
			habituationSettings.setId(-1);
			stimulusDisplayInfo.setId(-1);
			attentionGetterSettings.setId(-1);
			Habit::StimulusSettings ass = attentionGetterSettings.getAttentionGetterStimulus();
			ass.setId(-1);
			attentionGetterSettings.setAttentionGetterStimulus(ass);
			experimentSettings_.setMonitorSettings(monitorSettings);
			experimentSettings_.setControlBarOptions(controlBarOptions);
			experimentSettings_.setDesignSettings(designSettings);
			experimentSettings_.setHabituationSettings(habituationSettings);
			experimentSettings_.setStimulusDisplayInfo(stimulusDisplayInfo);
			experimentSettings_.setAttentionGetterSettings(attentionGetterSettings);

			Habit::StimuliSettings pretestSS = experimentSettings_.getPreTestStimuliSettings();
			Habit::StimulusSettingsList preSSList = pretestSS.getStimuli();
			for (Habit::StimulusSettingsListIterator it = preSSList.begin(); it!= preSSList.end(); it++)
				it->setId(-1);
			pretestSS.setStimuli(preSSList);

			Habit::StimuliSettings habituationSS = experimentSettings_.getHabituationStimuliSettings();
			Habit::StimulusSettingsList habitSSList = habituationSS.getStimuli();
			for (Habit::StimulusSettingsListIterator it = habitSSList.begin(); it!= habitSSList.end(); it++)
				it->setId(-1);
			habituationSS.setStimuli(habitSSList);
			Habit::StimuliSettings testSS = experimentSettings_.getTestStimuliSettings();
			Habit::StimulusSettingsList teSSList = testSS.getStimuli();
			for (Habit::StimulusSettingsListIterator it = teSSList.begin(); it!= teSSList.end(); it++)
				it->setId(-1);
			testSS.setStimuli(teSSList);

			experimentSettings_.setPreTestStimuliSettings(pretestSS);
			experimentSettings_.setHabituationStimuliSettings(habituationSS);
			experimentSettings_.setTestStimuliSettings(testSS);
			setConfigurationObject(experimentSettings_);
			initialize();
			setControlsActive(true);
		}
	}
	else {
		if(name.trimmed().isEmpty()) {
			setControlsActive(false);
		} else {
			setControlsActive(true);
			experimentBox_->setEditable(false);
			Habit::MainDao dao;
			experimentSettings_.setName(name);
			dao.getExperimentSettingsByName(&experimentSettings_);
			if(experimentSettings_.getId() != (unsigned int)-1) {
				experimentSettings_.loadFromDB();
			}
			initialize();
		}
	}
}

void GUILib::ExperimentSettingsForm::onCancel() {
	close();
}

void GUILib::ExperimentSettingsForm::onDone() {
	QComboBox * experimentBox_ = treeWizard_->getExperimentComboBox();
	QString experimentName = experimentBox_->currentText();
	if(!experimentName.isEmpty()) {
		experimentSettings_.setName(experimentName);
		experimentSettings_.setAttentionGetterSettings(attentionSetup_->getConfigurationObject());
		experimentSettings_.setControlBarOptions(controlBarOptionsForm_->getConfigurationObject());
		experimentSettings_.setDesignSettings(designSetupForm_->getConfigurationObject());
		experimentSettings_.setHabituationSettings(habituationSetupForm_->getConfigurationObject());
		experimentSettings_.setHabituationStimuliSettings(habituationStimuliForm_->getConfigurationObject());
		experimentSettings_.setMonitorSettings(monitorSettingsForm_->getConfigurationObject());
		experimentSettings_.setPreTestStimuliSettings(preTestStimuliForm_->getConfigurationObject());
		experimentSettings_.setStimulusDisplayInfo(stimulusDisplayForm_->getConfigurationObject());
		experimentSettings_.setTestStimuliSettings(testStimuliForm_->getConfigurationObject());
		if(!validateMonitorSettings()) {
			QMessageBox::warning(this, "Invalid monitor settings", "You have one or more stimuli that are configured for invalid monitors.  Please check your monitor settings.");
		} else {
			Habit::StimulusSettings stimulusSettings(attentionSetup_->getConfigurationObject().getAttentionGetterStimulus());
			if (attentionSetup_->getConfigurationObject().isAttentionGetterUsed() && !isMediaExists(stimulusSettings))
			{
				QMessageBox::warning(this, "Invalid attention getter settings", "Attention getter doesn't have any media connected to monitors.  Please check your Attention Getter settings.");
			}
			else if (!validateHabituationSettings())
			{
			}
			else
			if(!experimentSettings_.saveToDB()) {
				QMessageBox::warning(this, "Save failed", "Failed to save experiment settings into database");
			} else {
				int indx = experimentBox_->findText(experimentName);
				if(indx == -1) {
					experimentBox_->addItem(experimentName);
				}
				close();
			}
		}
	} else {
		QMessageBox::warning(this, "Invalid experiment name", "Invalid experiment name specified");
	}
}

#if 0
void fillStimulus(QVector<Habit::StimulusSettings>& v, const Habit::StimuliSettings& si) {
	typedef Habit::StimuliSettings::stimulus_container M;
	M sc = si.getStimuli();
	for(M::iterator it = sc.begin(); it != sc.end(); ++it) {
		v.append(*it);
	}
}
#endif

bool GUILib::ExperimentSettingsForm::validateMonitorSettings() {
	Habit::StimulusSettingsList ssList;
	//QVector<Habit::StimulusSettings> stimulus;
	ssList.append(experimentSettings_.getAttentionGetterSettings().getAttentionGetterStimulus());
	ssList.append(experimentSettings_.getHabituationStimuliSettings().getStimuli());
	ssList.append(experimentSettings_.getPreTestStimuliSettings().getStimuli());
	ssList.append(experimentSettings_.getTestStimuliSettings().getStimuli());
	bool result = true;
	for (Habit::StimulusSettingsListConstIterator it = ssList.begin(); it != ssList.end(); it++)
		if (it->isLeftEnabled() && experimentSettings_.getMonitorSettings().getLeftMonitorIndex() < 0 ||
			it->isCenterEnabled() && experimentSettings_.getMonitorSettings().getCenterMonitorIndex() < 0 ||
			it->isRightEnabled() && experimentSettings_.getMonitorSettings().getRightMonitorIndex() < 0 ||
			it->isIndependentSoundEnabled() && experimentSettings_.getMonitorSettings().getControlMonitorIndex() < 0)
				result = false;
	return result;
}

bool GUILib::ExperimentSettingsForm::validateHabituationSettings()
{
	Habit::HabituationSettings hs = habituationSetupForm_->getConfigurationObject();
	if (hs.getHabituationType() == HHabituationType::HHabituationTypeCriterion &&  hs.getCriterionSettings().getWindowSize() < 1)
	{
		QMessageBox::warning(this, "Invalid habituation settings", "Window Size should be more than 0.  Please check your Habituation settings.");
		return false;
	}
	return true;
}

bool GUILib::ExperimentSettingsForm::isMediaExists(Habit::StimulusSettings& stimulus)
{
	return ((!stimulus.getLeftStimulusInfo().getFileName().isEmpty()  ) && (stimulus.isLeftEnabled())) ||
		((!stimulus.getCenterStimulusInfo().getFileName().isEmpty()  ) && (stimulus.isCenterEnabled())) ||
		((!stimulus.getRightStimulusInfo().getFileName().isEmpty()  ) && (stimulus.isRightEnabled()));
}
void GUILib::ExperimentSettingsForm::setConfigurationObject( const Habit::ExperimentSettings& settings)
{
	experimentSettings_ = settings;
	initialize();
}

void GUILib::ExperimentSettingsForm::initialize() {
	QComboBox * experimentBox_ = treeWizard_->getExperimentComboBox();
	int indx = experimentBox_->findText(experimentSettings_.getName());
	if(indx == -1) {
		experimentBox_->addItem(experimentSettings_.getName());
		indx = experimentBox_->count() - 1;
	}
	experimentBox_->setCurrentIndex(indx);
	monitorSettingsForm_->setConfigurationObject(experimentSettings_.getMonitorSettings());
	controlBarOptionsForm_->setConfigurationObject(experimentSettings_.getControlBarOptions());
	designSetupForm_->setConfigurationObject(experimentSettings_.getDesignSettings());
	habituationSetupForm_->setConfigurationObject(experimentSettings_.getHabituationSettings());
	stimulusDisplayForm_->setConfigurationObject(experimentSettings_.getStimulusDisplayInfo());
	attentionSetup_->setConfigurationObject(experimentSettings_.getAttentionGetterSettings());
	preTestStimuliForm_->setConfigurationObject(experimentSettings_.getPreTestStimuliSettings());
	
	//preTestStimuliForm_->setAllConfigObjects(experimentSettings_.getPreTestStimuliSettings(), experimentSettings_.getHabituationStimuliSettings(), experimentSettings_.getTestStimuliSettings());
	habituationStimuliForm_->setConfigurationObject(experimentSettings_.getHabituationStimuliSettings());
	//habituationStimuliForm_->setAllConfigObjects(experimentSettings_.getPreTestStimuliSettings(), experimentSettings_.getHabituationStimuliSettings(), experimentSettings_.getTestStimuliSettings());
	testStimuliForm_->setConfigurationObject(experimentSettings_.getTestStimuliSettings());
	//testStimuliForm_->setAllConfigObjects(experimentSettings_.getPreTestStimuliSettings(), experimentSettings_.getHabituationStimuliSettings(), experimentSettings_.getTestStimuliSettings());
	

}

void GUILib::ExperimentSettingsForm::initializeAvailableExperimentSettings() {
	QComboBox * experimentBox_ = treeWizard_->getExperimentComboBox();
	Habit::MainDao dao;
	QStringList sl = dao.getAllExperimentNames();
	experimentBox_->addItems(sl);
}
