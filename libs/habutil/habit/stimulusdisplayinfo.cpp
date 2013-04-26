#include "stimulusdisplayinfo.h"

#include <maindao.h>

Habit::StimulusDisplayInfo::StimulusDisplayInfo() 
: id_(-1)
, pstyle_(&HPresentationStyle::HPresentationStyleUnknown)
, pdtype_(&HDisplayType::HDisplayTypeFullScreen)
, isOriginalAspectRatioMaintained_(true)
, backGroundColor_(0, 0, 0, 255)
{
}

Habit::StimulusDisplayInfo::~StimulusDisplayInfo()
{
}


QDataStream & Habit::operator<< (QDataStream& stream, Habit::StimulusDisplayInfo d)
{
	stream << d.getId() << d.getPresentationStyle().number() << d.getDisplayType().number() << d.isOriginalAspectRatioMaintained() << d.getBackGroundColor();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::StimulusDisplayInfo& d)
{
	int id, istyle, itype;
	bool b;
	QColor color;
	stream >> id >> istyle >> itype >> b >> color;
	d.setId(id);
	d.setPresentationStyle(getPresentationStyle(istyle));
	d.setDisplayType(getDisplayType(itype));
	d.setMaintainOriginalAspectRatio(b);
	d.setBackGroundColor(color);
	return stream;
}

bool Habit::operator==(const Habit::StimulusDisplayInfo& lhs, const Habit::StimulusDisplayInfo& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getPresentationStyle() == rhs.getPresentationStyle() &&
			lhs.getDisplayType() == rhs.getDisplayType() &&
			lhs.isOriginalAspectRatioMaintained() == rhs.isOriginalAspectRatioMaintained() &&
			lhs.getBackGroundColor() == rhs.getBackGroundColor());
}


int Habit::StimulusDisplayInfo::getId() const {
	return id_;
}

void Habit::StimulusDisplayInfo::setId(int id) {
	id_ = id;
}

const HPresentationStyle& Habit::StimulusDisplayInfo::getPresentationStyle() const
{
    return *pstyle_;
}

void Habit::StimulusDisplayInfo::setPresentationStyle(const HPresentationStyle& style)
{
    pstyle_ = &style;
}

const HDisplayType& Habit::StimulusDisplayInfo::getDisplayType() const
{
    return *pdtype_;
}

void Habit::StimulusDisplayInfo::setDisplayType(const HDisplayType& dtype)
{
    pdtype_ = &dtype;
}

bool Habit::StimulusDisplayInfo::isOriginalAspectRatioMaintained() const
{
    return isOriginalAspectRatioMaintained_;
}

void Habit::StimulusDisplayInfo::setMaintainOriginalAspectRatio(bool maintainOriginalaspectRatio)
{
    isOriginalAspectRatioMaintained_ = maintainOriginalaspectRatio;
}

QColor Habit::StimulusDisplayInfo::getBackGroundColor() const
{
    return backGroundColor_;
}

void Habit::StimulusDisplayInfo::setBackGroundColor(const QColor& backGroundColor)
{
    backGroundColor_ = backGroundColor;
}

void Habit::StimulusDisplayInfo::loadFromDB( size_t id )
{
	Habit::MainDao dao;
	dao.getStimulusDisplayInfoForExperiment(id, this);
}

bool Habit::StimulusDisplayInfo::saveToDB(size_t id_)
{
	Habit::MainDao dao;
	return dao.addOrUpdateStimulusDisplaySetting(id_, this);
}
