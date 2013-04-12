
#ifndef HABIT_EXPERIMENT_SETTINGS_H
#define HABIT_EXPERIMENT_SETTINGS_H

/// \file experimentsettings.h
/// \brief Declaraction of class Habit::ExperimentSettings
#include <QtCore/QString>
#include "monitorsettings.h"
#include "controlbaroptions.h"
#include "designsettings.h"
#include "habituationsettings.h"
#include "stimulusdisplayinfo.h"
#include "attentiongettersettings.h"
#include "stimulisettings.h"

/// Common namespace for all entities of the Habit
namespace Habit
{
 
/// Stores experiment settings.
class ExperimentSettings
{
public:
    ExperimentSettings();
    ~ExperimentSettings();

public:
	size_t getId() const;
	void setId(size_t id);
    QString getName() const;
    void setName(const QString& name);
    MonitorSettings getMonitorSettings() const;
    void setMonitorSettings(const MonitorSettings& monitorSettings);
    ControlBarOptions getControlBarOptions() const;
    void setControlBarOptions(const ControlBarOptions& controlBarOptions);
    DesignSettings getDesignSettings() const;
    void setDesignSettings(const DesignSettings& designSettings);
    HabituationSettings getHabituationSettings() const;
    void setHabituationSettings(const HabituationSettings& habituationSettings);
    StimulusDisplayInfo getStimulusDisplayInfo() const;
    void setStimulusDisplayInfo(const StimulusDisplayInfo& stimulusDisplayInfo);
    AttentionGetterSettings getAttentionGetterSettings() const;
    void setAttentionGetterSettings(const AttentionGetterSettings& attentionGetterSettings);
    StimuliSettings getPreTestStimuliSettings() const;
    void setPreTestStimuliSettings(const StimuliSettings& preTestStimuliSettings);
    StimuliSettings getHabituationStimuliSettings() const;
    void setHabituationStimuliSettings(const StimuliSettings& habituationStimuliSettings);
    StimuliSettings getTestStimuliSettings() const;
    void setTestStimuliSettings(const StimuliSettings& testStimuliSettings);
	void loadFromDB(bool byId = false);
	bool saveToDB();

private:
	size_t id_;
	QString name_;
	MonitorSettings monitorSettings_;
	ControlBarOptions controlBarOptions_;
	DesignSettings designSettings_;
	HabituationSettings habituationSettings_;
	StimulusDisplayInfo stimulusDisplayInfo_;
	AttentionGetterSettings attentionGetterSettings_;
	StimuliSettings pretestStimuliSettings_;
	StimuliSettings habituationStimuliSettings_;
	StimuliSettings testStimuliSettings_;	

};

}  //namespace Habit

#endif // HABIT_EXPERIMENT_SETTINGS_H
