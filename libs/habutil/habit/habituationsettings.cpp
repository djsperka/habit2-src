#include "habituationsettings.h"
#include "maindao.h"
#include <QtDebug>

static const QString f_sVersion21("hs2000021");

Habit::HabituationSettings::HabituationSettings(const HHabituationType& htype) 
: id_(-1)
, lookTime_(50)
, phtype_(&htype)
, ntrials_(0)
, criterionsettings_()
{
}

Habit::HabituationSettings::~HabituationSettings()
{
}

Habit::HabituationSettings::HabituationSettings(const Habit::HabituationSettings& settings)
: id_(settings.getId())
, lookTime_(settings.getTotalLookLengthToEnd())
, phtype_(&settings.getHabituationType())
, ntrials_(settings.getNTrials())
, criterionsettings_(settings.getCriterionSettings())
{
}

Habit::HabituationSettings& Habit::HabituationSettings::operator=(const Habit::HabituationSettings& rhs)
{
	if (this != &rhs)
	{
		setId(rhs.getId());
		setNTrials(rhs.getNTrials());
		setHabituationType(rhs.getHabituationType());
		setTotalLookLengthToEnd(rhs.getTotalLookLengthToEnd());
		setCriterionSettings(rhs.getCriterionSettings());
	}
	return *this;
}


Habit::HabituationSettings Habit::HabituationSettings::clone() const
{
	Habit::HabituationSettings settings(*this);
	settings.setId(-1);
	return settings;
}

QDataStream & Habit::operator<< (QDataStream& stream, Habit::HabituationSettings d)
{
	stream << f_sVersion21 << d.getId() << d.getTotalLookLengthToEnd() << d.getHabituationType().number() << d.getNTrials() << d.getCriterionSettings();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::HabituationSettings& d)
{
	QString sVersion;
	int id, lookTime,itype, ntrials;
	CriterionSettings c;

	// Save position in stream in case this is an old version
	qint64 pos = stream.device()->pos();
	stream >> sVersion;
	if (sVersion == f_sVersion21)
	{
		stream >> id >> lookTime >> itype >> ntrials >> c;
		d.setId(id);
		d.setTotalLookLengthToEnd(lookTime);
		d.setHabituationType(getHabituationType(itype));
		d.setNTrials(ntrials);
		d.setCriterionSettings(c);
	}
	else
	{
		// reset stream to position before trying to read version
		stream.device()->seek(pos);

		// and now read
		stream >> id >> lookTime >> itype >> c;
		d.setId(id);
		d.setTotalLookLengthToEnd(lookTime);
		d.setHabituationType(getHabituationType(itype));
		d.setNTrials(-1);				// this has to get set when the phase is read for old versions.
		d.setCriterionSettings(c);
	}
	return stream;
}

bool Habit::operator==(const Habit::HabituationSettings& lhs, const Habit::HabituationSettings& rhs)
{
	bool b;
	Habit::CriterionSettings csl(lhs.getCriterionSettings());
	Habit::CriterionSettings csr(rhs.getCriterionSettings());
	b = (lhs.getId() == rhs.getId() &&
		lhs.getTotalLookLengthToEnd() == rhs.getTotalLookLengthToEnd() &&
		lhs.getHabituationType() == rhs.getHabituationType() &&
		lhs.getNTrials() == rhs.getNTrials() &&
		lhs.getCriterionSettings() == rhs.getCriterionSettings());
	return b;
}

QDebug Habit::operator<<(QDebug dbg, const HabituationSettings& settings)
{
	dbg.space() << "Id:" << settings.getId() << "Type:" << settings.getHabituationType().name() << endl;
	dbg.space() << "NTrials:" << settings.getNTrials() << endl;
	dbg.space() << "TotalLookLength:" << settings.getTotalLookLengthToEnd() << endl;
	dbg.space() << "CriterionSettings:" << settings.getCriterionSettings() << endl;
	return dbg.space();
}


int Habit::HabituationSettings::getId() const
{
	return id_;
}

void Habit::HabituationSettings::setId(int id)
{
	id_ = id;
}

const HHabituationType& Habit::HabituationSettings::getHabituationType() const
{
    return *phtype_;
}

void Habit::HabituationSettings::setHabituationType(const HHabituationType& htype)
{
    phtype_ = &htype;
}

const Habit::CriterionSettings Habit::HabituationSettings::getCriterionSettings() const
{
    return criterionsettings_;
}

void Habit::HabituationSettings::setCriterionSettings(Habit::CriterionSettings criterionsettings)
{
    criterionsettings_ = criterionsettings;
}

void Habit::HabituationSettings::loadFromDB(int phaseId)
{
	Habit::MainDao maindao;
	maindao.getHabituationSettingsForPhase(phaseId, *this);
}

void Habit::HabituationSettings::saveToDB(int phaseId)
{
	Habit::MainDao dao;
	dao.addOrUpdateHabituationSettings(phaseId, *this);
}

int Habit::HabituationSettings::getTotalLookLengthToEnd() const
{
	return lookTime_;
}

void Habit::HabituationSettings::setTotalLookLengthToEnd(int length)
{
	lookTime_ = length;
}

