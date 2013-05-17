#include "monitorsettings.h"
#include "maindao.h"

Habit::MonitorSettings::MonitorSettings() 
{
	id_ = -1;
	controlMonitorIndex_ = -1;
    leftMonitorIndex_ = -1;
    centerMonitorIndex_ = -1;
    rightMonitorIndex_ = -1;
}

Habit::MonitorSettings::~MonitorSettings()
{
}

int Habit::MonitorSettings::getId() const {
	return id_;
}

void Habit::MonitorSettings::setId(int id) {
	id_ = id;
}

int Habit::MonitorSettings::getControlMonitorIndex() const
{
    return controlMonitorIndex_;
}

void Habit::MonitorSettings::setControlMonitorIndex(int index)
{
	if (index<-1) {
		index = -1;
	}
    controlMonitorIndex_ = index;
}

int Habit::MonitorSettings::getLeftMonitorIndex() const
{
    return leftMonitorIndex_;
}
void Habit::MonitorSettings::setLeftMonitorIndex(int index)
{
	if (index<-1) {
		index = -1;
	}
    leftMonitorIndex_ = index;
}

int Habit::MonitorSettings::getCenterMonitorIndex() const
{
    return centerMonitorIndex_;
}
void
Habit::MonitorSettings::setCenterMonitorIndex(int index)
{
	if (index<-1) {
		index = -1;
	}
    centerMonitorIndex_ = index;
}

int Habit::MonitorSettings::getRightMonitorIndex() const
{
    return rightMonitorIndex_;
}
void Habit::MonitorSettings::setRightMonitorIndex(int index)
{
	if (index<-1) {
		index = -1;
	}
    rightMonitorIndex_ = index;
}

void Habit::MonitorSettings::loadFromDB(size_t id) 
{
	Habit::MainDao dao;
	dao.getMonitorSettingsForExperiment(id, this);
}

bool Habit::MonitorSettings::saveToDB(size_t id_) {
	Habit::MainDao dao;
	return dao.addOrUpdateMonitorSetting(id_, this);
}


QDataStream& Habit::operator<< (QDataStream& stream, Habit::MonitorSettings settings)
{
	stream << settings.getId() << settings.getControlMonitorIndex() << settings.getLeftMonitorIndex() << settings.getCenterMonitorIndex() << settings.getRightMonitorIndex();
	return stream;
}

QDataStream& Habit::operator>> (QDataStream& stream, Habit::MonitorSettings& settings)
{
	int id, control, left, center, right;
	stream >> id >> control >> left >> center >> right;
	settings.setId(id);
	settings.setControlMonitorIndex(control);
	settings.setLeftMonitorIndex(left);
	settings.setCenterMonitorIndex(center);
	settings.setRightMonitorIndex(right);
	return stream;
}

bool Habit::operator==(const Habit::MonitorSettings& lhs, const Habit::MonitorSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getControlMonitorIndex() == rhs.getControlMonitorIndex() &&
			lhs.getLeftMonitorIndex() == rhs.getLeftMonitorIndex() &&
			lhs.getCenterMonitorIndex() == rhs.getCenterMonitorIndex() &&
			lhs.getRightMonitorIndex() == rhs.getRightMonitorIndex());
}

