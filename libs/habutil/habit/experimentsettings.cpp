#include "experimentsettings.h"
#include "maindao.h"
#include "connection.h"
#include "HTypes.h"

Habit::ExperimentSettings::ExperimentSettings()
: id_(-1)
, pretestStimuliSettings_(HStimContext::PreTestPhase)
, habituationStimuliSettings_(HStimContext::HabituationPhase)
, testStimuliSettings_(HStimContext::TestPhase)
{
    name_ = QString();
}

Habit::ExperimentSettings::~ExperimentSettings()
{
}

void Habit::ExperimentSettings::setId(size_t id) {
	id_ = id;
}

namespace Habit
{
	size_t Habit::ExperimentSettings::getId() const {
		 return id_;
	}
}

void Habit::ExperimentSettings::setName(const QString& name) {
	name_ = name;
}

QString Habit::ExperimentSettings::getName() const
{
	return name_;
}


Habit::MonitorSettings
Habit::ExperimentSettings::getMonitorSettings() const
{
    return monitorSettings_;
}

void
Habit::ExperimentSettings::setMonitorSettings(const Habit::MonitorSettings& monitorSettings)
{
    monitorSettings_ = monitorSettings;
}

Habit::ControlBarOptions
Habit::ExperimentSettings::getControlBarOptions() const
{
    return controlBarOptions_;
}

void
Habit::ExperimentSettings::setControlBarOptions(const Habit::ControlBarOptions& controlBarOptions)
{
    controlBarOptions_ = controlBarOptions;
}

Habit::DesignSettings
Habit::ExperimentSettings::getDesignSettings() const
{
    return designSettings_;
}

void
Habit::ExperimentSettings::setDesignSettings(const Habit::DesignSettings& designSettings)
{
    designSettings_ = designSettings;
}

Habit::HabituationSettings
Habit::ExperimentSettings::getHabituationSettings() const
{
    return habituationSettings_;
}

void
Habit::ExperimentSettings::setHabituationSettings(const Habit::HabituationSettings& habituationSettings)
{
    habituationSettings_ = habituationSettings;
}

Habit::StimulusDisplayInfo
Habit::ExperimentSettings::getStimulusDisplayInfo() const
{
    return stimulusDisplayInfo_;
}

void
Habit::ExperimentSettings::setStimulusDisplayInfo(const Habit::StimulusDisplayInfo& stimulusDisplayInfo)
{
    stimulusDisplayInfo_ = stimulusDisplayInfo;
}

Habit::AttentionGetterSettings
Habit::ExperimentSettings::getAttentionGetterSettings() const
{
    return attentionGetterSettings_;
}

void
Habit::ExperimentSettings::setAttentionGetterSettings(const Habit::AttentionGetterSettings& attentionGetterSettings)
{
    attentionGetterSettings_ = attentionGetterSettings;
}

Habit::StimuliSettings
Habit::ExperimentSettings::getPreTestStimuliSettings() const
{
    return pretestStimuliSettings_;
}

void
Habit::ExperimentSettings::setPreTestStimuliSettings(const Habit::StimuliSettings& pretestStimuliSettings)
{
    pretestStimuliSettings_ = pretestStimuliSettings;
}

Habit::StimuliSettings
Habit::ExperimentSettings::getHabituationStimuliSettings() const
{
    return habituationStimuliSettings_;
}

void
Habit::ExperimentSettings::setHabituationStimuliSettings(const Habit::StimuliSettings& habituationStimuliSettings)
{
    habituationStimuliSettings_ = habituationStimuliSettings;
}

Habit::StimuliSettings
Habit::ExperimentSettings::getTestStimuliSettings() const
{
    return testStimuliSettings_;
}

void
Habit::ExperimentSettings::setTestStimuliSettings(const Habit::StimuliSettings& testStimuliSettings)
{
    testStimuliSettings_ = testStimuliSettings;
}

bool Habit::ExperimentSettings::saveToDB() {
	Habit::MainDao dao;
	bool result = false;
	Habit::connection* con = Habit::connection::get_instance();
	Q_ASSERT(0 != con);
	con->transaction();
	if(dao.insertOrUpdateExperimentSettings(this)) {
		result = monitorSettings_.saveToDB(id_) && attentionGetterSettings_.saveToDB(id_) && controlBarOptions_.saveToDB(id_)
			&& designSettings_.saveToDB(id_) && habituationSettings_.saveToDB(id_) && stimulusDisplayInfo_.saveToDB(id_)
			&& habituationStimuliSettings_.saveToDB(id_)
			&& testStimuliSettings_.saveToDB(id_) && pretestStimuliSettings_.saveToDB(id_);
	}
	if(result) {
		con->commit();
	} else {
		con->rollback();
	}
	return result;
}

void Habit::ExperimentSettings::loadFromDB(bool byId)
{
	Habit::MainDao dao;
	if (!byId)
		dao.getExperimentSettingsByName(this);
	else
		dao.getExperimentNameById(this);
	monitorSettings_.loadFromDB(id_);
	attentionGetterSettings_.loadFromDB(id_);
	controlBarOptions_.loadFromDB(id_);
	designSettings_.loadFromDB(id_);
	habituationSettings_.loadFromDB(id_);
	stimulusDisplayInfo_.loadFromDB(id_);
	habituationStimuliSettings_.loadFromDB(id_);
	testStimuliSettings_.loadFromDB(id_);
	pretestStimuliSettings_.loadFromDB(id_);
}
