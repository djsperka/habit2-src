
#include "maindao.h"
#include "experimentsettings.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include <QtSql/QSqlError>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtCore/QTDebug>
#include <QSet>

namespace Habit {

MainDao::MainDao()
{}

MainDao::~MainDao()
{}


// Settings Add/Update methods

void MainDao::addOrUpdateHLookSettings(int experimentID, Habit::HLookSettings& lookSettings)
{
	QString sql;
	if (lookSettings.getId() > 0)
	{
		sql = "update look_settings set min_look_time=?, min_lookaway_time=?, inclusive_look_time=? where experiment_id=? and id=?";
	}
	else
	{
		sql = "insert into look_settings (min_look_time, min_lookaway_time, inclusive_look_time, experiment_id) values (?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(lookSettings.getMinLookTime());
	q.addBindValue(lookSettings.getMinLookAwayTime());
	q.addBindValue(lookSettings.getInclusiveLookTime() ? 1 : 0);
	q.addBindValue(experimentID);
	if (lookSettings.getId() > 0) q.addBindValue(lookSettings.getId());
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "MainDao::addOrUpdateHLookSettings sql error: " << q.lastError().text();
		throw HDBException(string("MainDao::addOrUpdateHLookSettings"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else
	{
		if (lookSettings.getId() < 1) lookSettings.setId(q.lastInsertId().toInt());
	}
	return;
}

void MainDao::addOrUpdateHPhaseSettings(int experimentID, Habit::HPhaseSettings& phaseSettings)
{
	QString sql;
	if (phaseSettings.getId() > 0)
	{
		sql = "update phase_settings "
				"set enabled=?, name=?, seqno=?, phase_type=?, ntrials=?, "
				"use_looking_criteria=?, is_single_look=?, is_max_accumulated_look_time=?, max_accumulated_look_time=?, "
				"is_max_lookaway_time=?, max_lookaway_time=?, "
				"repeat_trial_on_max_lookaway=?, "
				"is_max_stimulus_time=?, max_stimulus_time=?, "
				"measure_from_onset=?, measure_from_looking=?, "
				"is_max_no_look_time=?, max_no_look_time=? "
				"where experiment_id=? and id=?";
	}
	else
	{
		sql = "insert into phase_settings "
				"("
				"enabled, name, seqno, use_looking_criteria, is_single_look, is_max_accumulated_look_time, "
				"max_accumulated_look_time, is_max_lookaway_time, max_lookaway_time, repeat_trial_on_max_lookaway, "
				"is_max_stimulus_time, max_stimulus_time, "
				"measure_from_onset, measure_from_looking, "
				"is_max_no_look_time, max_no_look_time, "
				"experiment_id"
				") values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	}

	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(phaseSettings.getIsEnabled() ? 1 : 0);
	q.addBindValue(phaseSettings.getName());
	q.addBindValue(phaseSettings.getSeqno());
	q.addBindValue(phaseSettings.getUseLookingCriteria() ? 1 : 0);
	q.addBindValue(phaseSettings.getIsSingleLook() ? 1 : 0);
	q.addBindValue(phaseSettings.getIsMaxAccumulatedLookTime() ? 1 : 0);
	q.addBindValue(phaseSettings.getMaxAccumulatedLookTime());
	q.addBindValue(phaseSettings.getIsMaxLookAwayTime() ? 1 : 0);
	q.addBindValue(phaseSettings.getMaxLookAwayTime());
	q.addBindValue(phaseSettings.getRepeatTrialOnMaxLookAwayTime() ? 1 : 0);
	q.addBindValue(phaseSettings.getIsMaxStimulusTime() ? 1 : 0);
	q.addBindValue(phaseSettings.getMaxStimulusTime());
	q.addBindValue(phaseSettings.getMeasureStimulusTimeFromOnset() ? 1 : 0);
	q.addBindValue(phaseSettings.getMeasureStimulusTimeFromLooking() ? 1 : 0);
	q.addBindValue(phaseSettings.getIsMaxNoLookTime() ? 1 : 0);
	q.addBindValue(phaseSettings.getMaxNoLookTime());
	q.addBindValue(experimentID);
	if (phaseSettings.getId() > 0) q.addBindValue(phaseSettings.getId());
	if (q.exec())
	{
		if (phaseSettings.getId() < 1) phaseSettings.setId(q.lastInsertId().toInt());
	}
	else
	{
		qCritical() << "MainDao::addOrUpdateHPhaseSettings sql error: " << q.lastError().text();
		throw HDBException(string("MainDao::addOrUpdateHPhaseSettings"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	return;
}

void MainDao::addOrUpdateAttentionGetterSettings(int experimentID, Habit::AttentionGetterSettings& attentionGetterSettings)
{
	QString sql;

	// add/update stimulus first
	addOrUpdateStimulusSettings(-1, attentionGetterSettings.getAttentionGetterStimulus());
	attentionGetterSettings.setStimulusID(attentionGetterSettings.getAttentionGetterStimulus().getId());
	if(attentionGetterSettings.getId() > 0)
	{
		sql = "update attention_setup set use_attention_stimulus=?, stimulus_name=?, background_color=?, use_fixed_isi=?, isi_ms=?, stimulus_id=?"
			" where id=? and experiment_id=?";
	}
	else
	{
		sql = "insert into attention_setup (use_attention_stimulus, stimulus_name, background_color, use_fixed_isi, isi_ms, stimulus_id, experiment_id)"
			" values(?, ?, ?, ?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(attentionGetterSettings.isAttentionGetterUsed());
	q.addBindValue(attentionGetterSettings.getAttentionGetterStimulus().getName());
	q.addBindValue(attentionGetterSettings.getBackGroundColor().name());
	q.addBindValue(attentionGetterSettings.isFixedISI());
	q.addBindValue(attentionGetterSettings.getFixedISIMS());
	q.addBindValue(attentionGetterSettings.getStimulusID());
	if(attentionGetterSettings.getId() > 0)
	{
		q.addBindValue(attentionGetterSettings.getId());
	}
	q.addBindValue((uint)experimentID);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "MainDao::addOrUpdateAttentionGetterSettings sql error experimentID " << experimentID;
		throw HDBException(string("MainDao::addOrUpdateAttentionGetterSettings"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else if(attentionGetterSettings.getId() < 1)
	{
		attentionGetterSettings.setId(q.lastInsertId().toInt());
	}

	return;
}

void MainDao::addOrUpdateControlBarOptions(int experimentID, Habit::ControlBarOptions& controlBarOptions)
{
	QString sql;
	if(controlBarOptions.getId() > 0)
	{
		sql = "update controlbar_options set is_used=?, display_current_experiment=?, display_current_stimulus=?, display_looking_direction=?"
			" where id=? and experiment_id=?";
	}
	else
	{
		sql = "insert into controlbar_options (is_used, display_current_experiment, display_current_stimulus, display_looking_direction, experiment_id)"
			" values(?, ?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(controlBarOptions.isControlBarUsed());
	q.addBindValue(controlBarOptions.isCurrentExperimentDisplayed());
	q.addBindValue(controlBarOptions.isCurrentStimulusDisplayed());
	q.addBindValue(controlBarOptions.isLookingDirectionDisplayed());
	if(controlBarOptions.getId() > 0) {
		q.addBindValue(controlBarOptions.getId());
	}
	q.addBindValue(experimentID);
	q.exec();

	if (q.lastError().isValid())
	{
		qDebug() << "MainDao::addOrUpdateControlBarOption sql error: " << q.lastError().text();
		throw HDBException(string("MainDao::addOrUpdateControlBarOption"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else
	{
		if (controlBarOptions.getId() < 1) controlBarOptions.setId(q.lastInsertId().toInt());
	}
	return;
}


void MainDao::addOrUpdateHabituationSettings(int phaseId, Habit::HabituationSettings& habituationSettings)
{
	QString sql;
	if(habituationSettings.getId() > 0)
	{
		sql = "update habituation_settings set habituation_type=?, criterion_basis=?, criterion_percent=?, window_size=?, window_type=?, total_look=?, exclude_basis_window=?, require_min_basis_value=?, min_basis_value=?, phase_id=?, ntrials=?"
			" where id=?";
	}
	else
	{
		sql = "insert into habituation_settings (habituation_type, criterion_basis, criterion_percent, window_size, window_type, total_look, exclude_basis_window, require_min_basis_value, min_basis_value, phase_id, ntrials)"
			" values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(habituationSettings.getHabituationType().number());
	Habit::CriterionSettings cs = habituationSettings.getCriterionSettings();
	q.addBindValue(cs.getBasis().number());
	q.addBindValue(cs.getPercent());
	q.addBindValue(cs.getWindowSize());
	q.addBindValue(cs.getWindowType().number());
	q.addBindValue(habituationSettings.getTotalLookLengthToEnd());
	q.addBindValue(cs.getExcludeBasisWindow() ? 1 : 0);
	q.addBindValue(cs.getRequireMinBasisValue() ? 1 : 0);
	q.addBindValue(cs.getMinBasisValue());
	q.addBindValue(phaseId);
	q.addBindValue(habituationSettings.getNTrials());
	if(habituationSettings.getId() > 0)
	{
		q.addBindValue(habituationSettings.getId());
	}
	q.exec();
	if (q.lastError().isValid())
	{
		qDebug() << "MainDao::addOrUpdateHabituationSettings sql error: " << q.lastError().text();
		throw HDBException(string("MainDao::addOrUpdateHabituationSettings"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else
	{
		if (habituationSettings.getId() < 1) habituationSettings.setId(q.lastInsertId().toInt());
	}
	return;
}

void MainDao::deleteOrder(int id)
{
	QString sql = "delete from stimulus_order where id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(id);
	if (!q.exec())
	{
		qCritical() << "MainDao::deleteOrder  sql error: " << q.lastError().text();
		throw HDBException(string("MainDao::deleteOrder"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	return;
}

void MainDao::addOrUpdateStimuliSettings(int phaseId, Habit::StimuliSettings& stimuli)
{
	QSet<int> original_ids;
	QSet<int> edited_ids;
	QSet<int> deleted_ids;

	// See if any stimuli were deleted. First, get stimuli settings from db and make a list of all ids found.
	Habit::StimuliSettings original;
	getStimuliSettings(phaseId, original);
	for (int i=0; i<original.stimuli().size(); i++)
		original_ids.insert(original.stimuli()[i].getId());

	// make a list of all ids in the stimuli parameter
	for (int i=0; i<stimuli.stimuli().size(); i++)
		edited_ids.insert(stimuli.stimuli()[i].getId());

	// any stimuli that were deleted will be the difference between the two lists
	deleted_ids = original_ids.subtract(edited_ids);

	if (!deleted_ids.isEmpty())
	{
		qDebug() << "addOrUpdateStimuliSetting: deleting " << deleted_ids.size() << " stimuli.";
		QSetIterator<int> it(deleted_ids);
		while (it.hasNext())
			deleteStimulus(it.next());
	}

	// See if any orders were deleted.
	original_ids.clear();
	edited_ids.clear();
	deleted_ids.clear();

	for (int i=0; i<original.orders().size(); i++)
		original_ids.insert(original.orders()[i].getId());
	for (int i=0; i<stimuli.orders().size(); i++)
		edited_ids.insert(stimuli.orders()[i].getId());
	deleted_ids = original_ids.subtract(edited_ids);

	if (!deleted_ids.isEmpty())
	{
		qDebug() << "addOrUpdateStimuliSetting: deleting " << deleted_ids.size() << " orders";
		QSetIterator<int> it(deleted_ids);
		while (it.hasNext())
			deleteOrder(it.next());
	}

	// Now update or add what is in stimuli
	for (int i=0; i<stimuli.stimuli().size(); i++)
	{
		addOrUpdateStimulusSettings(phaseId, stimuli.stimuli()[i]);
	}

	// handle orders
	for (int i=0; i<stimuli.orders().size(); i++)
	{
		addOrUpdateStimulusOrder(phaseId, stimuli.orders()[i]);
	}

	return;
}


// TODO: fix this WILL NOT WORK
void MainDao::addOrUpdateStimulusSettings(int phaseID, Habit::StimulusSettings& ss)
{
	QString sql;
	QSqlQuery q;
	if (ss.getId() > 0)
	{
		// update stimulus table
		sql = "update stimulus set name=?, phase_id=? where id=?";
	}
	else
	{
		// insert into stimulus table. Make sure to set Id in ss object - it will be used to add/update stimfiles.
		sql = "insert into stimulus (name, phase_id) values (?)";
	}
	q.prepare(sql);
	q.addBindValue(QVariant(ss.getName()));
	q.addBindValue(QVariant(phaseID));
	if (ss.getId() > 0)
		q.addBindValue(QVariant(ss.getId()));
	if (!q.exec())
	{
		qCritical() << "MainDao::addOrUpdateStimulusSetting error: " << q.lastQuery() << " : " << q.lastError().text();
		throw HDBException(string("MainDao::addOrUpdateStimulusSetting"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else
	{
		if (ss.getId() < 1)
		{
			ss.setId(q.lastInsertId().toInt());
		}
	}

	// add/update stimulus info (these go into stimfiles table)
	//qDebug() << "add/update " << ss.getName() << "/" << ss.getId() << " info id l " << ss.getLeftStimulusInfo().getId() << " c " << ss.getCenterStimulusInfo().getId() << " r " << ss.getRightStimulusInfo().getId() << " s " << ss.getIndependentSoundInfo().getId();
	addOrUpdateStimulusInfo(ss.getId(), HPlayerPositionType::Left, ss.getLeftStimulusInfo());
	addOrUpdateStimulusInfo(ss.getId(), HPlayerPositionType::Center, ss.getCenterStimulusInfo());
	addOrUpdateStimulusInfo(ss.getId(), HPlayerPositionType::Right, ss.getRightStimulusInfo());
	addOrUpdateStimulusInfo(ss.getId(), HPlayerPositionType::Sound, ss.getIndependentSoundInfo());

	return;
}

void MainDao::addOrUpdateStimulusInfo(int stimulus_id, const HPlayerPositionType& position, StimulusInfo& info)
{
	QString sql;
	QSqlQuery q;
	if (info.getId() > 0)
	{
		// update stimfiles table
		sql = "update stimfiles set stimulus_id=?, position=?, filename=?, isLoop=?, volume=?, isBackground=?, isColor=?, color=? where id=?";
	}
	else
	{
		// insert into stimulus table. Make sure to set Id in ss object - it will be used to add/update stimfiles.
		sql = "insert into stimfiles (stimulus_id, position, filename, isLoop, volume, isBackground, isColor, color) values (?, ?, ?, ?, ?, ?, ?, ?)";
	}
	q.prepare(sql);
	q.addBindValue(QVariant(stimulus_id));
	q.addBindValue(QVariant(position.number()));
	q.addBindValue(QVariant(info.getFileName()));
	q.addBindValue(QVariant(info.isLoopPlayBack()));
	q.addBindValue(QVariant(info.getVolume()));
	q.addBindValue(QVariant(info.isBackground()));
	q.addBindValue(QVariant(info.isColor()));
	q.addBindValue(QVariant(info.getColor().name()));
	if (info.getId() > 0)
		q.addBindValue(QVariant(info.getId()));

	if (!q.exec())
	{
		qCritical() << "MainDao::addOrUpdateStimulusSettings error: " << q.lastQuery() << " : " << q.lastError().text();
		throw HDBException(string("MainDao::addOrUpdateStimulusSettings"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else
	{
		if (info.getId() <= 0)
			info.setId(q.lastInsertId().toInt());
	}
	return;
}

void MainDao::addOrUpdateStimulusOrder(int phase_id, Habit::HStimulusOrder& order)
{
	QString sql;
	if (order.getId() > 0)
	{
		sql = "update stimulus_order set name=?, phase_id=?, context=?, orderlist=? where id=?";
	}
	else
	{
		sql = "insert into stimulus_order (name, phase_id, context, orderlist) values (?, ?, ?, ?)";
	}
	QSqlQuery q;

	q.prepare(sql);
	q.addBindValue(order.getName());
	q.addBindValue(phase_id);
	q.addBindValue(order.getContext()->number());
	q.addBindValue(order.getList().join(QString(",")));
	if (order.getId() > 0)
	{
		q.addBindValue(order.getId());
	}
	if (!q.exec())
	{
		qCritical() << "MainDao::addOrUpdateStimulusOrder  sql error: " << q.lastError().text();
		throw HDBException(string("MainDao::addOrUpdateStimulusOrder"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else
	{
		if (order.getId() < 1) order.setId(q.lastInsertId().toInt());
	}
	return;
}

void MainDao::addOrUpdateStimulusDisplayInfo(int experimentID, Habit::StimulusDisplayInfo& stimulusDisplayInfo)
{
	QString sql;
	if(stimulusDisplayInfo.getId() > 0)
	{
		sql = "update stimulus_display set presentation_style=?, display_type=?, is_original_aspect_ratio=?, background_color=?, stimulus_layout=?, use_iss=?"
			" where id=? and experiment_id=?";
	}
	else
	{
		sql = "insert into stimulus_display (presentation_style, display_type, is_original_aspect_ratio, background_color, stimulus_layout, use_iss, experiment_id)"
			" values(?, ?, ?, ?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(stimulusDisplayInfo.getPresentationStyle().number());
	q.addBindValue(stimulusDisplayInfo.getDisplayType().number());
	q.addBindValue(stimulusDisplayInfo.isOriginalAspectRatioMaintained());
	q.addBindValue(stimulusDisplayInfo.getBackGroundColor());
	q.addBindValue(stimulusDisplayInfo.getStimulusLayoutType().number());
	q.addBindValue(stimulusDisplayInfo.getUseISS() ? 1 : 0);
	if(stimulusDisplayInfo.getId() > 0)
	{
		q.addBindValue(stimulusDisplayInfo.getId());
	}
	q.addBindValue(experimentID);
	q.exec();

	if (q.lastError().isValid())
	{
		qCritical() << "MainDao::addOrUpdateStimulusDisplaySetting  sql error: " << q.lastError().text();
		throw HDBException(string("MainDao::addOrUpdateStimulusDisplaySetting"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else
	{
		if (stimulusDisplayInfo.getId() < 1) stimulusDisplayInfo.setId(q.lastInsertId().toInt());
	}
	return;
}





// Retrieve Data from DB

void MainDao::getHLookSettingsForExperiment(int experimentID, HLookSettings& lookSettings)
{
	QString sql = "select * from look_settings where experiment_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(experimentID);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getHLookSettingsForExperiment id=" << experimentID;
		throw HDBException(string("MainDao::getHLookSettingsForExperiment"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else if(q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		lookSettings.setId(id);
		uint minLookTime = q.value(q.record().indexOf("min_look_time")).toUInt();
		lookSettings.setMinLookTime(minLookTime);
		uint minLookAwayTime = q.value(q.record().indexOf("min_lookaway_time")).toUInt();
		lookSettings.setMinLookAwayTime(minLookAwayTime);
		int iInclusiveLookTime = q.value(q.record().indexOf("inclusive_look_time")).toInt();
		lookSettings.setInclusiveLookTime(iInclusiveLookTime!=0);
	}
	else
	{
		qCritical() << "No look_settings record found for id=" << experimentID;
		throw HDBException(string("MainDao::getHLookSettingsForExperiment"), q.lastQuery().toStdString(), string("No record found!"));
	}
	return;
}

void MainDao::getHPhaseSettings(int phaseId, HPhaseSettings& phaseSettings)
{
	QString sql = "select * from phase_settings where phaseId = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(phaseId);
	if (!q.exec())
	{
		qCritical() << "Error in MainDao::getHPhaseSettings id=" << phaseId;
		throw HDBException(string("MainDao::getHPhaseSettings"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else if(q.next())
	{
		phaseSettings.setId(phaseId);
		phaseSettings.setName(q.value(q.record().indexOf("name")).toString());
		phaseSettings.setSeqno(q.value(q.record().indexOf("seqno")).toInt());
		phaseSettings.setIsEnabled(q.value(q.record().indexOf("enabled")).toBool());
		phaseSettings.setUseLookingCriteria(q.value(q.record().indexOf("use_looking_criteria")).toBool());
		phaseSettings.setIsSingleLook(q.value(q.record().indexOf("is_single_look")).toBool());
		phaseSettings.setIsMaxAccumulatedLookTime(q.value(q.record().indexOf("is_max_accumulated_look_time")).toBool());
		phaseSettings.setMaxAccumulatedLookTime(q.value(q.record().indexOf("max_accumulated_look_time")).toUInt());
		phaseSettings.setIsMaxLookAwayTime(q.value(q.record().indexOf("is_max_lookaway_time")).toBool());
		phaseSettings.setMaxLookAwayTime(q.value(q.record().indexOf("max_lookaway_time")).toUInt());
		phaseSettings.setRepeatTrialOnMaxLookAwayTime(q.value(q.record().indexOf("repeat_trial_on_max_lookaway")).toBool());
		phaseSettings.setIsMaxStimulusTime(q.value(q.record().indexOf("is_max_stimulus_time")).toBool());
		phaseSettings.setMaxStimulusTime(q.value(q.record().indexOf("max_stimulus_time")).toUInt());
		phaseSettings.setMeasureStimulusTimeFromOnset(q.value(q.record().indexOf("measure_from_onset")).toBool());
		phaseSettings.setMeasureStimulusTimeFromLooking(q.value(q.record().indexOf("measure_from_looking")).toBool());
		phaseSettings.setIsMaxNoLookTime(q.value(q.record().indexOf("is_max_no_look_time")).toBool());
		phaseSettings.setMaxNoLookTime(q.value(q.record().indexOf("max_no_look_time")).toUInt());
	}
	else
	{
		qCritical() << "No phase_settings record found for phase id=" << phaseId;
		throw HDBException(string("MainDao::getHPhaseSettings"), q.lastQuery().toStdString(), string("No record found!"));
	}

	return;
}


void MainDao::getHPhaseSettingsIDs(int experimentID, QList<int>& ids)
{
	QString sql = "select id from phase_settings where experiment_id=? order by seqno";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(experimentID);
	if (!q.exec())
	{
		qCritical() << "Error in MainDao::getHPhaseSettingsIDs experimentID=" << experimentID;
		throw HDBException(string("MainDao::getHPhaseSettingsIDs"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else
	{
		ids.clear();
		while(q.next())
		{
			ids.push_back(q.value(0).toInt());
		}
	}
	return;
}

void MainDao::getAttentionGetterSettings(int experimentID, AttentionGetterSettings& attentionGetterSettings)
{
	QSqlQuery q("select * from attention_setup where experiment_id=?");

	q.addBindValue(experimentID);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getAttentionGetterSettings, experiment_id=" << experimentID;
		throw HDBException(string("MainDao::getAttentionGetterSettings"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else if (q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		bool useAttentionStimulus = q.value(q.record().indexOf("use_attention_stimulus")).toBool();
		QString backgroundColor = q.value(q.record().indexOf("background_color")).toString();
		bool bFixedISI = q.value(q.record().indexOf("use_fixed_isi")).toBool();
		int isiMS = q.value(q.record().indexOf("isi_ms")).toInt();
		int stimulusID = q.value(q.record().indexOf("stimulus_id")).toInt();
		attentionGetterSettings.setId(id);
		attentionGetterSettings.setUseAttentionGetter(useAttentionStimulus);
		attentionGetterSettings.setBackGroundColor(QColor(backgroundColor));
		attentionGetterSettings.setIsFixedISI(bFixedISI);
		attentionGetterSettings.setFixedISIMS(isiMS);
		attentionGetterSettings.setStimulusID(stimulusID);
		StimulusSettings ss;
		getStimulusSettings(stimulusID, ss);
		attentionGetterSettings.setAttentionGetterStimulus(ss);
	}
	else
	{
		// There should be a record!
		qCritical() << "MainDao::getAttentionGetterSettings, no attention_setup record for experiment_id=" << experimentID;
		throw HDBException(string("MainDao::getAttentionGetterSettings"), q.lastQuery().toStdString(), string("No record found!"));
	}
	return;
}



void MainDao::getControlBarOptionsForExperiment(int experimentID, ControlBarOptions& controlBarOptions)
{
	QString sql = "select * from controlbar_options where experiment_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(experimentID);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getControlBarOptionsForExperiment id=" << experimentID;
		throw HDBException(string("MainDao::getControlBarOptionsForExperiment"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else if(q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		controlBarOptions.setId(id);
		size_t useControlBar = q.value(q.record().indexOf("is_used")).toBool();
		controlBarOptions.setUseControlBar(useControlBar);
		size_t displayCurrentExperiment = q.value(q.record().indexOf("display_current_experiment")).toBool();
		controlBarOptions.setDisplayCurrentExperiment(displayCurrentExperiment);
		size_t displayCurrentStimulus = q.value(q.record().indexOf("display_current_stimulus")).toBool();
		controlBarOptions.setDisplayCurrentStimulus(displayCurrentStimulus);
		int number = q.value(q.record().indexOf("display_looking_direction")).toBool();
		controlBarOptions.setDisplayLookingDirection(number);
	}
	else
	{
		qCritical() << "No controlbar_options record found in MainDao::getControlBarOptionsForExperiment id=" << experimentID;
		throw HDBException(string("MainDao::getControlBarOptionsForExperiment - no control bar options!"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	return;
}


void MainDao::getHabituationSettingsForPhase(int phaseId, HabituationSettings& habituationSettings)
{
	int number;
	QString sql = "select * from habituation_settings where phase_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(phaseId);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getHabituationSettingsForExperiment phaseid=" << phaseId;
		throw HDBException(string("MainDao::getHabituationSettingsForPhase"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else if (q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		habituationSettings.setId(id);
		number = q.value(q.record().indexOf("habituation_type")).toInt();
		habituationSettings.setHabituationType(getHabituationType(number));
		number = q.value(q.record().indexOf("ntrials")).toInt();
		habituationSettings.setNTrials(number);

		Habit::CriterionSettings criterionSettings;
		number = q.value(q.record().indexOf("criterion_basis")).toInt();
		criterionSettings.setBasis(getCriterionBasisType(number));
		number = q.value(q.record().indexOf("criterion_percent")).toInt();
		criterionSettings.setPercent(number);
		number = q.value(q.record().indexOf("window_size")).toInt();
		criterionSettings.setWindowSize(number);
		number = q.value(q.record().indexOf("window_type")).toInt();
		criterionSettings.setWindowType(getCriterionWindowType(number));
		number = q.value(q.record().indexOf("exclude_basis_window")).toInt();
		criterionSettings.setExcludeBasisWindow(number==1);
		number = q.value(q.record().indexOf("require_min_basis_value")).toInt();
		criterionSettings.setRequireMinBasisValue(number==1);
		number = q.value(q.record().indexOf("min_basis_value")).toInt();
		criterionSettings.setMinBasisValue(number);

		habituationSettings.setCriterionSettings(criterionSettings);
		number = q.value(q.record().indexOf("total_look")).toInt();
		habituationSettings.setTotalLookLengthToEnd(number);
	}
	else
	{
		qCritical() << "No habituation_settings record found for phase id=" << phaseId;
		throw HDBException(string("MainDao::getHabituationSettingsForPhase - no habituation_settings record found."), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	return;
}


void MainDao::getStimulusDisplayInfoForExperiment(int experimentID, Habit::StimulusDisplayInfo& stimulusDisplayInfo)
{
	QString sql = "select * from stimulus_display where experiment_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(experimentID);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getStimulusDisplayInfoForExperiment id=" << experimentID;
		throw HDBException(string("MainDao::getStimulusDisplayInfoForExperiment"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else if (q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		stimulusDisplayInfo.setId(id);
		int presentationStyle = q.value(q.record().indexOf("presentation_style")).toInt();
		int displayType = q.value(q.record().indexOf("display_type")).toInt();
		bool isOrigAspectRatio = q.value(q.record().indexOf("is_original_aspect_ratio")).toBool();
		QString color = q.value(q.record().indexOf("background_color")).toString();
		int stimulusLayout = q.value(q.record().indexOf("stimulus_layout")).toInt();
		bool useISS = q.value(q.record().indexOf("use_iss")).toBool();
		stimulusDisplayInfo.setPresentationStyle(getPresentationStyle(presentationStyle));
		stimulusDisplayInfo.setDisplayType(getDisplayType(displayType));
		stimulusDisplayInfo.setMaintainOriginalAspectRatio(isOrigAspectRatio);
		stimulusDisplayInfo.setBackGroundColor(QColor(color));
		stimulusDisplayInfo.setStimulusLayoutType(getStimulusLayoutType(stimulusLayout));
		stimulusDisplayInfo.setUseISS(useISS);
	}
	else
	{
		qCritical() << "No stimulus_display record found for id=" << experimentID;
		throw HDBException(string("MainDao::getStimulusDisplayInfoForExperiment: No stimulus_display record found"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	return;
}

void MainDao::getStimuliSettings(int phaseID, StimuliSettings& stimuli)
{
	QSqlQuery q("select id, name from stimulus where phase_id=?");
	q.addBindValue(phaseID);
	if (!q.exec())
	{
		qCritical() << "Error in MainDao::getStimuliSettings phase_id=" << phaseID;
		throw HDBException(string("MainDao::getStimuliSettings"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else if (q.next())
	{
		do
		{
			int stimulus_id = q.value(0).toInt();
			QString stimulus_name = q.value(1).toString();
			StimulusSettings stimulus(stimulus_name);
			stimulus.setId(stimulus_id);
			getStimulusSettings(stimulus_id, stimulus);
			stimuli.addStimulus(stimulus);
		} while (q.next());
	}

	// get the HStimulusOrderList

	QSqlQuery q2("select * from stimulus_order where phase_id=?");
	q2.addBindValue(phaseID);
	if (!q2.exec())
	{
		qCritical() << "Error getting orders in MainDao::getStimuliSettings phase_id=" << phaseID;
		throw HDBException(string("MainDao::getStimuliSettings"), q2.lastQuery().toStdString(), q2.lastError().text().toStdString());
	}
	else
	{
		while (q2.next())
		{
			Habit::HStimulusOrder order;
			QSqlRecord record = q2.record();

			getStimulusOrderFromRecord(record, order);
			stimuli.addOrder(order);
		}
	}
	return;
}

void MainDao::getStimulusSettings(int stimulus_id, StimulusSettings& settings)
{
	QSqlQuery qsf("select id, position, filename, isLoop, volume, isBackground, isColor, color from stimfiles where stimulus_id=?");
	qsf.addBindValue(stimulus_id);
	if (!qsf.exec())
	{
		qCritical() << "Error in MainDao::getStimulusSettings stimulus_id=" << stimulus_id << "No rows found!";
		throw HDBException(string("MainDao::getStimulusSettings"), qsf.lastQuery().toStdString(), qsf.lastError().text().toStdString());
	}
	else
	{
		while (qsf.next())
		{
			// TODO - this is dangerous! Fixme!
			StimulusInfo info(qsf.value(2).toString(), qsf.value(3).toBool(), qsf.value(4).toInt(), qsf.value(5).toBool(), qsf.value(6).toBool(), qsf.value(7).toString());
			info.setId(qsf.value(0).toInt());
			settings.setStimulusInfo(info, getPlayerPositionType(qsf.value(1).toInt()));
		}
	}
	return;
}


void MainDao::deleteStimulus(int stimulus_id)
{
	QSqlQuery q("delete from stimfiles where stimulus_id=?");
	q.addBindValue(QVariant(stimulus_id));
	if (!q.exec())
	{
		qCritical() << "Error in MainDao::deleteStimulus stimulus_id=" << stimulus_id;
		throw HDBException(string("MainDao::deleteStimulus"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	else
	{
		QSqlQuery q2("delete from stimulus where id=?");
		q2.addBindValue(QVariant(stimulus_id));
		if (!q2.exec())
		{
			qCritical() << "Error in MainDao::deleteStimulus stimulus_id=" << stimulus_id;
			throw HDBException(string("MainDao::deleteStimulus"), q2.lastQuery().toStdString(), q2.lastError().text().toStdString());
		}
	}
	return;
}

void MainDao::getStimulusOrderFromRecord(const QSqlRecord& record, Habit::HStimulusOrder& order)
{
	order.setId(record.value(record.indexOf("id")).toInt());
	order.setName(record.value(record.indexOf("name")).toString());
	order.setList(record.value(record.indexOf("orderlist")).toString().split(QString(",")));
}



#if 0
// Utility methods

void MainDao::getExperimentSettingsByName(ExperimentSettings* experiment)
{
	Q_ASSERT(0 != experiment);
	if(!experiment->getName().isEmpty()) {
		QString sql = "select * from experiments where name = ?";
		QSqlQuery q;
		q.prepare(sql);
		q.addBindValue(experiment->getName());
		q.exec();
		if(q.next()) {
			size_t id = q.value(q.record().indexOf("id")).toInt();
			experiment->setId(id);
		}
	}
}

void MainDao::getExperimentNameById(ExperimentSettings* experiment)
{
	Q_ASSERT(0 != experiment);
	QString sql = "select * from experiments where id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue((uint)experiment->getId());
	q.exec();
	if(q.next()) {
		QString id = q.value(q.record().indexOf("name")).toString();
		experiment->setName(id);
	}
}
#endif

bool MainDao::experimentExists(const QString& name, int& id)
{
	bool b = false;
	id = 0;
	QString sql = "select id from experiments where name=?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(name);
	q.exec();
	if (q.next())
	{
		b = true;
		id = q.value(q.record().indexOf("id")).toInt();
	}
	return b;
}

bool MainDao::experimentExists(int id)
{
	QString sql = "select name from experiments where id=?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(id);
	q.exec();
	return q.next();
}


void MainDao::updateExperimentName(const QString& oldName, const QString& newName)
{
	int id = 0;
	if (experimentExists(oldName, id))
	{
		QString sql = "update experiments set name=? where id=?";
		QSqlQuery q;
		q.prepare(sql);
		q.addBindValue(newName);
		q.addBindValue(id);
		q.exec();
		if (q.lastError().isValid())
		{
			qCritical() << "Cannot update experiment name: " << q.lastError().text();
			throw HDBException(string("MainDao::updateExperimentName"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
		}
	}
	else
	{
		qCritical() << "cannot update experiment name from " << oldName << " to " << newName << " Experiment \"" << oldName<< "\" doesn't exist";
		throw HDBException(string("MainDao::updateExperimentName"), string("Experiment not found in experiments table"), string("Experiment not found"));
	}
	return;
}

void MainDao::getExperimentName(int id, QString& name)
{
	name = "";
	QSqlQuery q("select name from experiments where id=?");
	q.addBindValue(id);
	if (q.exec())
	{
		if (q.next())
		{
			name = q.value(q.record().indexOf("name")).toString();
		}
		else
		{
			qCritical() << "MainDao::getExperimentName(): Cannot find record in experiments table for id=" << id;
			throw HDBException(string("MainDao::getExperimentName()"), q.lastQuery().toStdString(), string("no record found in experiments table!"));
		}
	}
	else
	{
		qCritical() << "MainDao::getExperimentName(): " << q.lastQuery() << " : error - " << q.lastError().text();
		throw HDBException(string("MainDao::getExperimentName()"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	return;
}

void MainDao::getExperimentID(const QString& name, int& id)
{
	id = -1;
	QSqlQuery q("select id from experiments where name=?");
	q.addBindValue(name);
	if (q.exec())
	{
		if (q.next())
		{
			id = q.value(q.record().indexOf("id")).toInt();
		}
		else
		{
			qCritical() << "Cannot find record in experiments table for name=" << name;
			throw HDBException(string("MainDao::getExperimentID()"), q.lastQuery().toStdString(), string("no record found in experiments table!"));
		}
	}
	else
	{
		qCritical() << "MainDao::getExperimentID(): " << q.lastQuery() << " : error - " << q.lastError().text();
		throw HDBException(string("MainDao::getExperimentID()"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	return;
}

#if 0
QString MainDao::getExperimentNameById(int id) {
	QString result;
	QString sql = "select name from experiments where id=?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(id);
	q.exec();
	if(q.next()) {
		result = q.value(q.record().indexOf("name")).toString();
	}
	return result;
}

Habit::ExperimentSettings MainDao::getExperimentSettingsById(int id)
{
	Habit::ExperimentSettings settings;
	QString sql = "select * from experiments where id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(id);
	q.exec();
	if(q.next()) {
		QString name = q.value(q.record().indexOf("name")).toString();
		settings.setName(name);
		settings.setId(id);
	}
	return settings;
}
#endif

void MainDao::addOrUpdateExperimentSettings(Habit::ExperimentSettings& experimentSettings)
{
	QString name = experimentSettings.getName();
	int id = experimentSettings.getId();
	int hidden = experimentSettings.isHidden() ? 1 : 0;
	QSqlQuery q;
	if(id > 0)
	{
		QString sql = "update experiments set name=?, hidden=? where id=?";
		q.prepare(sql);
		q.addBindValue(experimentSettings.getName());
		q.addBindValue(experimentSettings.isHidden() ? 1 : 0);
		q.addBindValue(experimentSettings.getId());
	}
	else {
		QString sql = "INSERT INTO experiments (name, hidden) VALUES (?, ?)";
		q.prepare(sql);
		q.addBindValue(experimentSettings.getName());
		q.addBindValue(experimentSettings.isHidden() ? 1 : 0);
	}

	if (!q.exec())
	{
		qCritical() << "MainDao::addOrUpdateExperimentSettings - sql error experiment " << experimentSettings.getName() << " id " << experimentSettings.getId();
		throw HDBException(string("MainDao::addOrUpdateExperimentSettings"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	if (experimentSettings.getId() < 0)
	{
		experimentSettings.setId(q.lastInsertId().toInt());
	}

	return;
}

void MainDao::deleteFromTable(const QString table, const QString key, int id)
{
	QString s = QString("delete from %1 where %2=%3").arg(table).arg(key).arg(id);
	QSqlQuery q(s);
	qDebug() << "deleteFromTable: " << s << (q.lastError().isValid() ? "ERROR" : "OK");
	if (q.lastError().isValid())
	{
		throw HDBException(string("MainDao::deleteFromTable"), q.lastQuery().toStdString(), q.lastError().text().toStdString());
	}
	return;
}

void MainDao::deleteExperiment(Habit::ExperimentSettings& experimentSettings)
{
	if (experimentSettings.getId()>0)
	{
		// delete attention getter stimulus
		deleteStimulus(experimentSettings.getAttentionGetterSettings().getStimulusID());

		// delete attention_setup, control_bar_options, look_settings, stimulus_display_info records
		deleteFromTable("attention_setup", "id", experimentSettings.getAttentionGetterSettings().getId());
		deleteFromTable("controlbar_options", "id", experimentSettings.getControlBarOptions().getId());
		deleteFromTable("look_settings", "id", experimentSettings.getHLookSettings().getId());
		deleteFromTable("stimulus_display_info", "id", experimentSettings.getStimulusDisplayInfo().getId());

		// delete each phase records in habituation_settings, phase_settings, and all stimuli/orders
		for (int i=0; i<experimentSettings.getNumberOfPhases(); i++)
		{
			const HPhaseSettings& phaseSettings = experimentSettings.phaseAt(i);
			deleteFromTable("habituation_settings", "id", phaseSettings.habituationSettings().getId());
			deleteFromTable("phase_settings", "id", phaseSettings.getId());
			const HStimulusSettingsList& ssList = phaseSettings.stimuli().stimuli();
			for (int j=0; j<ssList.size(); j++)
				deleteStimulus(ssList.at(j).getId());
			const HStimulusOrderList& orders = phaseSettings.stimuli().orders();
			for (int j=0; j<orders.size(); j++)
				deleteOrder(orders.at(j).getId());
		}
	}
	else
	{
		qCritical() << "Cannot delete experiment \"" << experimentSettings.getName() << "\" -- id<=0!";
		throw HDBException(string("MainDao::deleteExperiment"), string("id is less than zero"), string("this experiment may not be in database?"));
	}
	return;
}



QStringList MainDao::getAllExperimentNames(bool bIncludeHiddenExperiments)
{
	QSqlQuery q;
	if (bIncludeHiddenExperiments)
		q.exec("select name from experiments");
	else
		q.exec("select name from experiments where hidden=0");
	QStringList result;
	while(q.next())
	{
		QString name = q.value(q.record().indexOf("name")).toString();
		result.append(name);
	}
	return result;
}


} // namespace Habit
