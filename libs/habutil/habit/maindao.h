
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

#include <QtCore/QVector>
#include <QtCore/QStringList>

class ExperimentSettings;
class QString;
class QSqlQuery;

namespace Habit {

class MainDao
{
public:
	MainDao();
	~MainDao();
	void getExperimentSettingsByName(ExperimentSettings*);
	void getExperimentNameById(ExperimentSettings*);

	// does experiment with name exist? If it does, set id to experiment id from db.
	bool experimentExists(const QString& name, int& id);

	// does an experiment with id exist?
	bool experimentExists(int id);

	void insertExperiment(const QString& name);
	int getExperimentId(const QString& name);
	bool updateExperimentName(const QString& oldName, const QString& newName);
	QString getExperimentNameById(int id);
	ExperimentSettings getExperimentSettingsById(int id);
	QVector<ExperimentSettings> getAllExperiments();
	QStringList getAllExperimentNames(bool includeHidden = false);

	void getMonitorSettingsForExperiment(size_t id, MonitorSettings* monitorSettings);
	void getControlBarOptionsForExperiment(size_t id, ControlBarOptions* controlBarOptions);
	void getHabituationSettingsForExperiment( size_t id, HabituationSettings* habituationSettings);
	void getStimulusDisplayInfoForExperiment(size_t id, StimulusDisplayInfo* stimulusDisplayInfo);
	void getAttentionGetterSettingsForExperiment( size_t id, AttentionGetterSettings* attentionGetter);
	Habit::StimulusSettings getAttentionGetterStimulusSettings(int attentionGetterId);
	void getHLookSettingsForExperiment(int id, HLookSettings* lookSettings);
	void getHPhaseSettingsForExperiment(int id, int type, HPhaseSettings* phaseSettings);


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

	StimulusSettings getStimulusSettings(const QSqlQuery& q);
	bool deleteStimulus(const QString& table_name, int id);


	/*
	 * These are NEW methods for handling StimuliSettings, StimulusSettings
	 */

	StimuliSettings getStimuliSettings(int experiment_id, const HStimContext& context);
	bool getStimulusSettings(StimulusSettings& settings, int stimulus_id);
	bool deleteStimulus(int stimulus_id);
	bool addOrUpdateStimuliSettings(int experimentId, Habit::StimuliSettings& settings);
	bool addOrUpdateStimulusSettings(int experiment_id, Habit::StimulusSettings& ss);
	bool addOrUpdateStimulusInfo(int stimulus_id, const HPlayerPositionType& position, StimulusInfo& info);

	void getAttentionGetterSettings(int experimentId, AttentionGetterSettings* attentionGetter);
	bool addOrUpdateAttentionGetterSetting(int experimentId, Habit::AttentionGetterSettings* settings);


#if 0
	bool addOrUpdateStimuliSetting(size_t experimentId, Habit::StimuliSettings* settings);
	bool addOrUpdateStimulusSetting(int parentId, Habit::StimulusSettings& ss, const QString& tableName, const QString& parentKeyName);
#endif

	/* Get HStimulusOrder from the results of a query */
	HStimulusOrder getStimulusOrder(const QSqlQuery& q);

	/* Delete the StimulusOrder with given id */
	bool deleteOrder(int id);

	bool insertOrUpdateExperimentSettings(Habit::ExperimentSettings* experimentSettings);
	bool deleteExperimentSettings(Habit::ExperimentSettings* experimentSettings);
	bool addOrUpdateHLookSettings(int experimentId, Habit::HLookSettings* settings);
	bool addOrUpdateHPhaseSettings(int experimentId, Habit::HPhaseSettings* settings);
	bool addOrUpdateMonitorSetting(int experimentId, Habit::MonitorSettings* settings);
	bool addOrUpdateControlBarOption(size_t experimentId, Habit::ControlBarOptions* settings);
	bool addOrUpdateHabituationSetting(size_t experimentId, Habit::HabituationSettings* settings);


	bool addOrUpdateStimulusOrder(int parentId, Habit::HStimulusOrder& order);
	bool addOrUpdateStimulusDisplaySetting(size_t experimentId, Habit::StimulusDisplayInfo* settings);

#ifdef USE_SUBJECT_TABLES
	QStringList getAllSubjectsNames();
	void insertSubject(Habit::SubjectSettings*  subjectSettings_);
	void updateSubject(const Habit::SubjectSettings& subjectSettings);
	bool isSubjectUnique(const Habit::SubjectSettings& subjectSettings_);
	Habit::SubjectSettings getSubjectSettings(const QString& subj);
	Habit::SubjectSettings getSubjectSettings(int id);
	void insertRunSettings(Habit::RunSettings* runSettings);
	void updateRunSettings(const Habit::RunSettings& runSettings);
	Habit::RunSettings getRunSettingsBySubject(const Habit::SubjectSettings& subjectSettings);
	void addOrUpdateConfigObject(const Habit::ResultViewerSettings& config);
	Habit::ResultViewerSettings getResultViewerOptions();
#endif

private:
	bool deleteFromTable(const QString table, const QString key, int id);
};

} // namespace Habit 

#endif // HABIT_MAIN_DAO_H
