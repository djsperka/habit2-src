
#ifndef HABIT_MAIN_DAO_H
#define HABIT_MAIN_DAO_H

//#include "configuration.h"
#include "experimentsettings.h"
#include "monitorsettings.h"
#include "controlbaroptions.h"
#include "habituationsettings.h"
#include "stimulusdisplayinfo.h"
#include "attentiongettersettings.h"
#include "stimulisettings.h"
#include "stimulussettings.h"
#include "subjectsettings.h"
#include "runsettings.h"
//#include "resultviewersettings.h"
#include "HLookSettings.h"
#include "HStimulusOrder.h"
#include "HDBException.h"

#include <QtCore/QVector>
#include <QtCore/QStringList>

class ExperimentSettings;
class QString;
class QSqlQuery;
class QSqlRecord;

namespace Habit {

class MainDao
{
public:
	MainDao();
	~MainDao();

	// does experiment with name exist? If it does, set id to experiment id from db.
	bool experimentExists(const QString& name, int& id);

	// does an experiment with id exist?
	bool experimentExists(int id);

	void updateExperimentName(const QString& oldName, const QString& newName);
	QStringList getExperimentNames(bool includeExperiments = true, bool includeTemplates = false);


	// djs Clean up interface for fetching experiments. These return false when the id/name not found.
	void getExperimentName(int id, QString& name);			// can throw HDBException
	void getExperimentID(const QString& name, int& id);		// can throw HDBException


	void getControlBarOptionsForExperiment(int expID, ControlBarOptions& controlBarOptions);	// can throw HDBException
	void getHabituationSettingsForPhase(int phaseId, HabituationSettings& habituationSettings);
	void getStimulusDisplayInfoForExperiment(int experimentID, StimulusDisplayInfo& stimulusDisplayInfo);
	Habit::StimulusSettings getAttentionGetterStimulusSettings(int attentionGetterId);
	void getHLookSettingsForExperiment(int experimentID, HLookSettings& lookSettings);
	void getHPhaseSettingsIDs(int experimentID, QList<int>& ids);
	void getHPhaseSettings(int phaseId, HPhaseSettings& phaseSettings);
	QString getHPhaseName(int phaseId);

	/*
	 * Get a bunch of ids
	 */
	QList<QVariant> getColumnValuesFromTable(const QString& tableName, const QString& column, const QString& key = QString(), const QVariant& keyValue = QVariant());
	void getListFromTable(const QString& table, const QString& key, int id, const QString& column, QList<int>& list);

	/*
	 * These are OLD methods for handling StimuliSettings, StimulusSettings
	 */

	StimuliSettings getPretestStimuliSettingsForExperiment(const QString& experiment);
	StimuliSettings getHabituationStimuliSettingsForExperiment(const QString& experiment);
	StimuliSettings getTestStimuliSettingsForExperiment(const QString& experiment);
	StimuliSettings getPretestStimuliSettingsForExperiment(int experiment);
	StimuliSettings getHabituationStimuliSettingsForExperiment(int experiment);
	StimuliSettings getTestStimuliSettingsForExperiment(int experiment);
	StimuliSettings getStimuliSettings(const QString& table_name, int experiment_id, const HStimContext& context);
	StimuliSettings getStimuliSettings(const QString& table_name, const QString& experiment, const HStimContext& context);

	/*
	 * These are NEW methods for handling StimuliSettings, StimulusSettings
	 */

	void getStimuliSettings(int phase_id, StimuliSettings& settings);

	// will set Id and Name in this method. Previous value(s) are overwritten.
	void getStimulusSettings(int stimulus_id, StimulusSettings& settings);
	void addOrUpdateStimuliSettings(int phase_id, Habit::StimuliSettings& settings);
	void addOrUpdateStimulusSettings(int phaseID, Habit::StimulusSettings& ss);
	void addOrUpdateStimulusInfo(int stimulus_id, const HPlayerPositionType& position, StimulusInfo& info);

	void getAttentionGetterSettings(int experimentId, AttentionGetterSettings& attentionGetterSettings);	// can throw HDBException
	void addOrUpdateAttentionGetterSettings(int experimentId, Habit::AttentionGetterSettings& settings);		// can throw HDBException

	/* Get HStimulusOrder from the results of a query */
	HStimulusOrder getStimulusOrder(const QSqlQuery& q);
	void getStimulusOrderFromRecord(const QSqlRecord& r, Habit::HStimulusOrder& order);

	/* Delete stuff */
	void deleteStimulus(int stimulus_id);	// can throw HDBException
	void deleteOrder(int id);	// can throw HDBException
	void deleteExperiment(Habit::ExperimentSettings& experimentSettings);
	void deletePhase(int id);
	void deleteFromTable(const QString table, const QString key, int id);
	void deleteFromTable(const QString table, const QString key, const QList<int>& ids);

	void addOrUpdateExperimentSettings(Habit::ExperimentSettings& experimentSettings);	// can throw HDBException
	void addOrUpdateHLookSettings(int experimentID, Habit::HLookSettings& settings);
	void addOrUpdateHPhaseSettings(int experimentId, Habit::HPhaseSettings& phaseSettings);
	void addOrUpdateControlBarOptions(int experimentId, Habit::ControlBarOptions& controlBarOptions);	// can throw HDBException
	void addOrUpdateHabituationSettings(int phaseID, Habit::HabituationSettings& habituationSettings);
	void addOrUpdateStimulusOrder(int parentId, Habit::HStimulusOrder& order);
	void addOrUpdateStimulusDisplayInfo(int experimentId, Habit::StimulusDisplayInfo& stimulusDisplayInfo);
};

} // namespace Habit 

#endif // HABIT_MAIN_DAO_H
