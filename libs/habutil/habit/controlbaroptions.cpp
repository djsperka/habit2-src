#include "controlbaroptions.h"
#include "maindao.h"
#include <QDataStream>

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

QDataStream & Habit::operator<< (QDataStream& stream, Habit::ControlBarOptions cbo)
{
	stream << cbo.getId() << cbo.isControlBarUsed() << cbo.isCurrentExperimentDisplayed() << cbo.isCurrentStimulusDisplayed();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, ControlBarOptions& cbo)
{
	int id;
	bool bControlBar, bCurrExp, bCurrStim;
	stream >> id >> bControlBar >> bCurrExp >> bCurrStim;
	cbo.setId(id);
	cbo.setUseControlBar(bControlBar);
	cbo.setDisplayCurrentExperiment(bCurrExp);
	cbo.setDisplayCurrentStimulus(bCurrStim);
	return stream;
}

bool Habit::operator==(const Habit::ControlBarOptions& lhs, const Habit::ControlBarOptions& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.isControlBarUsed() == rhs.isControlBarUsed() &&
			lhs.isCurrentExperimentDisplayed() == rhs.isCurrentExperimentDisplayed() &&
			lhs.isCurrentStimulusDisplayed() == rhs.isCurrentStimulusDisplayed());
}

int Habit::ControlBarOptions::getId() const
{
	return id_;
}

void Habit::ControlBarOptions::setId(int id)
{
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