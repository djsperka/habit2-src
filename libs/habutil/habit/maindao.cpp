
#include "maindao.h"
#include "experimentsettings.h"
#include "connection.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QtSql/QSqlError>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtCore/QTDebug>

namespace Habit {

MainDao::MainDao()
{}

MainDao::~MainDao()
{}


// Settings Add/Update methods

bool MainDao::addOrUpdateMonitorSetting(size_t experimentId, Habit::MonitorSettings* settings) {
	Habit::connection* con = Habit::connection::get_instance();
	Q_ASSERT(0 != con);
	int id = settings->getId();
	QString sql;
	if(id > 0) {
		sql = "update monitor_settings set control_monitor=?, left_monitor=?, center_monitor=?, right_monitor=?"
			" where id=? and experiment_id=?";
	} else {
		sql = "insert into monitor_settings (control_monitor, left_monitor, center_monitor, right_monitor, experiment_id)"
			" values(?, ?, ?, ?, ?)";
	}
	QSqlQuery q = con->exec(sql);
	q.addBindValue(settings->getControlMonitorIndex());
	q.addBindValue(settings->getLeftMonitorIndex());
	q.addBindValue(settings->getCenterMonitorIndex());
	q.addBindValue(settings->getRightMonitorIndex());
	if(id > 0) {
		q.addBindValue(id);
	}
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	return !q.lastError().isValid();
}

bool MainDao::addOrUpdateAttentionGetterSetting(size_t experimentId, Habit::AttentionGetterSettings* settings) {
	Habit::connection* con = Habit::connection::get_instance();
	Q_ASSERT(0 != con);
	int id = settings->getId();
	QString sql;
	if(id > 0) {
		sql = "update attention_setup set use_attention_stimulus=?, stimulus_name=?, background_color=?"
			" where id=? and experiment_id=?";
	} else {
		sql = "insert into attention_setup (use_attention_stimulus, stimulus_name, background_color, experiment_id)"
			" values(?, ?, ?, ?)";
	}
	QSqlQuery q = con->exec(sql);
	q.addBindValue(settings->isAttentionGetterUsed());
	q.addBindValue(settings->getAttentionGetterStimulus().getName());
	q.addBindValue(settings->getBackGroundColor().name());
	if(id > 0) {
		q.addBindValue(id);
	}
	q.addBindValue((uint)experimentId);
	q.exec();
	if(id < 0) {
		id = q.lastInsertId().toInt();
	}
	qDebug() << q.lastError();
	Habit::StimulusSettings ss = settings->getAttentionGetterStimulus();
	return !q.lastError().isValid() && addOrUpdateStimulusSetting(ss.getId(), id, ss, "attention_getting_stimuli", "attention_getter_id");
}

bool MainDao::addOrUpdateControlBarOption(size_t experimentId, Habit::ControlBarOptions* settings) {
	Habit::connection* con = Habit::connection::get_instance();
	Q_ASSERT(0 != con);
	int id = settings->getId();
	QString sql;
	if(id > 0) {
		sql = "update controlbar_options set is_used=?, display_current_experiment=?, display_current_stimulus=?"
			" where id=? and experiment_id=?";
	} else {
		sql = "insert into controlbar_options (is_used, display_current_experiment, display_current_stimulus, experiment_id)"
			" values(?, ?, ?, ?)";
	}
	QSqlQuery q = con->exec(sql);
	q.addBindValue(settings->isControlBarUsed());
	q.addBindValue(settings->isCurrentExperimentDisplayed());
	q.addBindValue(settings->isCurrentStimulusDisplayed());
	if(id > 0) {
		q.addBindValue(id);
	}
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	return !q.lastError().isValid();
}

bool MainDao::addOrUpdateDesignSetting(size_t experimentId, Habit::DesignSettings* settings) {
	Habit::connection* con = Habit::connection::get_instance();
	Q_ASSERT(0 != con);
	int id = settings->getId();
	QString sql;
	if(id > 0) {
		sql = "update designs_settings set pretest_trial_type=?, habituation_trial_type=?, test_trial_type=?, pretest_trial_length=?, "
			" habituation_trial_length=?, test_trial_length=?, minimum_look_time=?, minimum_look_away_time=?, minimum_no_look_time=?, "
			" pretest_number_of_trials=?, habituation_number_of_trials=?, test_number_of_trials=?"
			" where id=? and experiment_id=?";
	} else {
		sql = "insert into designs_settings (pretest_trial_type, habituation_trial_type, test_trial_type, pretest_trial_length, "
			" habituation_trial_length, test_trial_length, minimum_look_time, minimum_look_away_time, minimum_no_look_time, "
			" pretest_number_of_trials, habituation_number_of_trials, test_number_of_trials, experiment_id)"
			" values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	}
	QSqlQuery q = con->exec(sql);
	Habit::TrialsInfo ht = settings->getHabituationTrialsInfo();
	Habit::TrialsInfo pt = settings->getPretestTrialsInfo();
	Habit::TrialsInfo tt = settings->getTestTrialsInfo();
	q.addBindValue(pt.getType());
	q.addBindValue(ht.getType());
	q.addBindValue(tt.getType());
	q.addBindValue(pt.getLength());
	q.addBindValue(ht.getLength());
	q.addBindValue(tt.getLength());
	q.addBindValue(pt.getLookTimes());
	q.addBindValue(ht.getLookTimes());
	q.addBindValue(tt.getLookTimes());
	q.addBindValue(pt.getNumberOfTrials());
	q.addBindValue(ht.getNumberOfTrials());
	q.addBindValue(tt.getNumberOfTrials());
	if(id > 0) {
		q.addBindValue(id);
	}
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	return !q.lastError().isValid();
}

bool MainDao::addOrUpdateHabituationSetting(size_t experimentId, Habit::HabituationSettings* settings) {
	Habit::connection* con = Habit::connection::get_instance();
	Q_ASSERT(0 != con);
	int id = settings->getId();
	QString sql;
	if(id > 0) {
		sql = "update habituation_settings set habituation_type=?, criterion_basis=?, criterion_percent=?, window_size=?, window_type=?, total_look=?"
			" where id=? and experiment_id=?";
	} else {
		sql = "insert into habituation_settings (habituation_type, criterion_basis, criterion_percent, window_size, window_type, total_look, experiment_id)"
			" values(?, ?, ?, ?, ?, ?, ?)";
	}
	QSqlQuery q = con->exec(sql);
	q.addBindValue(settings->getHabituationType().number());
	Habit::CriterionSettings cs = settings->getCriterionSettings();
	q.addBindValue(cs.getBasis().number());
	q.addBindValue(cs.getPercent());
	q.addBindValue(cs.getWindowSize());
	q.addBindValue(cs.getWindowType().number());
	q.addBindValue(settings->getTotalLookLengthToEnd());
	if(id > 0) {
		q.addBindValue(id);
	}
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	return !q.lastError().isValid();
}

void MainDao::deleteAllStimulus(const QString& table_name, int experiment_id) {
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "delete from " + table_name + " where experiment_id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(experiment_id);
	q.exec();
}

void MainDao::deleteStimulus(const QString& table_name, int id) {
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "delete from " + table_name + " where id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(id);
	q.exec();
}

bool MainDao::addOrUpdateStimuliSetting(size_t experiment_id, Habit::StimuliSettings* settings) {
	QString table_name;
	if (settings->getStimContext() == HStimContext::PreTestPhase)
	{
		table_name = "pretest_stimuli";
	}
	else if (settings->getStimContext() == HStimContext::HabituationPhase)
	{
		table_name = "habituation_stimuli";
	}
	else if (settings->getStimContext() == HStimContext::TestPhase)
	{
		table_name = "test_stimuli";
	}

	bool result = true;
	if(!table_name.isEmpty()) {
		typedef Habit::StimuliSettings::stimulus_container st_list;
		Habit::StimuliSettings::stimulus_container stimulus = settings->getStimuli();
		for(st_list::iterator it = stimulus.begin(); it != stimulus.end() && result; ++it) {
			Habit::StimulusSettings ss = *it;
			int id = ss.getId();
			result = result && addOrUpdateStimulusSetting(id, experiment_id, ss, table_name, "experiment_id");
		}
	} else {
		result = false;
	}
	return result;
}

bool MainDao::addOrUpdateStimulusSetting(int id, int parentId, Habit::StimulusSettings& ss, const QString& tableName, const QString& parentKeyName) {
	QString sql;
	if(id > 0) {
		sql = "update " + tableName + " set name=?, is_left_enabled=?, left_filename=?, is_left_loop_playback=?, is_center_enabled=?, "
			" center_filename=?, is_center_loop_playback=?, is_right_enabled=?, right_filename=?, is_right_loop_playback=?, is_independent_enabled=?, "
			" independent_sound_filename=?, is_independent_loop_playback=?, left_audio_balance=?, right_audio_balance=?, center_audio_balance=?,"
			" independent_audio_balance=?"
			" where id=? and " + parentKeyName + "=?";
	} else {
		sql = "insert into " + tableName + " (name, is_left_enabled, left_filename, is_left_loop_playback, is_center_enabled, "
			" center_filename, is_center_loop_playback, is_right_enabled, right_filename, is_right_loop_playback, is_independent_enabled, "
			" independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, "
			" independent_audio_balance, " + parentKeyName + ")"
			" values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	}
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QSqlQuery q = con->exec(sql);
	q.addBindValue(ss.getName());
	q.addBindValue(ss.isLeftEnabled());
	q.addBindValue(ss.getLeftStimulusInfo().getFileName());
	q.addBindValue(ss.getLeftStimulusInfo().isLoopPlayBack());
	q.addBindValue(ss.isCenterEnabled());
	q.addBindValue(ss.getCenterStimulusInfo().getFileName());
	q.addBindValue(ss.getCenterStimulusInfo().isLoopPlayBack());
	q.addBindValue(ss.isRightEnabled());
	q.addBindValue(ss.getRightStimulusInfo().getFileName());
	q.addBindValue(ss.getRightStimulusInfo().isLoopPlayBack());
	q.addBindValue(ss.isIndependentSoundEnabled());
	q.addBindValue(ss.getIndependentSoundInfo().getFileName());
	q.addBindValue(ss.getIndependentSoundInfo().isLoopPlayBack());
	q.addBindValue(ss.getLeftStimulusInfo().getAudioBalance());
	q.addBindValue(ss.getRightStimulusInfo().getAudioBalance());
	q.addBindValue(ss.getCenterStimulusInfo().getAudioBalance());
	q.addBindValue(ss.getIndependentSoundInfo().getAudioBalance());
	if(id > 0) {
		q.addBindValue(id);
	}
	q.addBindValue(parentId);
	q.exec();
	qDebug() << q.lastError();
	return !q.lastError().isValid();
}

bool MainDao::addOrUpdateStimulusDisplaySetting(size_t experimentId, Habit::StimulusDisplayInfo* settings) {
	Habit::connection* con = Habit::connection::get_instance();
	Q_ASSERT(0 != con);
	int id = settings->getId();
	QString sql;
	if(id > 0) {
		sql = "update stimulus_display set presentation_style=?, display_type=?, is_original_aspect_ratio=?, background_color=?"
			" where id=? and experiment_id=?";
	} else {
		sql = "insert into stimulus_display (presentation_style, display_type, is_original_aspect_ratio, background_color, experiment_id)"
			" values(?, ?, ?, ?, ?)";
	}
	QSqlQuery q = con->exec(sql);
	q.addBindValue(settings->getPresentationStyle());
	q.addBindValue(settings->getDisplayType());
	q.addBindValue(settings->isOriginalAspectRatioMaintained());
	q.addBindValue(settings->getBackGroundColor());
	if(id > 0) {
		q.addBindValue(settings->getId());
	}
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	return !q.lastError().isValid();
}





// Retrieve Data from DB

void MainDao::getMonitorSettingsForExperiment(size_t experimentId, MonitorSettings* monitorSettings)
{
	Q_ASSERT(0 != monitorSettings);
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from monitor_settings where experiment_id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		int id = q.value(q.record().indexOf("id")).toInt();
		monitorSettings->setId(id);
		int controlMonitor = q.value(q.record().indexOf("control_monitor")).toInt();
		monitorSettings->setControlMonitorIndex(controlMonitor);
		int leftMonitor= q.value(q.record().indexOf("left_monitor")).toInt();
		monitorSettings->setLeftMonitorIndex(leftMonitor);
		int centerMonitor = q.value(q.record().indexOf("center_monitor")).toInt();
		monitorSettings->setCenterMonitorIndex(centerMonitor);
		int rightMonitor = q.value(q.record().indexOf("right_monitor")).toInt();
		monitorSettings->setRightMonitorIndex(rightMonitor);
	}
}

void MainDao::getAttentionGetterSettingsForExperiment(size_t experimentId, AttentionGetterSettings* attentionGetter)
{
	Q_ASSERT(0 != attentionGetter);
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from attention_setup where experiment_id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		int id = q.value(q.record().indexOf("id")).toInt();
		attentionGetter->setId(id);
		size_t useAttentionStimulus = q.value(q.record().indexOf("use_attention_stimulus")).toBool();
		attentionGetter->setUseAttentionGetter(useAttentionStimulus);
		QString backgroundColor = q.value(q.record().indexOf("background_color")).toString();
		attentionGetter->setBackGroundColor(QColor(backgroundColor));
		sql = "select * from attention_getting_stimuli where attention_getter_id = ?";
		q = con->exec(sql);
		q.addBindValue(id);
		q.exec();
		qDebug() << q.lastError();
		if(q.next()) {
			Habit::StimulusSettings ss = getStimulusSettings(q);
			attentionGetter->setAttentionGetterStimulus(ss);
		}
	}
}

void MainDao::getControlBarOptionsForExperiment(size_t experimentId, ControlBarOptions* controlBarOptions)
{
	Q_ASSERT(0 != controlBarOptions);
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from controlbar_options where experiment_id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		int id = q.value(q.record().indexOf("id")).toInt();
		controlBarOptions->setId(id);
		size_t useControlBar = q.value(q.record().indexOf("is_used")).toBool();
		controlBarOptions->setUseControlBar(useControlBar);
		size_t displayCurrentExperiment = q.value(q.record().indexOf("display_current_experiment")).toBool();
		controlBarOptions->setDisplayCurrentExperiment(displayCurrentExperiment);
		size_t displayCurrentStimulus = q.value(q.record().indexOf("display_current_stimulus")).toBool();
		controlBarOptions->setDisplayCurrentStimulus(displayCurrentStimulus);
	}
}

void MainDao::getDesignSettingsForExperiment(size_t experimentId, DesignSettings* designSettings)
{
	Q_ASSERT(0 != designSettings);
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from designs_settings where experiment_id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		int id = q.value(q.record().indexOf("id")).toInt();
		designSettings->setId(id);

		TrialsInfo habituationTrials;
		int habituationTrialType = q.value(q.record().indexOf("habituation_trial_type")).toInt();
		int habituationTrialLength = q.value(q.record().indexOf("habituation_trial_length")).toInt();
		int habituationLookTimes = q.value(q.record().indexOf("minimum_look_away_time")).toInt();
		int habituationNumberOfTrials = q.value(q.record().indexOf("habituation_number_of_trials")).toInt();
		habituationTrials.setType(habituationTrialType);
		habituationTrials.setLength(habituationTrialLength);
		habituationTrials.setLookTimes(habituationLookTimes);
		habituationTrials.setNumberOfTrials(habituationNumberOfTrials);

		TrialsInfo pretestTrials;
		int pretestTrialType = q.value(q.record().indexOf("pretest_trial_type")).toInt();
		int pretestTrialLength = q.value(q.record().indexOf("pretest_trial_length")).toInt();
		int pretestLookTimes = q.value(q.record().indexOf("minimum_look_time")).toInt();
		int pretestNumberOfTrials = q.value(q.record().indexOf("pretest_number_of_trials")).toInt();
		pretestTrials.setType(pretestTrialType);
		pretestTrials.setLength(pretestTrialLength);
		pretestTrials.setLookTimes(pretestLookTimes);
		pretestTrials.setNumberOfTrials(pretestNumberOfTrials);

		TrialsInfo testTrials;
		int testTrialType = q.value(q.record().indexOf("test_trial_type")).toInt();
		int testTrialLength = q.value(q.record().indexOf("test_trial_length")).toInt();
		int testLookTimes = q.value(q.record().indexOf("minimum_no_look_time")).toInt();
		int testNumberOfTrials = q.value(q.record().indexOf("test_number_of_trials")).toInt();
		testTrials.setType(testTrialType);
		testTrials.setLength(testTrialLength);
		testTrials.setLookTimes(testLookTimes);
		testTrials.setNumberOfTrials(testNumberOfTrials);

		designSettings->setHabituationTrialsInfo(habituationTrials);
		designSettings->setPretestTrialsInfo(pretestTrials);
		designSettings->setTestTrialsInfo(testTrials);
	}
}

void MainDao::getHabituationSettingsForExperiment(size_t experimentId, HabituationSettings* habituationSettings)
{
	int number;
	Q_ASSERT(0 != habituationSettings);
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from habituation_settings where experiment_id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		int id = q.value(q.record().indexOf("id")).toInt();
		habituationSettings->setId(id);
		number = q.value(q.record().indexOf("habituation_type")).toInt();
		habituationSettings->setHabituationType(getHabituationType(number));

		Habit::CriterionSettings criterionSettings;
		number = q.value(q.record().indexOf("criterion_basis")).toInt();
		criterionSettings.setBasis(getCriterionBasisType(number));
		number = q.value(q.record().indexOf("criterion_percent")).toInt();
		criterionSettings.setPercent(number);
		number = q.value(q.record().indexOf("window_size")).toInt();
		criterionSettings.setWindowSize(number);
		number = q.value(q.record().indexOf("window_type")).toInt();
		criterionSettings.setWindowType(getCriterionWindowType(number));
		habituationSettings->setCriterionSettings(criterionSettings);
		number = q.value(q.record().indexOf("total_look")).toInt();
		habituationSettings->setTotalLookLengthToEnd(number);
	}
}

void MainDao::getStimulusDisplayInfoForExperiment(size_t experimentId, Habit::StimulusDisplayInfo* stimulusDisplayInfo)
{
	Q_ASSERT(0 != stimulusDisplayInfo);
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from stimulus_display where experiment_id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		int id = q.value(q.record().indexOf("id")).toInt();
		stimulusDisplayInfo->setId(id);
		int presentationStyle = q.value(q.record().indexOf("presentation_style")).toInt();
		int displayType = q.value(q.record().indexOf("display_type")).toInt();
		bool isOrigAspectRatio = q.value(q.record().indexOf("is_original_aspect_ratio")).toBool();
		QString color = q.value(q.record().indexOf("background_color")).toString();
		stimulusDisplayInfo->setPresentationStyle(presentationStyle);
		stimulusDisplayInfo->setDisplayType(displayType);
		stimulusDisplayInfo->setMaintainOriginalAspectRatio(isOrigAspectRatio);
		stimulusDisplayInfo->setBackGroundColor(QColor(color));
	}
}

	
// DJS Modify these three methods (get(Pretest|Habituation|Test)StimuliSettingsForExperiment to set the
// stimulus type in each StimulusSettings object. Original habit would set this value later, even though
// the object's phase (PRETEST|HABITUATION|TEST) is determined at this point. Also modified the 
// getStimuliSettings method below. 
Habit::StimuliSettings MainDao::getPretestStimuliSettingsForExperiment(const QString& experiment)
{
	Habit::StimuliSettings ss = getStimuliSettings("pretest_stimuli", experiment, HStimContext::PreTestPhase);
	return ss;
}

Habit::StimuliSettings MainDao::getHabituationStimuliSettingsForExperiment(const QString& experiment)
{
	Habit::StimuliSettings ss = getStimuliSettings("habituation_stimuli", experiment, HStimContext::HabituationPhase);
	return ss;
}

Habit::StimuliSettings MainDao::getTestStimuliSettingsForExperiment(const QString& experiment)
{
	Habit::StimuliSettings ss = getStimuliSettings("test_stimuli", experiment, HStimContext::TestPhase);
	return ss;
}

Habit::StimuliSettings MainDao::getPretestStimuliSettingsForExperiment(int experiment) {
	QString experimentName = getExperimentNameById(experiment);
	return getPretestStimuliSettingsForExperiment(experimentName);
}

Habit::StimuliSettings MainDao::getHabituationStimuliSettingsForExperiment(int experiment) {
	QString experimentName = getExperimentNameById(experiment);
	return getHabituationStimuliSettingsForExperiment(experimentName);
}

Habit::StimuliSettings MainDao::getTestStimuliSettingsForExperiment(int experiment) {
	QString experimentName = getExperimentNameById(experiment);
	return getTestStimuliSettingsForExperiment(experimentName);
}

Habit::StimuliSettings MainDao::getStimuliSettings(const QString& table_name, const QString& experiment, const HStimContext& context) {
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from " + table_name + " inner join experiments e on experiment_id=e.id where e.name=?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(experiment);
	q.exec();
	qDebug() << q.lastError();
	Habit::StimuliSettings result(context);
	while(q.next()) {
		Habit::StimulusSettings ss = getStimulusSettings(q);
		ss.setContext(context);
		result.addStimuli(ss);
	}
	return result;
}

Habit::StimulusSettings MainDao::getStimulusSettings(const QSqlQuery& q) {
	Habit::StimulusSettings ss;
	QString name = q.value(q.record().indexOf("name")).toString();

	bool is_left_enabled = q.value(q.record().indexOf("is_left_enabled")).toBool();
	QString left_filename = q.value(q.record().indexOf("left_filename")).toString();
	bool is_left_loop_playback = q.value(q.record().indexOf("is_left_loop_playback")).toBool();

	bool is_center_enabled = q.value(q.record().indexOf("is_center_enabled")).toBool();
	QString center_filename = q.value(q.record().indexOf("center_filename")).toString();
	bool is_center_loop_playback = q.value(q.record().indexOf("is_center_loop_playback")).toBool();

	bool is_right_enabled = q.value(q.record().indexOf("is_right_enabled")).toBool();
	QString right_filename = q.value(q.record().indexOf("right_filename")).toString();
	bool is_right_loop_playback = q.value(q.record().indexOf("is_right_loop_playback")).toBool();

	bool is_independent_enabled = q.value(q.record().indexOf("is_independent_enabled")).toBool();
	QString independent_sound_filename = q.value(q.record().indexOf("independent_sound_filename")).toString();
	bool is_independent_loop_playback = q.value(q.record().indexOf("is_independent_loop_playback")).toBool();

	int left_audio_balance = q.value(q.record().indexOf("left_audio_balance")).toInt();
	int center_audio_balance = q.value(q.record().indexOf("center_audio_balance")).toInt();
	int right_audio_balance = q.value(q.record().indexOf("right_audio_balance")).toInt();
	int independent_audio_balance = q.value(q.record().indexOf("independent_audio_balance")).toInt();

	int id = q.value(q.record().indexOf("id")).toInt();

	ss.setId(id);
	ss.setName(name);
	ss.setLeftEnabled(is_left_enabled);
	ss.setCenterEnabled(is_center_enabled);
	ss.setRightEnabled(is_right_enabled);
	ss.setIndependentSoundEnabled(is_independent_enabled);

	Habit::StimulusInfo si_left(name, left_filename, is_left_loop_playback, left_audio_balance);
	Habit::StimulusInfo si_center(name, center_filename, is_center_loop_playback, center_audio_balance);
	Habit::StimulusInfo si_right(name, right_filename, is_right_loop_playback, right_audio_balance);
	Habit::StimulusInfo si_ind(name, independent_sound_filename, is_independent_loop_playback, independent_audio_balance);

	ss.setLeftStimulusInfo(si_left);
	ss.setCenterStimulusInfo(si_center);
	ss.setRightStimulusInfo(si_right);
	ss.setIndependentSoundInfo(si_ind);

	return ss;
}







// Utility methods

void MainDao::getExperimentSettingsByName(ExperimentSettings* experiment)
{
	Q_ASSERT(0 != experiment);
	if(!experiment->getName().isEmpty()) {
		connection* con = connection::get_instance();
		Q_ASSERT(0 != con);
		QString sql = "select * from experiments where name = ?";
		QSqlQuery q = con->exec(sql);
		q.addBindValue(experiment->getName());
		q.exec();
		qDebug() << q.lastError();
		if(q.next()) {
			size_t id = q.value(q.record().indexOf("id")).toInt();
			experiment->setId(id);
		}
	}
}

void MainDao::getExperimentNameById(ExperimentSettings* experiment)
{
	Q_ASSERT(0 != experiment);
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from experiments where id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue((uint)experiment->getId());
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		QString id = q.value(q.record().indexOf("name")).toString();
		experiment->setName(id);
	}
}


int MainDao::isExperimentExists(const QString& name) {
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * form experiments where name=?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(name);
	q.exec();
	int result = 0;
	if(q.next()) {
		result = q.value(q.record().indexOf("id")).toInt();
	}
	return result;
}

void MainDao::insertExperiment(const QString& name) {
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	if(!isExperimentExists(name)) {
		QString sql = "insert into experiments (name) values (?)";
		QSqlQuery q = con->exec(sql);
		q.addBindValue(name);
		q.exec();
		qDebug() << q.lastError();
	} else {
		qDebug() << "Experiment exists";
	}
}

void MainDao::updateExperiment(const QString& name) {
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	int id = isExperimentExists(name);
	if(id) {
		QString sql = "update experiments set name=? where id=?";
		QSqlQuery q = con->exec(sql);
		q.addBindValue(name);
		q.addBindValue(id);
		q.exec();
		qDebug() << q.lastError();
	} else {
		qDebug() << "Experiment doesn't exist";
	}
}

QString MainDao::getExperimentNameById(int id) {
	QString result;
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select name from experiments where id=?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(id);
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		result = q.value(q.record().indexOf("name")).toString();
	}
	return result;
}

Habit::ExperimentSettings MainDao::getExperimentSettingsById(int id)
{
	Habit::ExperimentSettings settings;
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from experiments where id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(id);
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		QString name = q.value(q.record().indexOf("name")).toString();
		settings.setName(name);
		settings.setId(id);
	}
	return settings;
}

QVector<ExperimentSettings> MainDao::getAllExperiments()
{
	QVector<ExperimentSettings> experiments;
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from experiments";
	QSqlQuery q = con->exec(sql);
	q.exec();
	qDebug() << q.lastError();
	while (q.next()) {
		ExperimentSettings settings;
		QString name = q.value(q.record().indexOf("name")).toString();
		settings.setName(name);
		size_t id = q.value(q.record().indexOf("id")).toInt();
		settings.setId(id);
		experiments.push_back(settings);
	}
	return experiments;
}

bool MainDao::insertOrUpdateExperimentSettings(Habit::ExperimentSettings* experimentSettings)
{
	bool result = false;
	if(0 != experimentSettings) {
		connection* con = connection::get_instance();
		Q_ASSERT(0 != con);
		QString name = experimentSettings->getName();
		int id = experimentSettings->getId();
		QSqlQuery q(*con);
		if(id > 0) {
			QString sql = "update experiments set name=? where id=?";
			q.prepare(sql);
			q.addBindValue(name);
			q.addBindValue(id);
			q.exec();
			qDebug() << q.lastError();
		} else {
			QString sql = "INSERT INTO experiments (name) VALUES (?)";
			q.prepare(sql);
			q.addBindValue(name);
			q.exec();
			qDebug() << q.lastError();
			qDebug() << q.lastInsertId().toInt();
			experimentSettings->setId(q.lastInsertId().toInt());
		}
		result = !q.lastError().isValid();
	}
	return result;
}

QStringList MainDao::getAllExperimentNames() {
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select name from experiments";
	QSqlQuery q = con->exec(sql);
	q.exec();
	qDebug() << q.lastError();
	QStringList result;
	while(q.next()) {
		QString name = q.value(q.record().indexOf("name")).toString();
		result.append(name);
	}
	return result;
}

QStringList MainDao::getAllSubjectsNames()
{
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select name from subject_settings";
	QSqlQuery q = con->exec(sql);
	q.exec();
	qDebug() << q.lastError();
	QStringList result;
	while(q.next()) {
		QString name = q.value(q.record().indexOf("name")).toString();
		result.append(name);
	}
	return result;
}

void MainDao::insertSubject(Habit::SubjectSettings* subjectSettings)
{
	Q_ASSERT(0 != subjectSettings);
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "insert into subject_settings (name, birthdate, testdate, condition, observer, cell_number, comments)"
		" values(?, ?, ?, ?, ?, ?, ?)";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(subjectSettings->getSubjectName());
	q.addBindValue(subjectSettings->getBirthDate());
	q.addBindValue(subjectSettings->getTestDate());
	q.addBindValue(subjectSettings->getCondition());
	q.addBindValue(subjectSettings->getObserver());
	q.addBindValue(subjectSettings->getCellNumber());
	q.addBindValue(subjectSettings->getComments());
	q.exec();
	subjectSettings->setId(q.lastInsertId().toInt());
	qDebug() << q.lastError();
}

void MainDao::updateSubject( const Habit::SubjectSettings& subjectSettings )
{
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "update subject_settings set name=?, birthdate=?, testdate=?, condition=?, observer=?, cell_number=?, comments=?"
		" where id=?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(subjectSettings.getSubjectName());
	q.addBindValue(subjectSettings.getBirthDate());
	q.addBindValue(subjectSettings.getTestDate());
	q.addBindValue(subjectSettings.getCondition());
	q.addBindValue(subjectSettings.getObserver());
	q.addBindValue(subjectSettings.getCellNumber());
	q.addBindValue(subjectSettings.getComments());
	q.addBindValue(subjectSettings.getId());
	q.exec();
	qDebug() << q.lastError();
}

bool MainDao::isSubjectUnique(const Habit::SubjectSettings& subjectSettings)
{
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from subject_settings where name=?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(subjectSettings.getSubjectName());
	q.exec();
	qDebug() << q.lastError();
	return !q.next();
}

Habit::SubjectSettings MainDao::getSubjectSettings(const QString& subj) {
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from subject_settings where name=?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(subj);
	q.exec();
	qDebug() << q.lastError();
	Habit::SubjectSettings ss;
	if(q.next()) {
		ss.setId(q.value(q.record().indexOf("id")).toInt());
		ss.setSubjectName(q.value(q.record().indexOf("name")).toString());
		ss.setBirthDate(q.value(q.record().indexOf("birthdate")).toDateTime());
		ss.setTestDate(q.value(q.record().indexOf("testdate")).toDateTime());
		ss.setCondition(q.value(q.record().indexOf("condition")).toString());
		ss.setObserver(q.value(q.record().indexOf("observer")).toString());
		ss.setCellNumber(q.value(q.record().indexOf("cell_number")).toString());
		ss.setComments(q.value(q.record().indexOf("comments")).toString());
	}
	return ss;
}

Habit::SubjectSettings MainDao::getSubjectSettings(int id) {
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from subject_settings where id=?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(id);
	q.exec();
	qDebug() << q.lastError();
	Habit::SubjectSettings ss;
	if(q.next()) {
		ss.setId(q.value(q.record().indexOf("id")).toInt());
		ss.setSubjectName(q.value(q.record().indexOf("name")).toString());
		ss.setBirthDate(q.value(q.record().indexOf("birthdate")).toDateTime());
		ss.setTestDate(q.value(q.record().indexOf("testdate")).toDateTime());
		ss.setCondition(q.value(q.record().indexOf("condition")).toString());
		ss.setObserver(q.value(q.record().indexOf("observer")).toString());
		ss.setCellNumber(q.value(q.record().indexOf("cell_number")).toString());
		ss.setComments(q.value(q.record().indexOf("comments")).toString());
	}
	return ss;
}


void MainDao::insertRunSettings(Habit::RunSettings* runSettings)
{
	Q_ASSERT(0 != runSettings);
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "insert into run_settings (experiment_id, subject_id, "
		"habituation_test_order, is_habituation_order_randomized, habituation_randomize_method, "
		"pretest_test_order, is_pretest_order_randomized, pretest_randomize_method, "
		"test_order, is_test_order_randomized, test_randomize_method) "
		" values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(runSettings->getExperimentId());
	q.addBindValue(runSettings->getSubjectId());
	q.addBindValue(runSettings->getHabituationOrder());
	q.addBindValue(runSettings->isHabituationRandomized());
	q.addBindValue(runSettings->getHabituationRandomizeMethod());
	q.addBindValue(runSettings->getPretestOrder());
	q.addBindValue(runSettings->isPretestRandomized());
	q.addBindValue(runSettings->getPretestRandomizeMethod());
	q.addBindValue(runSettings->getTestOrder());
	q.addBindValue(runSettings->isTestRandomized());
	q.addBindValue(runSettings->getTestRandomizeMethod());
	q.exec();
	runSettings->setId(q.lastInsertId().toInt());
	qDebug() << q.lastError();
}

void MainDao::updateRunSettings(const Habit::RunSettings& runSettings)
{
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "update run_settings set experiment_id=?, subject_id=?, "
		"habituation_test_order=?, is_habituation_order_randomized=?, habituation_randomize_method=?, "
		"pretest_test_order=?, is_pretest_order_randomized=?, pretest_randomize_method=?, "
		"test_order=?, is_test_order_randomized=?, test_randomize_method=? "
		" where id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(runSettings.getExperimentId());
	q.addBindValue(runSettings.getSubjectId());
	q.addBindValue(runSettings.getHabituationOrder());
	q.addBindValue(runSettings.isHabituationRandomized());
	q.addBindValue(runSettings.getHabituationRandomizeMethod());
	q.addBindValue(runSettings.getPretestOrder());
	q.addBindValue(runSettings.isPretestRandomized());
	q.addBindValue(runSettings.getPretestRandomizeMethod());
	q.addBindValue(runSettings.getTestOrder());
	q.addBindValue(runSettings.isTestRandomized());
	q.addBindValue(runSettings.getTestRandomizeMethod());
	q.addBindValue(runSettings.getId());
	q.exec();
	qDebug() << q.lastError();
}

Habit::RunSettings MainDao::getRunSettingsBySubject(const Habit::SubjectSettings& subjectSettings)
{
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from run_settings where subject_id = ?";
	QSqlQuery q = con->exec(sql);
	q.addBindValue(subjectSettings.getId());
	q.exec();
	Habit::RunSettings rs;
	if(q.next()) {
		rs.setExperimentId(q.value(q.record().indexOf("experiment_id")).toInt());
		rs.setSubjectId(q.value(q.record().indexOf("subject_id")).toInt());

		rs.setHabituationOrder(q.value(q.record().indexOf("habituation_test_order")).toString());
		rs.setHabituationRandomized(q.value(q.record().indexOf("is_habituation_order_randomized")).toBool());
		rs.setHabituationRandomizeMethod(q.value(q.record().indexOf("habituation_randomize_method")).toInt());

		rs.setPretestOrder(q.value(q.record().indexOf("pretest_test_order")).toString());
		rs.setPretestRandomized(q.value(q.record().indexOf("is_pretest_order_randomized")).toBool());
		rs.setPretestRandomizeMethod(q.value(q.record().indexOf("pretest_randomize_method")).toInt());

		rs.setTestOrder(q.value(q.record().indexOf("test_order")).toString());
		rs.setTestRandomized(q.value(q.record().indexOf("is_test_order_randomized")).toBool());
		rs.setTestRandomizeMethod(q.value(q.record().indexOf("test_randomize_method")).toInt());

		rs.setId(q.value(q.record().indexOf("id")).toInt());
	}
	return rs;
}

void MainDao::addOrUpdateConfigObject(const Habit::ResultViewerSettings& config)
{
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from result_view_settings";
	QSqlQuery q = con->exec(sql);
	q.exec();
	qDebug() << q.lastError();
	if(q.next()) {
		sql = "update result_view_settings set settings=?";
		QSqlQuery q1 = con->exec(sql);
		q1.addBindValue(config.toString());
		q1.exec();
		qDebug() << q1.lastError();
	} else {
		sql = "insert into result_view_settings (settings) values (?)";
		QSqlQuery q2 = con->exec(sql);
		q2.addBindValue(config.toString());
		q2.exec();
		qDebug() << q2.lastError();
	}
}

Habit::ResultViewerSettings MainDao::getResultViewerOptions()
{
	connection* con = connection::get_instance();
	Q_ASSERT(0 != con);
	QString sql = "select * from result_view_settings";
	QSqlQuery q = con->exec(sql);
	q.exec();
	qDebug() << q.lastError();
	Habit::ResultViewerSettings r;
	if(q.next()) {
		QString val = q.value(q.record().indexOf("settings")).toString();
		r.fromString(val);
	}
	return r;
}

} // namespace Habit