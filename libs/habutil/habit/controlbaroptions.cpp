#include "controlbaroptions.h"
#include "maindao.h"

Habit::ControlBarOptions::ControlBarOptions() 
{
	id_ = -1;
    useControlBar_ = true;
    displayCurrentExperiment_ = false;
    displayCurrentStimulus_ = false;
}

Habit::ControlBarOptions::~ControlBarOptions()
{

}

int Habit::ControlBarOptions::getId() const {
	return id_;
}

void Habit::ControlBarOptions::setId(int id) {
	id_ = id;
}

bool Habit::ControlBarOptions::isControlBarUsed() const
{
    return useControlBar_;
}

void Habit::ControlBarOptions::setUseControlBar(bool use)
{
    useControlBar_ = use;
}

bool Habit::ControlBarOptions::isCurrentExperimentDisplayed() const
{
    return displayCurrentExperiment_;
}

void Habit::ControlBarOptions::setDisplayCurrentExperiment(bool display)
{
    displayCurrentExperiment_ = display;
}

bool Habit::ControlBarOptions::isCurrentStimulusDisplayed() const
{
    return displayCurrentStimulus_;
}

void Habit::ControlBarOptions::setDisplayCurrentStimulus(bool display)
{
    displayCurrentStimulus_ = display;
}

void Habit::ControlBarOptions::loadFromDB(size_t id)
{
	Habit::MainDao mainDao;
	mainDao.getControlBarOptionsForExperiment(id, this);

}

bool Habit::ControlBarOptions::saveToDB( size_t id_ )
{
	Habit::MainDao dao;
	return dao.addOrUpdateControlBarOption(id_, this);
}
