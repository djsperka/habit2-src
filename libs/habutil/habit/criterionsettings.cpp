#include "criterionsettings.h"

Habit::CriterionSettings::CriterionSettings(const HCriterionBasisType& basis, unsigned int percent, unsigned int windowSize, const HCriterionWindowType& windowType)
: pbasis_(&basis)
, percent_(percent)
, windowSize_(windowSize)
, pwindowType_(&windowType)
{
}

Habit::CriterionSettings::~CriterionSettings()
{
}

const HCriterionBasisType& Habit::CriterionSettings::getBasis() const 
{ 
	return *pbasis_; 
}

void Habit::CriterionSettings::setBasis(const HCriterionBasisType& basis) 
{ 
	pbasis_ = &basis; 
}

unsigned int Habit::CriterionSettings::getPercent() const
{
    return percent_;
}

void Habit::CriterionSettings::setPercent(unsigned int persent)
{
    percent_ = persent;
}

unsigned int Habit::CriterionSettings::getWindowSize() const
{
    return windowSize_;
}

void Habit::CriterionSettings::setWindowSize(unsigned int windowSize)
{
    windowSize_ = windowSize;
}

const HCriterionWindowType& Habit::CriterionSettings::getWindowType() const
{
    return *pwindowType_;
}

void Habit::CriterionSettings::setWindowType(const HCriterionWindowType& windowType)
{
    pwindowType_ = &windowType;
}