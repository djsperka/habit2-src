
#ifndef HABIT_STIMULUS_DISPLAY_INFO_H
#define HABIT_STIMULUS_DISPLAY_INFO_H

/// \file stimulusdisplayinfo.h
/// \brief Declaraction of class Habit::StimulusDisplayInfo

#include "HTypes.h"
#include <QColor>
#include <QMetaType>

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
    StimulusDisplayInfo clone() const;

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
	void loadFromDB(int experimentID);	// can throw HDBException
	void saveToDB(int experimentID);	// can throw HDBException

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
QDebug operator<<(QDebug dbg, const Habit::StimulusDisplayInfo& info);
bool operator==(const Habit::StimulusDisplayInfo& lhs, const Habit::StimulusDisplayInfo& rhs);


} //namespace Habit

Q_DECLARE_METATYPE(Habit::StimulusDisplayInfo);

#endif // HABIT_STIMULUS_DISPLAY_INFO_H
