
#ifndef HABIT_STIMULUS_DISPLAY_INFO_H
#define HABIT_STIMULUS_DISPLAY_INFO_H

/// \file stimulusdisplayinfo.h
/// \brief Declaraction of class Habit::StimulusDisplayInfo

#include <QtGui/QColor>

/// Common namespace for all entities of the Habit
namespace Habit
{

/// Stores information used for stimulus display.
class StimulusDisplayInfo
{
public:
    StimulusDisplayInfo();
    ~StimulusDisplayInfo();

public:
    enum PresentationStyle {
        eMonitorDefined = 0,
        eSingleMovable
    };
    enum DisplayType {
        eFullScreen = 0,
        eOriginalSize
    };

	int getId() const;
	void setId(int id);
    int getPresentationStyle() const;
    void setPresentationStyle(int style);
    int getDisplayType() const;
    void setDisplayType(int displayType);
    bool isOriginalAspectRatioMaintained() const;
    void setMaintainOriginalAspectRatio(bool maintainOriginalaspectRatio);
    QColor getBackGroundColor() const;
    void setBackGroundColor(const QColor& backGroundColor);
	void loadFromDB(size_t id);
	bool saveToDB(size_t id_);

private:
	int id_;
	int presentationStyle_;
	int displayType_;
	bool isOriginalAspectRatioMaintained_;
	QColor backGroundColor_;
};

} //namespace Habit

#endif // HABIT_STIMULUS_DISPLAY_INFO_H
