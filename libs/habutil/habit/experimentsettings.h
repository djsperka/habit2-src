
#ifndef HABIT_EXPERIMENT_SETTINGS_H
#define HABIT_EXPERIMENT_SETTINGS_H

/// \file experimentsettings.h
/// \brief Declaraction of class Habit::ExperimentSettings
#include <QString>
#include <QDataStream>
#include <QMap>
#include "monitorsettings.h"
#include "controlbaroptions.h"
#include "designsettings.h"
#include "habituationsettings.h"
#include "stimulusdisplayinfo.h"
#include "attentiongettersettings.h"
#include "stimulisettings.h"
#include "HLookSettings.h"
#include "HPhaseSettings.h"

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
    bool isHidden() const;
    void setHidden(bool bHidden);

    // make a copy of this experiment, with the given name and all database ids set to -1.
    ExperimentSettings clone(const QString& name);

    static bool load(const QString& filename, ExperimentSettings& settings);

    ControlBarOptions getControlBarOptions() const;
    void setControlBarOptions(const ControlBarOptions& controlBarOptions);
    HLookSettings getHLookSettings() const { return lookSettings_; };
    void setHLookSettings(const HLookSettings& lookSettings) { lookSettings_ = lookSettings; };
    HPhaseSettings getPreTestPhaseSettings() const { return pretestPhaseSettings_; };
    void setPreTestPhaseSettings(const HPhaseSettings& settings) { pretestPhaseSettings_ = settings; };
    HPhaseSettings getHabituationPhaseSettings() const { return habituationPhaseSettings_; };
    void setHabituationPhaseSettings(const HPhaseSettings& settings) { habituationPhaseSettings_ = settings; };
    HPhaseSettings getTestPhaseSettings() const { return testPhaseSettings_; };
    void setTestPhaseSettings(const HPhaseSettings& settings) { testPhaseSettings_ = settings; };

    HabituationSettings getHabituationSettings() const;
    void setHabituationSettings(const HabituationSettings& habituationSettings);
    StimulusDisplayInfo getStimulusDisplayInfo() const;
    void setStimulusDisplayInfo(const StimulusDisplayInfo& stimulusDisplayInfo);

    const AttentionGetterSettings& getAttentionGetterSettings() const;
    AttentionGetterSettings& getAttentionGetterSettings();
    void setAttentionGetterSettings(const AttentionGetterSettings& attentionGetterSettings);

    const StimuliSettings& getPreTestStimuliSettings() const;
    StimuliSettings& getPreTestStimuliSettings();
    void setPreTestStimuliSettings(const StimuliSettings& preTestStimuliSettings);

    const StimuliSettings& getHabituationStimuliSettings() const;
    StimuliSettings& getHabituationStimuliSettings();
    void setHabituationStimuliSettings(const StimuliSettings& habituationStimuliSettings);

    const StimuliSettings& getTestStimuliSettings() const;
    StimuliSettings& getTestStimuliSettings();
    void setTestStimuliSettings(const StimuliSettings& testStimuliSettings);

    void loadFromDB(bool byId = false);
	bool saveToDB();
	bool deleteFromDB();

	static bool load(ExperimentSettings& settings, int id);
	static bool load(ExperimentSettings& settings, const QString& name);

private:
	size_t id_;
	QString name_;
	int hidden_;	// 0 = visible; 1=hidden; this value not saved on I/O (saved in db)
// 11/10/2014 djs removed from experiment settings.	MonitorSettings monitorSettings_;
	ControlBarOptions controlBarOptions_;
	HabituationSettings habituationSettings_;
	StimulusDisplayInfo stimulusDisplayInfo_;
	AttentionGetterSettings attentionGetterSettings_;
	StimuliSettings pretestStimuliSettings_;
	StimuliSettings habituationStimuliSettings_;
	StimuliSettings testStimuliSettings_;	
	HLookSettings lookSettings_;
	HPhaseSettings pretestPhaseSettings_;
	HPhaseSettings habituationPhaseSettings_;
	HPhaseSettings testPhaseSettings_;
};


QDataStream & operator<< (QDataStream& stream, const ExperimentSettings& settings);
QDataStream & operator>> (QDataStream& stream, ExperimentSettings& settings);
bool operator==(const Habit::ExperimentSettings& lhs, const Habit::ExperimentSettings& rhs);

}  //namespace Habit

#endif // HABIT_EXPERIMENT_SETTINGS_H
