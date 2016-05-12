
#ifndef HABIT_HABITUATION_SETTINGS_H
#define HABIT_HABITUATION_SETTINGS_H

/// \file habituationsettings.h
/// \brief Declaraction of class Habit::HabituationSettings

#include "criterionsettings.h"
#include <QDataStream>
#include "HTypes.h"

/// Common namespace for all entities of the Habit
namespace Habit
{

/// Stores habituation settings.
class HabituationSettings
{
public:
    HabituationSettings(const HHabituationType& htype = HHabituationType::HHabituationTypeCriterion);
    HabituationSettings(const HabituationSettings& settings);
    HabituationSettings& operator=(const HabituationSettings& rhs);
    ~HabituationSettings();
    HabituationSettings clone();

public:
	int getId() const;
	void setId(int);

    const HHabituationType& getHabituationType() const;
    void setHabituationType(const HHabituationType& htype);

//    const Habit::CriterionSettings& getCriterionSettings() const;
    const Habit::CriterionSettings getCriterionSettings() const;
    void setCriterionSettings(Habit::CriterionSettings criterionsettings);
	int getTotalLookLengthToEnd() const;
	void setTotalLookLengthToEnd(int length);
	bool loadFromDB(int phaseId);
	bool saveToDB(int phaseId);
	int getNTrials() const { return ntrials_; };
	void setNTrials(int n) { ntrials_ = n; };

private:
	int id_;
	int lookTime_;
	const HHabituationType* phtype_;
	int ntrials_;
	CriterionSettings criterionsettings_;
};

QDataStream & operator<< (QDataStream& stream, Habit::HabituationSettings d);
QDataStream & operator>> (QDataStream& stream, Habit::HabituationSettings& d);
bool operator==(const Habit::HabituationSettings& lhs, const Habit::HabituationSettings& rhs);

} // namespace Habit

#endif // HABIT_HABITUATION_SETTINGS_H
