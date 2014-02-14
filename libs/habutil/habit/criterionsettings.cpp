#include "criterionsettings.h"

Habit::CriterionSettings::CriterionSettings(const HCriterionBasisType& basis, unsigned int percent, unsigned int windowSize, const HCriterionWindowType& windowType,  bool bExcludeBasisWindow, bool bRequireMinBasisValue, unsigned int uiMinBasisValue)
: pbasis_(&basis)
, percent_(percent)
, windowSize_(windowSize)
, pwindowType_(&windowType)
, m_bExcludeBasisWindow(bExcludeBasisWindow)
, m_bRequireMinBasisValue(bRequireMinBasisValue)
, m_uiMinBasisValue(uiMinBasisValue)
{
}

Habit::CriterionSettings::CriterionSettings(const CriterionSettings& c)
: pbasis_(&c.getBasis())
, percent_(c.getPercent())
, windowSize_(c.getWindowSize())
, pwindowType_(&c.getWindowType())
, m_bExcludeBasisWindow(c.getExcludeBasisWindow())
, m_bRequireMinBasisValue(c.getRequireMinBasisValue())
, m_uiMinBasisValue(c.getMinBasisValue())
{
}



Habit::CriterionSettings::~CriterionSettings()
{
}


QDataStream & Habit::operator<< (QDataStream& stream, Habit::CriterionSettings d)
{
	stream << d.getBasis().number() << d.getPercent() << d.getWindowSize() << d.getWindowType().number() << d.getExcludeBasisWindow() << d.getRequireMinBasisValue() << d.getMinBasisValue();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::CriterionSettings& d)
{
	int ibasis, iwtype;
	unsigned int percent, windowSize, minBasisValue;
	bool bExcludeBasisWindow, bRequireMinBasisValue;
	stream >> ibasis >> percent >> windowSize >> iwtype >> bExcludeBasisWindow >> bRequireMinBasisValue >> minBasisValue;
	d.setBasis(getCriterionBasisType(ibasis));
	d.setPercent(percent);
	d.setWindowSize(windowSize);
	d.setWindowType(getCriterionWindowType(iwtype));
	d.setExcludeBasisWindow(bExcludeBasisWindow);
	d.setRequireMinBasisValue(bRequireMinBasisValue);
	d.setMinBasisValue(minBasisValue);
	return stream;
}

bool Habit::operator==(const Habit::CriterionSettings& lhs, const Habit::CriterionSettings& rhs)
{
	return (lhs.getBasis() == rhs.getBasis() &&
			lhs.getPercent() == rhs.getPercent() &&
			lhs.getWindowSize() == rhs.getWindowSize() &&
			lhs.getWindowType() == rhs.getWindowType() &&
			lhs.getExcludeBasisWindow() == rhs.getExcludeBasisWindow() &&
			lhs.getRequireMinBasisValue() == rhs.getRequireMinBasisValue() &&
			(!lhs.getRequireMinBasisValue() || (lhs.getRequireMinBasisValue() && lhs.getMinBasisValue() == rhs.getMinBasisValue())));
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

void Habit::CriterionSettings::setExcludeBasisWindow(bool b)
{
	m_bExcludeBasisWindow = b;
}

bool Habit::CriterionSettings::getExcludeBasisWindow() const
{
	return m_bExcludeBasisWindow;
}

void Habit::CriterionSettings::setRequireMinBasisValue(bool b)
{
	m_bRequireMinBasisValue = b;
}

bool Habit::CriterionSettings::getRequireMinBasisValue() const
{
	return m_bRequireMinBasisValue;
}

void Habit::CriterionSettings::setMinBasisValue(unsigned int v)
{
	m_uiMinBasisValue = v;
}

unsigned int Habit::CriterionSettings::getMinBasisValue() const
{
	return m_uiMinBasisValue;
}
