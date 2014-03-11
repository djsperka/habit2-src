/*
 * HCriterionSettings.cpp
 *
 *  Created on: Mar 11, 2014
 *      Author: Oakeslab
 */

#include "HCriterionSettings.h"

Habit::HCriterionSettings::HCriterionSettings(const HCriterionBasisType& basis, unsigned int percent, unsigned int windowSize, const HCriterionWindowType& windowType,  bool bExcludeBasisWindow, bool bRequireMinBasisValue, unsigned int uiMinBasisValue)
: pbasis_(&basis)
, percent_(percent)
, windowSize_(windowSize)
, pwindowType_(&windowType)
, m_bExcludeBasisWindow(bExcludeBasisWindow)
, m_bRequireMinBasisValue(bRequireMinBasisValue)
, m_uiMinBasisValue(uiMinBasisValue)
{
}

Habit::HCriterionSettings::HCriterionSettings(const HCriterionSettings& c)
: pbasis_(&c.getBasis())
, percent_(c.getPercent())
, windowSize_(c.getWindowSize())
, pwindowType_(&c.getWindowType())
, m_bExcludeBasisWindow(c.getExcludeBasisWindow())
, m_bRequireMinBasisValue(c.getRequireMinBasisValue())
, m_uiMinBasisValue(c.getMinBasisValue())
{
}



Habit::HCriterionSettings::~HCriterionSettings()
{
}


QDataStream & Habit::operator<< (QDataStream& stream, Habit::HCriterionSettings d)
{
	stream << d.getBasis().number() << d.getPercent() << d.getWindowSize() << d.getWindowType().number() << d.getExcludeBasisWindow() << d.getRequireMinBasisValue() << d.getMinBasisValue();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::HCriterionSettings& d)
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

bool Habit::operator==(const Habit::HCriterionSettings& lhs, const Habit::HCriterionSettings& rhs)
{
	return (lhs.getBasis() == rhs.getBasis() &&
			lhs.getPercent() == rhs.getPercent() &&
			lhs.getWindowSize() == rhs.getWindowSize() &&
			lhs.getWindowType() == rhs.getWindowType() &&
			lhs.getExcludeBasisWindow() == rhs.getExcludeBasisWindow() &&
			lhs.getRequireMinBasisValue() == rhs.getRequireMinBasisValue() &&
			(!lhs.getRequireMinBasisValue() || (lhs.getRequireMinBasisValue() && lhs.getMinBasisValue() == rhs.getMinBasisValue())));
}


const HCriterionBasisType& Habit::HCriterionSettings::getBasis() const
{
	return *pbasis_;
}

void Habit::HCriterionSettings::setBasis(const HCriterionBasisType& basis)
{
	pbasis_ = &basis;
}

unsigned int Habit::HCriterionSettings::getPercent() const
{
    return percent_;
}

void Habit::HCriterionSettings::setPercent(unsigned int persent)
{
    percent_ = persent;
}

unsigned int Habit::HCriterionSettings::getWindowSize() const
{
    return windowSize_;
}

void Habit::HCriterionSettings::setWindowSize(unsigned int windowSize)
{
    windowSize_ = windowSize;
}

const HCriterionWindowType& Habit::HCriterionSettings::getWindowType() const
{
    return *pwindowType_;
}

void Habit::HCriterionSettings::setWindowType(const HCriterionWindowType& windowType)
{
    pwindowType_ = &windowType;
}

void Habit::HCriterionSettings::setExcludeBasisWindow(bool b)
{
	m_bExcludeBasisWindow = b;
}

bool Habit::HCriterionSettings::getExcludeBasisWindow() const
{
	return m_bExcludeBasisWindow;
}

void Habit::HCriterionSettings::setRequireMinBasisValue(bool b)
{
	m_bRequireMinBasisValue = b;
}

bool Habit::HCriterionSettings::getRequireMinBasisValue() const
{
	return m_bRequireMinBasisValue;
}

void Habit::HCriterionSettings::setMinBasisValue(unsigned int v)
{
	m_uiMinBasisValue = v;
}

unsigned int Habit::HCriterionSettings::getMinBasisValue() const
{
	return m_uiMinBasisValue;
}



