
#ifndef HABIT_DESIGNE_SETTINGS_H
#define HABIT_DESIGNE_SETTINGS_H

/// \file designsettings.h
/// \brief Declaraction of class Habit::DesignSettings

#include "trialsinfo.h"
#include <stdlib.h>

/// Common namespace for all entities of the Habit
namespace Habit
{

/// Stores design settings of the experiment.
class DesignSettings
{
public:
    DesignSettings();
    ~DesignSettings();

public:
	int getId() const;
	void setId(int);

    TrialsInfo getPretestTrialsInfo() const;
    void setPretestTrialsInfo(const TrialsInfo& trialsInfo);

    TrialsInfo getHabituationTrialsInfo() const;
    void setHabituationTrialsInfo(const TrialsInfo& trialsInfo);

    TrialsInfo getTestTrialsInfo() const;
    void setTestTrialsInfo(const TrialsInfo& trialsInfo);
	void loadFromDB(size_t id);
	bool saveToDB(size_t id_);

private:
	int id_;
	Habit::TrialsInfo pretestTrialsInfo_;
	Habit::TrialsInfo habituationTrialsInfo_;
	Habit::TrialsInfo testTrialsInfo_;
};

} // namespace Habit

#endif // HABIT_DESIGNE_SETTINGS_H
