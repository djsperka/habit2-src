
#ifndef HABIT_STIMULUS_DISPLAY_INFO_H
#define HABIT_STIMULUS_DISPLAY_INFO_H

/// \file stimulusdisplayinfo.h
/// \brief Declaraction of class Habit::StimulusDisplayInfo

#include <QtGui/QColor>
#include "HTypes.h"

/// Common namespace for all entities of the Habit
namespace Habit
{

/// Stores information used for stimulus display.
class StimulusDisplayInfo
{
public:
    StimulusDisplayInfo();
    StimulusDisplayInfo(const StimulusDisplayInfo& info);
    ~StimulusDisplayInfo();
    StimulusDisplayInfo& operator=(const StimulusDisplayInfo& rhs);
    StimulusDisplayInfo clone();

public:
	int getId() const;
	void setId(int id);
    const HPresentationStyle& getPresentationStyle() const;
    void setPresentationStyle(const HPresentationStyle& style);
    const HDisplayType& getDisplayType() const;
    void setDisplayType(const HDisplayType& type);
    bool isOriginalAspectRatioMaintained() const;
    void setMaintainOriginalAspectRatio(bool maintainOriginalaspectRatio);
    QColor getBackGroundColor() const;
    void setBackGroundColor(const QColor& backGroundColor);
    const HStimulusLayoutType& getStimulusLayoutType() const;
    void setStimulusLayoutType(const HStimulusLayoutType& type);
    bool getUseISS() const;
    void setUseISS(bool use);
	void loadFromDB(size_t id);
	bool saveToDB(size_t id_);

private:
	int id_;
	const HPresentationStyle* pstyle_;
	const HDisplayType* pdtype_;
	bool isOriginalAspectRatioMaintained_;
	QColor backGroundColor_;
	const HStimulusLayoutType* playouttype_;
	bool useISS_;
};

QDataStream & operator<< (QDataStream& stream, Habit::StimulusDisplayInfo d);
QDataStream & operator>> (QDataStream& stream, Habit::StimulusDisplayInfo& d);
bool operator==(const Habit::StimulusDisplayInfo& lhs, const Habit::StimulusDisplayInfo& rhs);


} //namespace Habit

#endif // HABIT_STIMULUS_DISPLAY_INFO_H
