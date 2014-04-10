#include "experimentsettings.h"
#include "maindao.h"
//#include "connection.h"
#include "HTypes.h"
#include <QtSql/QSqlDatabase>

Habit::ExperimentSettings::ExperimentSettings()
: id_(-1)
, name_()
, pretestStimuliSettings_(HStimContext::PreTestPhase)
, habituationStimuliSettings_(HStimContext::HabituationPhase)
, testStimuliSettings_(HStimContext::TestPhase)
, pretestPhaseSettings_(HPhaseType::PreTest)
, habituationPhaseSettings_(HPhaseType::Habituation)
, testPhaseSettings_(HPhaseType::Test)
{
//    name_ = QString();
}

Habit::ExperimentSettings::~ExperimentSettings()
{
}

QDataStream & Habit::operator<< (QDataStream& stream, const Habit::ExperimentSettings& settings)
{
	stream << (int)settings.getId() << settings.getName() <<
			settings.getMonitorSettings() << settings.getControlBarOptions() <<
			settings.getHLookSettings() <<
			settings.getPreTestPhaseSettings() <<
			settings.getHabituationPhaseSettings() <<
			settings.getTestPhaseSettings() <<
			settings.getHabituationSettings() << settings.getStimulusDisplayInfo() <<
			settings.getAttentionGetterSettings() <<
			settings.getPreTestStimuliSettings() <<
			settings.getHabituationStimuliSettings() <<
			settings.getTestStimuliSettings();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::ExperimentSettings& settings)
{
	int id;
	QString name;
	Habit::MonitorSettings monitorSettings;
	Habit::ControlBarOptions controlBarOptions;
	Habit::HLookSettings lookSettings;
	Habit::HPhaseSettings pretestPhaseSettings;
	Habit::HPhaseSettings habituationPhaseSettings;
	Habit::HPhaseSettings testPhaseSettings;
	Habit::HabituationSettings habituationSettings;
	Habit::StimulusDisplayInfo stimulusDisplayInfo;
	Habit::AttentionGetterSettings attentionGetterSettings;
	Habit::StimuliSettings pretestStimuliSettings(HStimContext::PreTestPhase);
	Habit::StimuliSettings habituationStimuliSettings(HStimContext::HabituationPhase);
	Habit::StimuliSettings testStimuliSettings(HStimContext::TestPhase);
	stream >> id >> name >> monitorSettings >> controlBarOptions >>
		lookSettings >>
		pretestPhaseSettings >>
		habituationPhaseSettings >>
		testPhaseSettings >>
		habituationSettings >>
		stimulusDisplayInfo >> attentionGetterSettings >>
		pretestStimuliSettings >> habituationStimuliSettings >> testStimuliSettings;
	settings.setId(id);
	settings.setName(name);
	settings.setMonitorSettings(monitorSettings);
	settings.setControlBarOptions(controlBarOptions);
	settings.setHLookSettings(lookSettings);
	settings.setPreTestPhaseSettings(pretestPhaseSettings);
	settings.setHabituationPhaseSettings(habituationPhaseSettings);
	settings.setTestPhaseSettings(testPhaseSettings);
	settings.setHabituationSettings(habituationSettings);
	settings.setStimulusDisplayInfo(stimulusDisplayInfo);
	settings.setAttentionGetterSettings(attentionGetterSettings);
	settings.setPreTestStimuliSettings(pretestStimuliSettings);
	settings.setHabituationStimuliSettings(habituationStimuliSettings);
	settings.setTestStimuliSettings(testStimuliSettings);
	return stream;
}

bool Habit::operator==(const Habit::ExperimentSettings& lhs, const Habit::ExperimentSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getName() == rhs.getName() &&
			lhs.getMonitorSettings() == rhs.getMonitorSettings() &&
			lhs.getControlBarOptions() == rhs.getControlBarOptions() &&
			lhs.getHLookSettings() == rhs.getHLookSettings() &&
			lhs.getPreTestPhaseSettings() == rhs.getPreTestPhaseSettings() &&
			lhs.getHabituationPhaseSettings() == rhs.getHabituationPhaseSettings() &&
			lhs.getTestPhaseSettings() == rhs.getTestPhaseSettings() &&
			lhs.getHabituationSettings() == rhs.getHabituationSettings() &&
			lhs.getStimulusDisplayInfo() == rhs.getStimulusDisplayInfo() &&
			lhs.getAttentionGetterSettings() == rhs.getAttentionGetterSettings() &&
			lhs.getPreTestStimuliSettings() == rhs.getPreTestStimuliSettings() &&
			lhs.getHabituationStimuliSettings() == rhs.getHabituationStimuliSettings() &&
			lhs.getTestStimuliSettings() == rhs.getTestStimuliSettings());
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
	QSqlDatabase db = QSqlDatabase::database();	// default connection, assumed to be open.

//	Q_ASSERT(connection::get_instance()->isOpen());
//	connection::get_instance()->transaction();
	Q_ASSERT(db.isOpen());
	db.transaction();
	if(dao.insertOrUpdateExperimentSettings(this)) {
		result = monitorSettings_.saveToDB(id_) && attentionGetterSettings_.saveToDB(id_) && controlBarOptions_.saveToDB(id_)
			&& lookSettings_.saveToDB(id_)
			&& pretestPhaseSettings_.saveToDB(id_)
			&& habituationPhaseSettings_.saveToDB(id_)
			&& testPhaseSettings_.saveToDB(id_)
			&& habituationSettings_.saveToDB(id_) && stimulusDisplayInfo_.saveToDB(id_)
			&& habituationStimuliSettings_.saveToDB(id_)
			&& testStimuliSettings_.saveToDB(id_) && pretestStimuliSettings_.saveToDB(id_);
	}

	if(result) {
//		connection::get_instance()->commit();
		db.commit();
	} else {
		db.rollback();
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
	lookSettings_.loadFromDB(id_);
	pretestPhaseSettings_.loadFromDB(id_, HPhaseType::PreTest.number());
	habituationPhaseSettings_.loadFromDB(id_, HPhaseType::Habituation.number());
	testPhaseSettings_.loadFromDB(id_, HPhaseType::Test.number());
	habituationSettings_.loadFromDB(id_);
	stimulusDisplayInfo_.loadFromDB(id_);
	habituationStimuliSettings_.loadFromDB(id_);
	testStimuliSettings_.loadFromDB(id_);
	pretestStimuliSettings_.loadFromDB(id_);
}
