#include "experimentsettings.h"
#include "maindao.h"
#include "HTypes.h"
#include <QtSql/QSqlDatabase>
#include <QListIterator>
#include "HDBUtil.h"


using namespace Habit;

// This dummy will be used when you ask for a phase that doesn't exist.
static HPhaseSettings f_dummyPhaseSettings;

Habit::ExperimentSettings::ExperimentSettings()
: id_(-1)
, name_()
, hidden_(0)
{
//    name_ = QString();
}

Habit::ExperimentSettings::~ExperimentSettings()
{
}

Habit::ExperimentSettings Habit::ExperimentSettings::clone(const QString& name)
{
	ExperimentSettings settings;
	settings.setName(name);
	settings.setHidden(false);
	settings.setId(-1);

	settings.setControlBarOptions(this->getControlBarOptions().clone());
	settings.setHLookSettings(this->getHLookSettings().clone());
	settings.setStimulusDisplayInfo(this->getStimulusDisplayInfo().clone());
	settings.setAttentionGetterSettings(this->getAttentionGetterSettings().clone());
	QListIterator<HPhaseSettings> phaseIterator = this->phaseIterator();
	while (phaseIterator.hasNext())
	{
		settings.appendPhase(phaseIterator.next().clone());
	}

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
			settings.getStimulusDisplayInfo() <<
			settings.getAttentionGetterSettings() <<
			settings.getHLookSettings() <<
			settings.phases();
	return stream;
}

QDataStream& Habit::operator>> (QDataStream& stream, Habit::ExperimentSettings& settings)
{
	int id, value, version=0;
	QString name;
	ControlBarOptions controlBarOptions;
	HLookSettings lookSettings;
	HabituationSettings habituationSettings;
	StimulusDisplayInfo stimulusDisplayInfo;
	AttentionGetterSettings attentionGetterSettings;
	MonitorSettings monitorSettings;

	// djs 11/10/2014
	// Put a version number into the output for an experiment.
	// To maintain backwards compatibility with existing data (there's not much of it, but have to take care of it)
	// the new format will put the current database version (multiplied by -1) as the first value output.
	// When reading, the first value is read - if its a positive value then it must be an Id (and hence an old version).
	// The major difference is that MonitorSettings are removed from the ExperimentSettings (more appropriately
	// placed with the RunSettings).

	stream >> value;
	if (value < 0) version = -1 * value;
	qDebug() << "Loading experiment settings, version " << version;
	if (version == 0)
	{
		StimuliSettings pretestStimuliSettings;
		StimuliSettings habituationStimuliSettings;
		StimuliSettings testStimuliSettings;
		HPhaseSettings pretestPhaseSettings;
		HPhaseSettings habituationPhaseSettings;
		HPhaseSettings testPhaseSettings;
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

		pretestPhaseSettings.setStimuli(pretestStimuliSettings);
		habituationPhaseSettings.setStimuli(habituationStimuliSettings);
		testPhaseSettings.setStimuli(testStimuliSettings);
		settings.appendPhase(pretestPhaseSettings);
		settings.appendPhase(habituationPhaseSettings);
		settings.appendPhase(testPhaseSettings);

		settings.setId(id);
		settings.setName(name);
		settings.setControlBarOptions(controlBarOptions);
		settings.setHLookSettings(lookSettings);
		settings.setStimulusDisplayInfo(stimulusDisplayInfo);
		settings.setAttentionGetterSettings(attentionGetterSettings);

	}
	else
	{
		if (version < 2000021)
		{
			StimuliSettings pretestStimuliSettings;
			StimuliSettings habituationStimuliSettings;
			StimuliSettings testStimuliSettings;
			HPhaseSettings pretestPhaseSettings;
			HPhaseSettings habituationPhaseSettings;
			HPhaseSettings testPhaseSettings;
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

			pretestPhaseSettings.setStimuli(pretestStimuliSettings);
			habituationPhaseSettings.setStimuli(habituationStimuliSettings);
			testPhaseSettings.setStimuli(testStimuliSettings);
			settings.appendPhase(pretestPhaseSettings);
			settings.appendPhase(habituationPhaseSettings);
			settings.appendPhase(testPhaseSettings);

			settings.setId(id);
			settings.setName(name);
			settings.setControlBarOptions(controlBarOptions);
			settings.setHLookSettings(lookSettings);
			settings.setStimulusDisplayInfo(stimulusDisplayInfo);
			settings.setAttentionGetterSettings(attentionGetterSettings);
		}
		else
		{
			QList<HPhaseSettings> phases;
			stream >>
				id >>
				name >>
				controlBarOptions >>
				stimulusDisplayInfo >>
				attentionGetterSettings >>
				lookSettings >>
				phases;
			settings.setId(id);
			settings.setName(name);
			settings.setControlBarOptions(controlBarOptions);
			settings.setHLookSettings(lookSettings);
			settings.setStimulusDisplayInfo(stimulusDisplayInfo);
			settings.setAttentionGetterSettings(attentionGetterSettings);
			settings.setPhases(phases);
		}
	}
	return stream;
}

bool Habit::operator==(const Habit::ExperimentSettings& lhs, const Habit::ExperimentSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getName() == rhs.getName() &&
			lhs.getControlBarOptions() == rhs.getControlBarOptions() &&
			lhs.getHLookSettings() == rhs.getHLookSettings() &&
			lhs.getStimulusDisplayInfo() == rhs.getStimulusDisplayInfo() &&
			lhs.getAttentionGetterSettings() == rhs.getAttentionGetterSettings() &&
			lhs.phases() == rhs.phases());
}

void Habit::ExperimentSettings::setId(size_t id)
{
	id_ = id;
}

int Habit::ExperimentSettings::getId() const
{
	 return id_;
}

void Habit::ExperimentSettings::setName(const QString& name)
{
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

ControlBarOptions Habit::ExperimentSettings::getControlBarOptions() const
{
    return m_controlBarOptions;
}

void Habit::ExperimentSettings::setControlBarOptions(const ControlBarOptions& controlBarOptions)
{
    m_controlBarOptions = controlBarOptions;
}


StimulusDisplayInfo Habit::ExperimentSettings::getStimulusDisplayInfo() const
{
    return m_stimulusDisplayInfo;
}

void Habit::ExperimentSettings::setStimulusDisplayInfo(const StimulusDisplayInfo& stimulusDisplayInfo)
{
    m_stimulusDisplayInfo = stimulusDisplayInfo;
}

const AttentionGetterSettings& Habit::ExperimentSettings::getAttentionGetterSettings() const
{
    return m_attentionGetterSettings;
}

AttentionGetterSettings& Habit::ExperimentSettings::getAttentionGetterSettings()
{
    return m_attentionGetterSettings;
}

void Habit::ExperimentSettings::setAttentionGetterSettings(const AttentionGetterSettings& attentionGetterSettings)
{
    m_attentionGetterSettings = attentionGetterSettings;
}

void Habit::ExperimentSettings::saveToDB()
{
	MainDao dao;
	QSqlDatabase db = QSqlDatabase::database();	// default connection, assumed to be open.
	Q_ASSERT(db.isOpen());
	db.transaction();
	try
	{
		dao.addOrUpdateExperimentSettings(*this);
		m_attentionGetterSettings.saveToDB(getId());
		m_controlBarOptions.saveToDB(getId());
		m_lookSettings.saveToDB(getId());
		m_stimulusDisplayInfo.saveToDB(getId());

		// When saving phases, we have to account for phases that may have been deleted.
		QList<int> existingPhaseIDs;
		dao.getHPhaseSettingsIDs(getId(), existingPhaseIDs);
		qDebug() << "Existing phase ids: " << existingPhaseIDs;
		QListIterator<HPhaseSettings> phaseIterator = this->phaseIterator();
		while (phaseIterator.hasNext())
		{
			int id = phaseIterator.next().getId();
			int foundAt = existingPhaseIDs.indexOf(id);
			qDebug() << "check id " << id << " found at " << foundAt;
			if (foundAt > -1)
			{
				existingPhaseIDs.removeAt(foundAt);
			}
		}

		// Anything remaining in 'existingPhaseIDs' has been deleted.
		QListIterator<int> itDelete(existingPhaseIDs);
		while (itDelete.hasNext())
		{
			int idel = itDelete.next();
			qDebug() << "Deleting phase id " << idel;
			dao.deletePhase(idel);
		}


		for (int i=0; i<this->getNumberOfPhases(); i++)
		{
			(this->phases())[i].saveToDB(this->getId());
		}
	}
	catch(HDBException& e)
	{
		db.rollback();
		throw e;
	}

	db.commit();
	return;
}

void Habit::ExperimentSettings::loadFromDB(const QString& name)
{
	MainDao dao;
	int id;
	dao.getExperimentID(name, id);
	loadFromDB(id);
	return;
}


// Caution: Can throw HDBException

void Habit::ExperimentSettings::loadFromDB(int experimentID)
{
	QString name;
	MainDao dao;

	dao.getExperimentName(experimentID, name);
	setId(experimentID);
	setName(name);
	m_controlBarOptions.loadFromDB(experimentID);
	m_stimulusDisplayInfo.loadFromDB(experimentID);
	m_attentionGetterSettings.loadFromDB(experimentID);
	m_lookSettings.loadFromDB(experimentID);
	loadPhasesFromDB(experimentID);
	return;
}

void Habit::ExperimentSettings::loadPhasesFromDB(int experimentID)
{
	MainDao dao;
	int i;
	QList<int> phaseIDs;
	dao.getHPhaseSettingsIDs(experimentID, phaseIDs);
	foreach(i, phaseIDs)
	{
		HPhaseSettings p;
		p.loadFromDB(i);
		m_phases.append(p);
	}
	return;
}


#if 0

void Habit::ExperimentSettings::loadFromDB(bool byId)
{
	MainDao dao;
	if (!byId)
		dao.getExperimentSettingsByName(this);
	else
		dao.getExperimentNameById(this);
//	monitorSettings_.loadFromDB(id_);
	m_attentionGetterSettings.loadFromDB(id_);
	m_controlBarOptions.loadFromDB(id_);
	m_lookSettings.loadFromDB(id_);
	pretestPhaseSettings_.loadFromDB(id_, HPhaseType::PreTest.number());
	habituationPhaseSettings_.loadFromDB(id_, HPhaseType::Habituation.number());
	testPhaseSettings_.loadFromDB(id_, HPhaseType::Test.number());
	habituationSettings_.loadFromDB(id_);
	m_stimulusDisplayInfo.loadFromDB(id_);
	habituationStimuliSettings_.loadFromDB(id_);
	testStimuliSettings_.loadFromDB(id_);
	pretestStimuliSettings_.loadFromDB(id_);
}

bool Habit::ExperimentSettings::load(ExperimentSettings& settings, int id)
{
	bool b = false;
	MainDao dao;
	if (dao.experimentExists(id))
	{
		b = true;
		settings.setId(id);
		settings.loadFromDB(true);
	}
	return b;
}

bool Habit::ExperimentSettings::load(ExperimentSettings& settings, const QString& name)
{
	bool b = false;
	int id;
	MainDao dao;
	if (dao.experimentExists(name, id))
	{
		b = true;
		settings.setId(id);
		settings.loadFromDB(true);
	}
	return b;
}
#endif



void Habit::ExperimentSettings::deleteFromDB()
{
	MainDao dao;
	QSqlDatabase db = QSqlDatabase::database();	// default connection, assumed to be open.

	db.transaction();
	try
	{
		dao.deleteExperiment(*this);
	}
	catch(HDBException& e)
	{
		db.rollback();
		throw e;
	}

	db.commit();
	return;
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

int Habit::ExperimentSettings::phaseExists(const QString& name) const
{
	int iFound = -1;
	int i;
	for (i=0; iFound<0 && i<m_phases.size(); i++)
		if (name == m_phases.at(i).getName()) iFound = i;
	return iFound;
}

int Habit::ExperimentSettings::phaseExists(int seqno) const
{
	int iFound = -1;
	int i;
	for (i=0; iFound<0 && i<m_phases.size(); i++)
		if (seqno == m_phases.at(i).getSeqno()) iFound = i;
	return iFound;
}

const HPhaseSettings& Habit::ExperimentSettings::phaseAt(int index) const
{
	return m_phases.at(index);
}

const HPhaseSettings& Habit::ExperimentSettings::phaseAt(const QString& name) const
{
	int i;
	for (i=0; i<m_phases.size(); i++)
		if (name == m_phases.at(i).getName()) return m_phases.at(i);
	return f_dummyPhaseSettings;
}

QStringList Habit::ExperimentSettings::getPhaseNames() const
{
	QStringList list;
	for (int i=0; i<m_phases.size(); i++)
		list << m_phases.at(i).getName();
	return list;
}
