
#ifndef HABIT_CONTROL_BAR_OPTIONS_H
#define HABIT_CONTROL_BAR_OPTIONS_H

#include <stdlib.h>

/// \file controlbaroptions.h
/// \brief Declaraction of class Habit::ControlBarOptions

/// Common namespace for all entities of the Habit
namespace Habit
{

/// Stores control bar options.
class ControlBarOptions
{
public:
    ControlBarOptions();
    //ControlBarOptions(const ControlBarOptions& );
    ~ControlBarOptions();
    //ControlBarOptions& operator = (const ControlBarOptions&);

public:
	int getId() const;
	void setId(int);
    bool isControlBarUsed() const;
    void setUseControlBar(bool use);
    bool isCurrentExperimentDisplayed() const;
    void setDisplayCurrentExperiment(bool display);
    bool isCurrentStimulusDisplayed() const;
    void setDisplayCurrentStimulus(bool display);
	void loadFromDB(size_t id);
	bool saveToDB(size_t id_);

private:
	int id_;
	bool useControlBar_;
	bool displayCurrentExperiment_;
	bool displayCurrentStimulus_;
};

} // namespace Habit

#endif // HABIT_CONTROL_BAR_OPTIONS_H
