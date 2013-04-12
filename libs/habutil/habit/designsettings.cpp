#include "designsettings.h"
#include "maindao.h"

Habit::DesignSettings::DesignSettings() 
	: pretestTrialsInfo_()
	, habituationTrialsInfo_()
	, testTrialsInfo_()
{
	id_ = -1;
	pretestTrialsInfo_.setLookTimes(0);
	habituationTrialsInfo_.setLookTimes(0);

}

Habit::DesignSettings::~DesignSettings()
{}

int Habit::DesignSettings::getId() const {
	return id_;
}

void Habit::DesignSettings::setId(int id) {
	id_ = id;
}

Habit::TrialsInfo
Habit::DesignSettings::getPretestTrialsInfo() const
{
    return pretestTrialsInfo_;
}

void
Habit::DesignSettings::setPretestTrialsInfo(const Habit::TrialsInfo& trialsInfo)
{
    pretestTrialsInfo_ = trialsInfo;
}

Habit::TrialsInfo
Habit::DesignSettings::getHabituationTrialsInfo() const
{
    return habituationTrialsInfo_;
}

void
Habit::DesignSettings::setHabituationTrialsInfo(const Habit::TrialsInfo& trialsInfo)
{
    habituationTrialsInfo_ = trialsInfo;
}


Habit::TrialsInfo
Habit::DesignSettings::getTestTrialsInfo() const
{
    return testTrialsInfo_;
}

void
Habit::DesignSettings::setTestTrialsInfo(const Habit::TrialsInfo& trialsInfo)
{
    testTrialsInfo_ = trialsInfo;
}

void
Habit::DesignSettings::loadFromDB(size_t id)
{
	Habit::MainDao maindao;
	maindao.getDesignSettingsForExperiment(id, this);
}

bool Habit::DesignSettings::saveToDB( size_t id_ )
{
	Habit::MainDao dao;
	return dao.addOrUpdateDesignSetting(id_, this);
}
