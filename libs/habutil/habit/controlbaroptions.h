
#ifndef HABIT_CONTROL_BAR_OPTIONS_H
#define HABIT_CONTROL_BAR_OPTIONS_H

#include <stdlib.h>
#include <QDataStream>

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
    ControlBarOptions(const ControlBarOptions&);
    ~ControlBarOptions();
    ControlBarOptions& operator=(const ControlBarOptions& rhs);

public:
	int getId() const;
	void setId(int);
    bool isControlBarUsed() const;
    void setUseControlBar(bool use);
    bool isCurrentExperimentDisplayed() const;
    void setDisplayCurrentExperiment(bool display);
    bool isCurrentStimulusDisplayed() const;
    void setDisplayCurrentStimulus(bool display);
    bool isLookingDirectionDisplayed() const;
    void setDisplayLookingDirection(bool display);
	void loadFromDB(int experimentID);
	void saveToDB(int experimentID);
	ControlBarOptions clone();

private:
	int id_;
	bool useControlBar_;
	bool displayCurrentExperiment_;
	bool displayCurrentStimulus_;
	bool displayLookingDirection_;
};

QDataStream & operator<< (QDataStream& stream, ControlBarOptions settings);
QDataStream & operator>> (QDataStream& stream, ControlBarOptions& settings);
QDebug operator<<(QDebug dbg, ControlBarOptions& cbo);
bool operator==(const Habit::ControlBarOptions& lhs, const Habit::ControlBarOptions& rhs);

} // namespace Habit

#endif // HABIT_CONTROL_BAR_OPTIONS_H
