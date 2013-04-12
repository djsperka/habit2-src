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

int Habit::HabituationSettings::getId() const {
	return id_;
}

void Habit::HabituationSettings::setId(int id) {
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

