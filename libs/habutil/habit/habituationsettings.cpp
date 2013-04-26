#include "habituationsettings.h"
#include "maindao.h"

Habit::HabituationSettings::HabituationSettings(const HHabituationType& htype) 
: id_(-1)
, lookTime_(50)
, phtype_(&htype)
{
}

Habit::HabituationSettings::~HabituationSettings()
{
}

QDataStream & Habit::operator<< (QDataStream& stream, Habit::HabituationSettings d)
{
	stream << d.getId() << d.getTotalLookLengthToEnd() << d.getHabituationType().number() << d.getCriterionSettings();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::HabituationSettings& d)
{
	int id, lookTime,itype;
	CriterionSettings c;
	stream >> id >> lookTime >> itype >> c;
	d.setId(id);
	d.setTotalLookLengthToEnd(lookTime);
	d.setHabituationType(getHabituationType(itype));
	d.setCriterionSettings(c);
	return stream;
}

bool Habit::operator==(const Habit::HabituationSettings& lhs, const Habit::HabituationSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getTotalLookLengthToEnd() == rhs.getTotalLookLengthToEnd() &&
			lhs.getHabituationType() == rhs.getHabituationType() &&
			lhs.getCriterionSettings() == rhs.getCriterionSettings());
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

const Habit::CriterionSettings& Habit::HabituationSettings::getCriterionSettings() const
{
    return criterionsettings_;
}

void Habit::HabituationSettings::setCriterionSettings(Habit::CriterionSettings criterionsettings)
{
    criterionsettings_ = criterionsettings;
}

void Habit::HabituationSettings::loadFromDB(size_t id)
{
	Habit::MainDao maindao;
	maindao.getHabituationSettingsForExperiment(id, this);
}

bool Habit::HabituationSettings::saveToDB( size_t id_ )
{
	Habit::MainDao dao;
	return dao.addOrUpdateHabituationSetting(id_, this);
}

int Habit::HabituationSettings::getTotalLookLengthToEnd() const
{
	return lookTime_;
}

void Habit::HabituationSettings::setTotalLookLengthToEnd(int length)
{
	lookTime_ = length;
}

