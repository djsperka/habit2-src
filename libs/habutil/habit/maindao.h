
#ifndef HABIT_MAIN_DAO_H
#define HABIT_MAIN_DAO_H

#include "configuration.h"
#include "experimentsettings.h"
#include "monitorsettings.h"
#include "controlbaroptions.h"
#include "designsettings.h"
#include "habituationsettings.h"
#include "stimulusdisplayinfo.h"
#include "attentiongettersettings.h"
#include "stimulisettings.h"
#include "stimulussettings.h"
#include "subjectsettings.h"
#include "runsettings.h"
#include "resultviewersettings.h"
#include "HLookSettings.h"

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
	int isExperimentExists(const QString& name);
	void insertExperiment(const QString& name);
	void updateExperiment(const QString& name);
	QString getExperimentNameById(int id);
	ExperimentSettings getExperimentSettingsById(int id);
	QVector<ExperimentSettings> getAllExperiments();
	QStringList getAllExperimentNames();

	void getMonitorSettingsForExperiment(size_t id, MonitorSettings* monitorSettings);
	void getControlBarOptionsForExperiment(size_t id, ControlBarOptions* controlBarOptions);
	void getHabituationSettingsForExperiment( size_t id, HabituationSettings* habituationSettings);
	void getStimulusDisplayInfoForExperiment(size_t id, StimulusDisplayInfo* stimulusDisplayInfo);
	void getAttentionGetterSettingsForExperiment( size_t id, AttentionGetterSettings* attentionGetter);
	Habit::StimulusSettings getAttentionGetterStimulusSettings(int attentionGetterId);
	void getHLookSettingsForExperiment(int id, HLookSettings* lookSettings);
	void getHPhaseSettingsForExperiment(int id, int type, HPhaseSettings* phaseSettings);

	StimuliSettings getPretestStimuliSettingsForExperiment(const QString& experiment);
	StimuliSettings getHabituationStimuliSettingsForExperiment(const QString& experiment);
	StimuliSettings getTestStimuliSettingsForExperiment(const QString& experiment);
	StimuliSettings getPretestStimuliSettingsForExperiment(int experiment);
	StimuliSettings getHabituationStimuliSettingsForExperiment(int experiment);
	StimuliSettings getTestStimuliSettingsForExperiment(int experiment);
	StimuliSettings getStimuliSettings(const QString& table_name, const QString& experiment, const HStimContext& context);
	StimulusSettings getStimulusSettings(const QSqlQuery& q);

	void deleteStimulus(const QString& table_name, int id);
	void deleteAllStimulus(const QString& table_name, int experiment_id);
	bool insertOrUpdateExperimentSettings(Habit::ExperimentSettings* experimentSettings);
	bool addOrUpdateHLookSettings(int experimentId, Habit::HLookSettings* settings);
	bool addOrUpdateHPhaseSettings(int experimentId, const Habit::HPhaseSettings* settings);
	bool addOrUpdateMonitorSetting(int experimentId, Habit::MonitorSettings* settings);
	bool addOrUpdateAttentionGetterSetting(size_t experimentId, Habit::AttentionGetterSettings* settings);
	bool addOrUpdateControlBarOption(size_t experimentId, Habit::ControlBarOptions* settings);
	bool addOrUpdateHabituationSetting(size_t experimentId, Habit::HabituationSettings* settings);
	bool addOrUpdateStimuliSetting(size_t experimentId, Habit::StimuliSettings* settings);
	bool addOrUpdateStimulusSetting(int id, int parentId, Habit::StimulusSettings& ss, const QString& tableName, const QString& parentKeyName);
	bool addOrUpdateStimulusDisplaySetting(size_t experimentId, Habit::StimulusDisplayInfo* settings);

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
};

} // namespace Habit 

#endif // HABIT_MAIN_DAO_H
