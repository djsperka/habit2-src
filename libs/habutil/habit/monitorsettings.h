
#ifndef HABIT_MONITOR_SETTINGS_H
#define HABIT_MONITOR_SETTINGS_H

#include <stdlib.h>
#include <QDataStream>

/// \file monitorsettings.h
/// \brief Declaraction of class Habit::MonitorSettings

/// Common namespace for all entities of the Habit
namespace Habit
{
 
/// Stores monitors settings.
class MonitorSettings
{
public:
    MonitorSettings();
    ~MonitorSettings();

public:
	int getId() const;
	void setId(int);
    int getControlMonitorIndex() const;
    void setControlMonitorIndex(int index);
    int getLeftMonitorIndex() const;
    void setLeftMonitorIndex(int index);
    int getCenterMonitorIndex() const;
    void setCenterMonitorIndex(int index);
    int getRightMonitorIndex() const;
    void setRightMonitorIndex(int index);
	void loadFromDB(size_t id);
	bool saveToDB(size_t id_);

private:
	int id_;
	int controlMonitorIndex_;
	int leftMonitorIndex_;
	int centerMonitorIndex_;
	int rightMonitorIndex_;
};

QDataStream & operator<< (QDataStream& stream, MonitorSettings settings);
QDataStream & operator>> (QDataStream& stream, MonitorSettings& settings);
bool operator==(const Habit::MonitorSettings& lhs, const Habit::MonitorSettings& rhs);


} //namespace Habit

#endif // HABIT_MONITOR_SETTINGS_H
