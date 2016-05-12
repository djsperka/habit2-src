
#ifndef HABIT_EXPERIMENT_SETTINGS_H
#define HABIT_EXPERIMENT_SETTINGS_H

/// \file experimentsettings.h
/// \brief Declaraction of class Habit::ExperimentSettings
#include <QString>
#include <QDataStream>
#include <QMap>
#include <QList>
#include <QListIterator>
#include "monitorsettings.h"
#include "controlbaroptions.h"
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
	int getId() const;
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
    HLookSettings getHLookSettings() const { return m_lookSettings; };
    void setHLookSettings(const HLookSettings& lookSettings) { m_lookSettings = lookSettings; };

    StimulusDisplayInfo getStimulusDisplayInfo() const;
    void setStimulusDisplayInfo(const StimulusDisplayInfo& stimulusDisplayInfo);

    const AttentionGetterSettings& getAttentionGetterSettings() const;
    AttentionGetterSettings& getAttentionGetterSettings();
    void setAttentionGetterSettings(const AttentionGetterSettings& attentionGetterSettings);

    //void loadFromDB(bool byId = false);
    bool loadFromDB(int experimentID);
    bool loadFromDB(const QString& name);

    bool loadPhasesFromDB(int experimentID);

    bool saveToDB();
	bool deleteFromDB();

	static bool load(ExperimentSettings& settings, int id);
	static bool load(ExperimentSettings& settings, const QString& name);

	// Add a phase to this experiment
	void appendPhase(const HPhaseSettings& phase) { m_phases.append(phase); return; };

	const QList<HPhaseSettings>& phases() const { return m_phases; };
	void setPhases(const QList<HPhaseSettings>& phases) { m_phases = phases; };

	// return true if phase with this name exists
	int phaseExists(const QString& name) const;
	int phaseExists(int seqno) const;

	QListIterator<HPhaseSettings> phaseIterator() const{ return QListIterator<HPhaseSettings>(m_phases); };

	// just like QList.at(int)
	const HPhaseSettings& phaseAt(int index) const;
	const HPhaseSettings& phaseAt(const QString& name) const;

	// just like QList::value(int)
	HPhaseSettings phaseValue(int index);
	HPhaseSettings phaseValue(const QString& name);

	// number of phases
	int getNumberOfPhases() const { return m_phases.size(); };


private:
	int id_;
	QString name_;
	int hidden_;	// 0 = visible; 1=hidden; this value not saved on I/O (saved in db)
	ControlBarOptions m_controlBarOptions;
	StimulusDisplayInfo m_stimulusDisplayInfo;
	AttentionGetterSettings m_attentionGetterSettings;
	HLookSettings m_lookSettings;
	QList<HPhaseSettings> m_phases;
};


QDataStream & operator<< (QDataStream& stream, const ExperimentSettings& settings);
QDataStream & operator>> (QDataStream& stream, ExperimentSettings& settings);
bool operator==(const Habit::ExperimentSettings& lhs, const Habit::ExperimentSettings& rhs);

}  //namespace Habit

#endif // HABIT_EXPERIMENT_SETTINGS_H
