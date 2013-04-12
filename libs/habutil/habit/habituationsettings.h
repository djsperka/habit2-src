
#ifndef HABIT_HABITUATION_SETTINGS_H
#define HABIT_HABITUATION_SETTINGS_H

/// \file habituationsettings.h
/// \brief Declaraction of class Habit::HabituationSettings

#include "criterionsettings.h"
#include <stdlib.h>
#include "HTypes.h"

/// Common namespace for all entities of the Habit
namespace Habit
{

/// Stores habituation settings.
class HabituationSettings
{
public:
    HabituationSettings(const HHabituationType& htype = HHabituationType::HHabituationTypeCriterion);
    ~HabituationSettings();

public:
#if 0	
    enum HabituationType {
        eFixedNumberOfTrials = 0,
        eUseCriterionToEnd = 1,
        eUseTotalLookingTimeToEnd  = 2
    };
#endif
	int getId() const;
	void setId(int);

    const HHabituationType& getHabituationType() const;
    void setHabituationType(const HHabituationType& htype);

    const Habit::CriterionSettings& getCriterionSettings() const;
    void setCriterionSettings(Habit::CriterionSettings criterionsettings);
	int getTotalLookLengthToEnd() const;
	void setTotalLookLengthToEnd(int length);
	void loadFromDB(size_t id);
	bool saveToDB(size_t id_);

private:
	int id_;
	int lookTime_;
	const HHabituationType* phtype_;
	CriterionSettings criterionsettings_;
};

} // namespace Habit

#endif // HABIT_HABITUATION_SETTINGS_H
