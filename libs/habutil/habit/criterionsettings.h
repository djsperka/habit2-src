
#ifndef HABIT_CRITERION_SETTINGS_H
#define HABIT_CRITERION_SETTINGS_H

/// \file criterionsettings.h
/// \brief Declaraction of class Habit::CriterionSettings


#include "HTypes.h"
#include <QDataStream>

/// Common namespace for all entities of the Habit
namespace Habit
{

/// Stores habituation criterion settings.
class CriterionSettings
{
public:
    CriterionSettings(const HCriterionBasisType& basis = HCriterionBasisType::HCriterionBasisFirstN, unsigned int percent=50, unsigned int windowSize=3, const HCriterionWindowType& windowType = HCriterionWindowType::HCriterionWindowFixed);
    ~CriterionSettings();
    //CriterionSettings& operator = (const CriterionSettings&);
	
	
    const HCriterionBasisType& getBasis() const;
    void setBasis(const HCriterionBasisType& basis);
    unsigned int getPercent() const;
    void setPercent(unsigned int persent);
    unsigned int getWindowSize() const;
    void setWindowSize(unsigned int windowSize);
    const HCriterionWindowType& getWindowType() const;
    void setWindowType(const HCriterionWindowType& windowType);

private:
	const HCriterionBasisType* pbasis_;
	unsigned int percent_;
	unsigned int windowSize_;
	const HCriterionWindowType* pwindowType_;
};

QDataStream & operator<< (QDataStream& stream, Habit::CriterionSettings d);
QDataStream & operator>> (QDataStream& stream, Habit::CriterionSettings& d);
bool operator==(const Habit::CriterionSettings& lhs, const Habit::CriterionSettings& rhs);



} // namespace Habit

#endif // HABIT_CRITERION_SETTINGS_H
