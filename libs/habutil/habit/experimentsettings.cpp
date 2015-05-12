#include "experimentsettings.h"
#include "maindao.h"
//#include "connection.h"
#include "HTypes.h"
#include <QtSql/QSqlDatabase>
#include "HDBUtil.h"

Habit::ExperimentSettings::ExperimentSettings()
: id_(-1)
, name_()
, hidden_(0)
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

Habit::ExperimentSettings Habit::ExperimentSettings::clone(const QString& name)
{
	Habit::ExperimentSettings settings;
	settings.setName(name);
	settings.setHidden(false);
	settings.setId(-1);

	settings.setControlBarOptions(this->getControlBarOptions().clone());
	settings.setHLookSettings(this->getHLookSettings().clone());
	settings.setPreTestPhaseSettings(this->getPreTestPhaseSettings().clone());
	settings.setHabituationPhaseSettings(this->getHabituationPhaseSettings().clone());
	settings.setTestPhaseSettings(this->getTestPhaseSettings().clone());
	settings.setHabituationSettings(this->getHabituationSettings().clone());
	settings.setStimulusDisplayInfo(this->getStimulusDisplayInfo().clone());
	settings.setAttentionGetterSettings(this->getAttentionGetterSettings().clone());
	settings.setPreTestStimuliSettings(this->getPreTestStimuliSettings().clone());
	settings.setHabituationStimuliSettings(this->getHabituationStimuliSettings().clone());
	settings.setTestStimuliSettings(this->getTestStimuliSettings().clone());

	return settings;
}


QDataStream & Habit::operator<< (QDataStream& stream, const Habit::ExperimentSettings& settings)
{
	int version = getDBVersion();
	stream <<
			(int)(-1*version) <<
			(int)settings.getId() <<
			settings.getName() <<
			settings.getControlBarOptions() <<
			settings.getHLookSettings() <<
			settings.getPreTestPhaseSettings() <<
			settings.getHabituationPhaseSettings() <<
			settings.getTestPhaseSettings() <<
			settings.getHabituationSettings() <<
			settings.getStimulusDisplayInfo() <<
			settings.getAttentionGetterSettings() <<
			settings.getPreTestStimuliSettings() <<
			settings.getHabituationStimuliSettings() <<
			settings.getTestStimuliSettings();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::ExperimentSettings& settings)
{
	int id, value, version=0;
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

	// djs 11/10/2014
	// Put a version number into the output for an experiment.
	// To maintain backwards compatibility with existing data (there's not much of it, but have to take care of it)
	// the new format will put the current database version (multiplied by -1) as the first value output.
	// When reading, the first value is read - if its a positive value then it must be an Id (and hence an old version).
	// The major difference is that Habit::MonitorSettings are removed from the ExperimentSettings (more appropriately
	// placed with the RunSettings).

	stream >> value;
	if (value < 0) version = -1 * value;
	qDebug() << "Loading experiment settings, version " << version;
	if (version == 0)
	{
		id = value;
		stream >>
			name >>
			monitorSettings >>
			controlBarOptions >>
			lookSettings >>
			pretestPhaseSettings >>
			habituationPhaseSettings >>
			testPhaseSettings >>
			habituationSettings >>
			stimulusDisplayInfo >>
			attentionGetterSettings >>
			pretestStimuliSettings >>
			habituationStimuliSettings >>
			testStimuliSettings;
	}
	else
	{
		// At version 2000012 a new field 'hidden' was added to the experiment settings, but it is not saved.
		stream >>
			id >>
			name >>
			controlBarOptions >>
			lookSettings >>
			pretestPhaseSettings >>
			habituationPhaseSettings >>
			testPhaseSettings >>
			habituationSettings >>
			stimulusDisplayInfo >>
			attentionGetterSettings >>
			pretestStimuliSettings >>
			habituationStimuliSettings >>
			testStimuliSettings;
	}
	settings.setId(id);
	settings.setName(name);
	// 11/10/2014 djs settings.setMonitorSettings(monitorSettings);
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
//			lhs.getMonitorSettings() == rhs.getMonitorSettings() &&
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

bool Habit::ExperimentSettings::isHidden() const
{
	return (hidden_ != 0);
}

void Habit::ExperimentSettings::setHidden(bool bHidden)
{
	hidden_ = bHidden ? 1 : 0;
}

Habit::ControlBarOptions Habit::ExperimentSettings::getControlBarOptions() const
{
    return controlBarOptions_;
}

void Habit::ExperimentSettings::setControlBarOptions(const Habit::ControlBarOptions& controlBarOptions)
{
    controlBarOptions_ = controlBarOptions;
}

Habit::HabituationSettings Habit::ExperimentSettings::getHabituationSettings() const
{
    return habituationSettings_;
}

void Habit::ExperimentSettings::setHabituationSettings(const Habit::HabituationSettings& habituationSettings)
{
    habituationSettings_ = habituationSettings;
}

Habit::StimulusDisplayInfo Habit::ExperimentSettings::getStimulusDisplayInfo() const
{
    return stimulusDisplayInfo_;
}

void Habit::ExperimentSettings::setStimulusDisplayInfo(const Habit::StimulusDisplayInfo& stimulusDisplayInfo)
{
    stimulusDisplayInfo_ = stimulusDisplayInfo;
}

const Habit::AttentionGetterSettings& Habit::ExperimentSettings::getAttentionGetterSettings() const
{
    return attentionGetterSettings_;
}

Habit::AttentionGetterSettings& Habit::ExperimentSettings::getAttentionGetterSettings()
{
    return attentionGetterSettings_;
}

void Habit::ExperimentSettings::setAttentionGetterSettings(const Habit::AttentionGetterSettings& attentionGetterSettings)
{
    attentionGetterSettings_ = attentionGetterSettings;
}

const Habit::StimuliSettings& Habit::ExperimentSettings::getPreTestStimuliSettings() const
{
    return pretestStimuliSettings_;
}

Habit::StimuliSettings& Habit::ExperimentSettings::getPreTestStimuliSettings()
{
    return pretestStimuliSettings_;
}

void Habit::ExperimentSettings::setPreTestStimuliSettings(const Habit::StimuliSettings& pretestStimuliSettings)
{
    pretestStimuliSettings_ = pretestStimuliSettings;
}

const Habit::StimuliSettings& Habit::ExperimentSettings::getHabituationStimuliSettings() const
{
    return habituationStimuliSettings_;
}

Habit::StimuliSettings& Habit::ExperimentSettings::getHabituationStimuliSettings()
{
    return habituationStimuliSettings_;
}

void Habit::ExperimentSettings::setHabituationStimuliSettings(const Habit::StimuliSettings& habituationStimuliSettings)
{
    habituationStimuliSettings_ = habituationStimuliSettings;
}

const Habit::StimuliSettings& Habit::ExperimentSettings::getTestStimuliSettings() const
{
    return testStimuliSettings_;
}

Habit::StimuliSettings& Habit::ExperimentSettings::getTestStimuliSettings()
{
    return testStimuliSettings_;
}

void Habit::ExperimentSettings::setTestStimuliSettings(const Habit::StimuliSettings& testStimuliSettings)
{
    testStimuliSettings_ = testStimuliSettings;
}

bool Habit::ExperimentSettings::saveToDB()
{
	Habit::MainDao dao;
	bool result = false;
	QSqlDatabase db = QSqlDatabase::database();	// default connection, assumed to be open.

//	Q_ASSERT(connection::get_instance()->isOpen());
//	connection::get_instance()->transaction();
	Q_ASSERT(db.isOpen());
	db.transaction();
	if(dao.insertOrUpdateExperimentSettings(this)) {
		result = attentionGetterSettings_.saveToDB(id_) && controlBarOptions_.saveToDB(id_)
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
//	monitorSettings_.loadFromDB(id_);
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

bool Habit::ExperimentSettings::load(Habit::ExperimentSettings& settings, int id)
{
	bool b = false;
	Habit::MainDao dao;
	if (dao.experimentExists(id))
	{
		b = true;
		settings.setId(id);
		settings.loadFromDB(true);
	}
	return b;
}

bool Habit::ExperimentSettings::load(Habit::ExperimentSettings& settings, const QString& name)
{
	bool b = false;
	int id;
	Habit::MainDao dao;
	if (dao.experimentExists(name, id))
	{
		b = true;
		settings.setId(id);
		settings.loadFromDB(true);
	}
	return b;
}



bool Habit::ExperimentSettings::deleteFromDB()
{
	Habit::MainDao dao;
	bool result = false;
	QSqlDatabase db = QSqlDatabase::database();	// default connection, assumed to be open.

	Q_ASSERT(db.isOpen());
	db.transaction();
	result = dao.deleteExperimentSettings(this);
	if(result)
	{
		db.commit();
	} else
	{
		db.rollback();
	}
	return result;
}


bool Habit::ExperimentSettings::load(const QString& filename, ExperimentSettings& settings)
{
	QFile file(filename);
	QString version;
	bool b = false;

	if (file.open(QIODevice::ReadOnly))
	{
		QDataStream in(&file);
		in >> version >> settings;
		qDebug() << "Loading settings from file " << filename << " created with Habit version " << version;
		file.close();
		b = true;
	}
	else
	{
		qCritical() << "Cannot open export file " << filename;
	}
	return b;
}
