#include "stimulusdisplayinfo.h"

#include <maindao.h>

Habit::StimulusDisplayInfo::StimulusDisplayInfo() 
{
    presentationStyle_ = eMonitorDefined;
    displayType_ = eFullScreen;
    isOriginalAspectRatioMaintained_ = true;
    backGroundColor_ = QColor(0, 0, 0, 255);
	id_ = -1;
}

Habit::StimulusDisplayInfo::~StimulusDisplayInfo()
{
}

int Habit::StimulusDisplayInfo::getId() const {
	return id_;
}

void Habit::StimulusDisplayInfo::setId(int id) {
	id_ = id;
}

int Habit::StimulusDisplayInfo::getPresentationStyle() const
{
    return presentationStyle_;
}

void Habit::StimulusDisplayInfo::setPresentationStyle(int style)
{
    presentationStyle_ = style;
}

int Habit::StimulusDisplayInfo::getDisplayType() const
{
    return displayType_;
}

void Habit::StimulusDisplayInfo::setDisplayType(int displayType)
{
    displayType_ = displayType;
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