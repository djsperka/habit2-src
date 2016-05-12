
#include "maindao.h"
#include "experimentsettings.h"
//#include "connection.h"

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

bool MainDao::addOrUpdateHLookSettings(int experimentId, Habit::HLookSettings* settings)
{
	QString sql;
	if (settings->getId() > 0)
	{
		sql = "update look_settings set min_look_time=?, min_lookaway_time=?, inclusive_look_time=? where experiment_id=? and id=?";
	}
	else
	{
		sql = "insert into look_settings (min_look_time, min_lookaway_time, inclusive_look_time, experiment_id) values (?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(settings->getMinLookTime());
	q.addBindValue(settings->getMinLookAwayTime());
	q.addBindValue(settings->getInclusiveLookTime() ? 1 : 0);
	q.addBindValue(experimentId);
	if (settings->getId() > 0) q.addBindValue(settings->getId());
	q.exec();
	if (!q.lastError().isValid())
	{
		if (settings->getId() < 1) settings->setId(q.lastInsertId().toInt());
		return true;
	}
	qDebug() << "MainDao::addOrUpdateHLookSettings sql error: " << q.lastError().text();
	return false;
}

bool MainDao::addOrUpdateHPhaseSettings(int experimentId, Habit::HPhaseSettings* settings)
{
	QString sql;
	if (settings->getId() > 0)
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
	q.addBindValue(settings->getIsEnabled() ? 1 : 0);
	q.addBindValue(settings->getName());
	q.addBindValue(settings->getSeqno());
	//q.addBindValue(settings->getPhaseType().number());
	//q.addBindValue(settings->getNTrials());
	q.addBindValue(settings->getUseLookingCriteria() ? 1 : 0);
	q.addBindValue(settings->getIsSingleLook() ? 1 : 0);
	q.addBindValue(settings->getIsMaxAccumulatedLookTime() ? 1 : 0);
	q.addBindValue(settings->getMaxAccumulatedLookTime());
	q.addBindValue(settings->getIsMaxLookAwayTime() ? 1 : 0);
	q.addBindValue(settings->getMaxLookAwayTime());
	q.addBindValue(settings->getRepeatTrialOnMaxLookAwayTime() ? 1 : 0);
	//qDebug() << "MainDao::addOrUpdateHPhaseSettings(" << experimentId << ", " << settings->getPhaseType().number() << ") repeat on max lookaway " << (settings->getRepeatTrialOnMaxLookAwayTime() ? 1 : 0);
	q.addBindValue(settings->getIsMaxStimulusTime() ? 1 : 0);
	q.addBindValue(settings->getMaxStimulusTime());
	q.addBindValue(settings->getMeasureStimulusTimeFromOnset() ? 1 : 0);
	q.addBindValue(settings->getMeasureStimulusTimeFromLooking() ? 1 : 0);
	q.addBindValue(settings->getIsMaxNoLookTime() ? 1 : 0);
	q.addBindValue(settings->getMaxNoLookTime());
	q.addBindValue(experimentId);
	if (settings->getId() > 0) q.addBindValue(settings->getId());
	q.exec();
	if (!q.lastError().isValid())
	{
		if (settings->getId() < 1) settings->setId(q.lastInsertId().toInt());
		return true;
	}
	qDebug() << "MainDao::addOrUpdateHPhaseSettings sql error: " << q.lastError().text();
	return false;
}


bool MainDao::addOrUpdateMonitorSetting(int experimentId, Habit::MonitorSettings* settings)
{
	QString sql;
	if(settings->getId() > 0)
	{
		sql = "update monitor_settings set control_monitor=?, left_monitor=?, center_monitor=?, right_monitor=?"
			" where id=? and experiment_id=?";
	}
	else
	{
		sql = "insert into monitor_settings (control_monitor, left_monitor, center_monitor, right_monitor, experiment_id)"
			" values(?, ?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(settings->getControlMonitorIndex());
	q.addBindValue(settings->getLeftMonitorIndex());
	q.addBindValue(settings->getCenterMonitorIndex());
	q.addBindValue(settings->getRightMonitorIndex());
	if(settings->getId() > 0)
	{
		q.addBindValue(settings->getId());
	}
	q.addBindValue(experimentId);
	q.exec();
	if (!q.lastError().isValid())
	{
		if (settings->getId() < 1) settings->setId(q.lastInsertId().toInt());
		return true;
	}
	qDebug() << "MainDao::addOrUpdateMonitorSettings sql error: " << q.lastError().text();
	return false;
}

bool MainDao::addOrUpdateAttentionGetterSetting(int experimentId, Habit::AttentionGetterSettings* settings)
{
	QString sql;
	if(settings->getId() > 0)
	{
		sql = "update attention_setup set use_attention_stimulus=?, stimulus_name=?, background_color=?, use_fixed_isi=?, isi_ms=?"
			" where id=? and experiment_id=?";
	}
	else
	{
		sql = "insert into attention_setup (use_attention_stimulus, stimulus_name, background_color, use_fixed_isi, isi_ms, experiment_id)"
			" values(?, ?, ?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(settings->isAttentionGetterUsed());
	q.addBindValue(settings->getAttentionGetterStimulus().getName());
	q.addBindValue(settings->getBackGroundColor().name());
	q.addBindValue(settings->isFixedISI());
	q.addBindValue(settings->getFixedISIMS());
	if(settings->getId() > 0)
	{
		q.addBindValue(settings->getId());
	}
	q.addBindValue((uint)experimentId);
	q.exec();
	if(settings->getId() < 1 && !q.lastError().isValid())
	{
		settings->setId(q.lastInsertId().toInt());
	}
	Habit::StimulusSettings ss = settings->getAttentionGetterStimulus();
	if (q.lastError().isValid())
	{
		qDebug() << "MainDao::addOrUpdateMonitorSettings sql error: " << q.lastError().text();
		return false;
	}
	else
		return addOrUpdateStimulusSettings(experimentId, ss);
}

bool MainDao::addOrUpdateControlBarOption(size_t experimentId, Habit::ControlBarOptions* settings)
{
	QString sql;
	if(settings->getId() > 0)
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
	q.addBindValue(settings->isControlBarUsed());
	q.addBindValue(settings->isCurrentExperimentDisplayed());
	q.addBindValue(settings->isCurrentStimulusDisplayed());
	q.addBindValue(settings->isLookingDirectionDisplayed());
	if(settings->getId() > 0) {
		q.addBindValue(settings->getId());
	}
	q.addBindValue((uint)experimentId);
	q.exec();

	if (!q.lastError().isValid())
	{
		if (settings->getId() < 1) settings->setId(q.lastInsertId().toInt());
		return true;
	}
	qDebug() << "MainDao::addOrUpdateControlBarOption sql error: " << q.lastError().text();
	return false;
}


bool MainDao::addOrUpdateHabituationSettings(int phaseId, Habit::HabituationSettings* settings)
{
	QString sql;
	if(settings->getId() > 0)
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
	q.addBindValue(settings->getHabituationType().number());
	Habit::CriterionSettings cs = settings->getCriterionSettings();
	q.addBindValue(cs.getBasis().number());
	q.addBindValue(cs.getPercent());
	q.addBindValue(cs.getWindowSize());
	q.addBindValue(cs.getWindowType().number());
	q.addBindValue(settings->getTotalLookLengthToEnd());
	q.addBindValue(cs.getExcludeBasisWindow() ? 1 : 0);
	q.addBindValue(cs.getRequireMinBasisValue() ? 1 : 0);
	q.addBindValue(cs.getMinBasisValue());
	q.addBindValue(phaseId);
	q.addBindValue(settings->getNTrials());
	if(settings->getId() > 0)
	{
		q.addBindValue(settings->getId());
	}
	q.exec();
	if (!q.lastError().isValid())
	{
		if (settings->getId() < 1) settings->setId(q.lastInsertId().toInt());
		return true;
	}
	qDebug() << "MainDao::addOrUpdateHabituationSettings sql error: " << q.lastError().text();
	return false;
}

bool MainDao::deleteStimulus(const QString& table_name, int id)
{
	QString sql = "delete from " + table_name + " where id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(id);
	q.exec();
	if (!q.lastError().isValid())
		return true;
	qDebug() << "MainDao::deleteStimulus  sql error: " << q.lastError().text();
	return false;
}

bool MainDao::deleteOrder(int id)
{
	QString sql = "delete from stimulus_order where id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(id);
	q.exec();
	if (!q.lastError().isValid())
		return true;
	qDebug() << "MainDao::deleteOrder  sql error: " << q.lastError().text();
	return false;
}

bool MainDao::addOrUpdateStimuliSettings(int phaseId, Habit::StimuliSettings& stimuli)
{
	bool result = true;
	QSet<int> original_ids;
	QSet<int> edited_ids;
	QSet<int> deleted_ids;

	// See if any stimuli were deleted. First, get stimuli settings from db...
	Habit::StimuliSettings original;
	result = getStimuliSettings(phaseId, original);
	if (result)
	{

		for (int i=0; i<original.stimuli().size(); i++)
			original_ids.insert(original.stimuli()[i].getId());
		for (int i=0; i<stimuli.stimuli().size(); i++)
			edited_ids.insert(stimuli.stimuli()[i].getId());
		deleted_ids = original_ids.subtract(edited_ids);

		if (!deleted_ids.isEmpty())
		{
			qDebug() << "addOrUpdateStimuliSetting: deleting " << deleted_ids.size() << " stimuli.";
			QSetIterator<int> it(deleted_ids);
			while (result && it.hasNext())
				result = deleteStimulus(it.next());
		}
	}

	if (result)
	{
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
			while (result && it.hasNext())
				result = deleteOrder(it.next());
		}
	}

	// Now update or add what is in stimuli
	if (result)
	{
		for (int i=0; result && i<stimuli.stimuli().size(); i++)
		{
			result = addOrUpdateStimulusSettings(phaseId, stimuli.stimuli()[i]);
		}
		if (!result) qDebug() << "MainDao::addOrUpdateStimuliSettings(phaseId=" << phaseId << ") error in adding/updating stim settings";
	}


	// handle orders
	if (result)
	{
		for (int i=0; result && i<stimuli.orders().size(); i++)
		{
			result = addOrUpdateStimulusOrder(phaseId, stimuli.orders()[i]);
		}
		if (!result) qDebug() << "MainDao::addOrUpdateStimuliSetting(phaseId=" << phaseId << ") error adding/updating orders";
	}

	return result;
}

bool MainDao::addOrUpdateStimulusSettings(int phase_id, Habit::StimulusSettings& ss)
{
	bool b = false;
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
		sql = "insert into stimulus (name) values (?)";
	}
	q.prepare(sql);
	q.addBindValue(QVariant(ss.getName()));
	q.addBindValue(QVariant(phase_id));
	if (ss.getId() > 0)
		q.addBindValue(QVariant(ss.getId()));
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "MainDao::addOrUpdateStimulusSetting error: " << q.lastQuery() << " : " << q.lastError().text();
	}
	else
	{
		b = true;
		if (ss.getId() < 1)
		{
			ss.setId(q.lastInsertId().toInt());
		}
	}

	if (b)
	{
		// add/update stimulus info (these go into stimfiles table)
		//qDebug() << "add/update " << ss.getName() << "/" << ss.getId() << " info id l " << ss.getLeftStimulusInfo().getId() << " c " << ss.getCenterStimulusInfo().getId() << " r " << ss.getRightStimulusInfo().getId() << " s " << ss.getIndependentSoundInfo().getId();
		b = addOrUpdateStimulusInfo(ss.getId(), HPlayerPositionType::Left, ss.getLeftStimulusInfo());
		if (b) b = addOrUpdateStimulusInfo(ss.getId(), HPlayerPositionType::Center, ss.getCenterStimulusInfo());
		if (b) b = addOrUpdateStimulusInfo(ss.getId(), HPlayerPositionType::Right, ss.getRightStimulusInfo());
		if (b) b = addOrUpdateStimulusInfo(ss.getId(), HPlayerPositionType::Sound, ss.getIndependentSoundInfo());
	}

	return b;
}

bool MainDao::addOrUpdateStimulusInfo(int stimulus_id, const HPlayerPositionType& position, StimulusInfo& info)
{
	bool b = false;
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

	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "MainDao::addOrUpdateStimulusSettings error: " << q.lastQuery() << " : " << q.lastError().text();
	}
	else
	{
		b = true;
		if (info.getId() <= 0)
			info.setId(q.lastInsertId().toInt());
	}
	return b;
}

bool MainDao::addOrUpdateStimulusOrder(int experiment_id, Habit::HStimulusOrder& order)
{
	QString sql;
	if (order.getId() > 0)
	{
		sql = "update stimulus_order set name=?, experiment_id=?, context=?, orderlist=? where id=?";
	}
	else
	{
		sql = "insert into stimulus_order (name, experiment_id, context, orderlist) values (?, ?, ?, ?)";
	}
	QSqlQuery q;

	q.prepare(sql);
	q.addBindValue(order.getName());
	q.addBindValue(experiment_id);
	q.addBindValue(order.getContext()->number());
	q.addBindValue(order.getList().join(QString(",")));
	if (order.getId() > 0)
	{
		q.addBindValue(order.getId());
	}
	q.exec();
	if (!q.lastError().isValid())
	{
		if (order.getId() < 1) order.setId(q.lastInsertId().toInt());
		return true;
	}
	qDebug() << "MainDao::addOrUpdateStimulusOrder  sql error: " << q.lastError().text();
	return false;

}

bool MainDao::addOrUpdateStimulusDisplaySetting(size_t experimentId, Habit::StimulusDisplayInfo* settings)
{
	QString sql;
	if(settings->getId() > 0)
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
	q.addBindValue(settings->getPresentationStyle().number());
	q.addBindValue(settings->getDisplayType().number());
	q.addBindValue(settings->isOriginalAspectRatioMaintained());
	q.addBindValue(settings->getBackGroundColor());
	q.addBindValue(settings->getStimulusLayoutType().number());
	q.addBindValue(settings->getUseISS() ? 1 : 0);
	if(settings->getId() > 0)
	{
		q.addBindValue(settings->getId());
	}
	q.addBindValue((uint)experimentId);
	q.exec();

	if (!q.lastError().isValid())
	{
		if (settings->getId() < 1) settings->setId(q.lastInsertId().toInt());
		return true;
	}
	qDebug() << "MainDao::addOrUpdateStimulusDisplaySetting  sql error: " << q.lastError().text();
	return false;
}





// Retrieve Data from DB

bool MainDao::getHLookSettingsForExperiment(int experimentId, HLookSettings* lookSettings)
{
	Q_ASSERT(0 != lookSettings);
	bool b = false;
	QString sql = "select * from look_settings where experiment_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(experimentId);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getHLookSettingsForExperiment id=" << experimentId;
		qDebug() << q.lastQuery() << " : " << q.lastError();
	}
	else if(q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		lookSettings->setId(id);
		uint minLookTime = q.value(q.record().indexOf("min_look_time")).toUInt();
		lookSettings->setMinLookTime(minLookTime);
		uint minLookAwayTime = q.value(q.record().indexOf("min_lookaway_time")).toUInt();
		lookSettings->setMinLookAwayTime(minLookAwayTime);
		int iInclusiveLookTime = q.value(q.record().indexOf("inclusive_look_time")).toInt();
		lookSettings->setInclusiveLookTime(iInclusiveLookTime!=0);
		b = true;
	}
	else
	{
		qCritical() << "No look_settings record found for id=" << experimentId;
	}
	return b;
}

bool MainDao::getHPhaseSettings(int phaseId, HPhaseSettings& phaseSettings)
{
	bool b = false;
	QString sql = "select * from phase_settings where phaseId = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(phaseId);
	b = q.exec();
	if (!b)
	{
		qCritical() << "Error in MainDao::getHPhaseSettings id=" << phaseId;
		qDebug() << q.lastQuery() << " : " << q.lastError();
	}
	else if(q.next())
	{
		int index;
		phaseSettings.setId(phaseId);
		phaseSettings.setName(q.value(q.record().indexOf("name")).toString());
		phaseSettings.setSeqno(q.value(q.record().indexOf("seqno")).toInt());
		//phaseSettings->setPhaseType(getPhaseType(type));
		phaseSettings.setIsEnabled(q.value(q.record().indexOf("enabled")).toBool());
		//phaseSettings.setNTrials(q.value(q.record().indexOf("ntrials")).toInt());
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
		b = true;
	}
	else
	{
		qCritical() << "No phase_settings record found for phase id=" << phaseId;
		b = false;
	}

	return b;
}


bool MainDao::getHPhaseSettingsIDs(int experimentID, QList<int>& ids)
{
	bool b = false;
	QString sql = "select id from phase_settings where experiment_id=? order by seqno";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(experimentID);
	b = q.exec();
	if (!b)
	{
		qCritical() << "Error in MainDao::getHPhaseSettingsIDs experimentID=" << experimentID;
		qDebug() << q.lastQuery() << " : " << q.lastError();

	}
	else
	{
		ids.clear();
		while(q.next())
		{
			ids.push_back(q.value(0).toInt());
		}
	}
	return b;
}

bool MainDao::getAttentionGetterSettings(int experimentId, AttentionGetterSettings* attentionGetter)
{
	Q_ASSERT(0 != attentionGetter);
	bool b = false;
	int stimulusId;
	StimulusSettings ss;
	QSqlQuery q("select * from attention_setup where experiment_id=?");

	q.addBindValue(experimentId);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getAttentionGetterSettings, experiment_id=" << experimentId;
		qDebug() << q.lastQuery() << " : " << q.lastError();
	}
	else if (q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		bool useAttentionStimulus = q.value(q.record().indexOf("use_attention_stimulus")).toBool();
		QString backgroundColor = q.value(q.record().indexOf("background_color")).toString();
		bool bFixedISI = q.value(q.record().indexOf("use_fixed_isi")).toBool();
		int isiMS = q.value(q.record().indexOf("isi_ms")).toInt();
		attentionGetter->setId(id);
		attentionGetter->setUseAttentionGetter(useAttentionStimulus);
		attentionGetter->setBackGroundColor(QColor(backgroundColor));
		attentionGetter->setIsFixedISI(bFixedISI);
		attentionGetter->setFixedISIMS(isiMS);

		b = getAttentionStimulusId(experimentId, stimulusId);
		if (!b)
		{
			qCritical() << "Error in MainDao::getAttentionGetterSettings, cannot get ag stimulus id for experimentId=" << experimentId;
		}
		else
		{
			b = getStimulusSettings(stimulusId, ss);
			if (!b)
			{
				qCritical() << "Error in MainDao::getAttentionGetterSettings, cannot get ag stimulus for experimentId=" << experimentId << " stimulusId=" << stimulusId;
			}
			else
			{
				attentionGetter->setAttentionGetterStimulus(ss);
				b = true;
			}
		}
	}
	else
	{
		// There should be a record!
		qCritical() << "MainDao::getAttentionGetterSettings, no attention_setup record for experiment_id=" << experimentId;
	}
	return b;
}



bool MainDao::getControlBarOptionsForExperiment(size_t experimentId, ControlBarOptions* controlBarOptions)
{
	Q_ASSERT(0 != controlBarOptions);
	bool b = false;
	QString sql = "select * from controlbar_options where experiment_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getControlBarOptionsForExperiment id=" << experimentId;
		qCritical() << q.lastQuery() << " : " << q.lastError();
	}
	else if(q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		controlBarOptions->setId(id);
		size_t useControlBar = q.value(q.record().indexOf("is_used")).toBool();
		controlBarOptions->setUseControlBar(useControlBar);
		size_t displayCurrentExperiment = q.value(q.record().indexOf("display_current_experiment")).toBool();
		controlBarOptions->setDisplayCurrentExperiment(displayCurrentExperiment);
		size_t displayCurrentStimulus = q.value(q.record().indexOf("display_current_stimulus")).toBool();
		controlBarOptions->setDisplayCurrentStimulus(displayCurrentStimulus);
		int number = q.value(q.record().indexOf("display_looking_direction")).toBool();
		controlBarOptions->setDisplayLookingDirection(number);
		b = true;
	}
	else
	{
		qCritical() << "No controlbar_options record found in MainDao::getControlBarOptionsForExperiment id=" << experimentId;
	}
	return b;
}


bool MainDao::getHabituationSettingsForPhase(int phaseId, HabituationSettings* habituationSettings)
{
	int number;
	bool b = false;
	Q_ASSERT(0 != habituationSettings);
	QString sql = "select * from habituation_settings where phase_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(phaseId);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getHabituationSettingsForExperiment phaseid=" << phaseId;
		qCritical() << q.lastQuery() << " : " << q.lastError();
	}
	else if (q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		habituationSettings->setId(id);
		number = q.value(q.record().indexOf("habituation_type")).toInt();
		habituationSettings->setHabituationType(getHabituationType(number));
		number = q.value(q.record().indexOf("ntrials")).toInt();
		habituationSettings->setNTrials(number);

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

		habituationSettings->setCriterionSettings(criterionSettings);
		number = q.value(q.record().indexOf("total_look")).toInt();
		habituationSettings->setTotalLookLengthToEnd(number);
		b = true;
	}
	else
	{
		qCritical() << "No habituation_settings record found for phase id=" << phaseId;
	}
	return b;
}


bool MainDao::getStimulusDisplayInfoForExperiment(size_t experimentId, Habit::StimulusDisplayInfo* stimulusDisplayInfo)
{
	Q_ASSERT(0 != stimulusDisplayInfo);
	bool b = false;
	QString sql = "select * from stimulus_display where experiment_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getStimulusDisplayInfoForExperiment id=" << experimentId;
		qCritical() << q.lastQuery() << " : " << q.lastError();
	}
	else if (q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		stimulusDisplayInfo->setId(id);
		int presentationStyle = q.value(q.record().indexOf("presentation_style")).toInt();
		int displayType = q.value(q.record().indexOf("display_type")).toInt();
		bool isOrigAspectRatio = q.value(q.record().indexOf("is_original_aspect_ratio")).toBool();
		QString color = q.value(q.record().indexOf("background_color")).toString();
		int stimulusLayout = q.value(q.record().indexOf("stimulus_layout")).toInt();
		bool useISS = q.value(q.record().indexOf("use_iss")).toBool();
		stimulusDisplayInfo->setPresentationStyle(getPresentationStyle(presentationStyle));
		stimulusDisplayInfo->setDisplayType(getDisplayType(displayType));
		stimulusDisplayInfo->setMaintainOriginalAspectRatio(isOrigAspectRatio);
		stimulusDisplayInfo->setBackGroundColor(QColor(color));
		stimulusDisplayInfo->setStimulusLayoutType(getStimulusLayoutType(stimulusLayout));
		stimulusDisplayInfo->setUseISS(useISS);
		b = true;
	}
	else
	{
		qCritical() << "No stimulus_display record found for id=" << experimentId;
	}
	return b;
}

bool MainDao::getStimuliSettings(int phase_id, StimuliSettings& settings)
{
	bool b = false;
	StimuliSettings stimuli;

	/*
	 * Fetch rows from stimulus table
	 */

	QSqlQuery q("select id, name from stimulus where phase_id=?");
	q.addBindValue(phase_id);
	b = q.exec();
	if (!b)
	{
		qCritical() << "Error in MainDao::getStimuliSettings phase_id=" << phase_id ;
		qCritical() << q.lastQuery() << " : " << q.lastError();
	}
	else if (q.next())
	{
		do
		{
			int stimulus_id = q.value(0).toInt();
			QString stimulus_name = q.value(1).toString();
			StimulusSettings stimulus(stimulus_name);
			stimulus.setId(stimulus_id);
			if (b = getStimulusSettings(stimulus_id, settings))
			{
				stimuli.addStimulus(settings);
			}
			else
			{
				qCritical() << "Error in MainDao::getStimuliSettings phase_id=" << phase_id << " Cannot load stimulus with stimulus_id=" << stimulus_id;
			}
		} while (b && q.next());
	}

	if (!b) return b;

	// get the HStimulusOrderList

	QSqlQuery q2("select * from stimulus_order where phase_id=?");
	q2.addBindValue(phase_id);
	b = q2.exec();
	if (!b)
	{
		qCritical() << "Error getting orders in MainDao::getStimuliSettings phase_id=" << phase_id;
		qCritical() << q2.lastQuery() << " : " << q2.lastError();
	}
	else
	{
		while (b && q2.next())
		{
			Habit::HStimulusOrder order;
			QSqlRecord record = q2.record();
			if (b = getStimulusOrderFromRecord(record))
				stimuli.addOrder(order);
			else
				qCritical() << "Error in MainDao::getStimuliSettings phase_id=" << phase_id << " Cannot load stimulus orders with stimulus_id=" << stimulus_id;
		}
	}
	return b;
}

bool MainDao::getAttentionStimulusId(int experiment_id, int &stimulus_id)
{
	bool b = false;
	QSqlQuery q("select stimulus_id from stimulus where experiment_id=? and phase_id=-1");
	q.addBindValue(experiment_id);
	b = q.exec();
	if (!b)
	{
		qCritical() << "Error in MainDao::getAttentionStimulusId experiment_id=" << experiment_id ;
		qCritical() << q.lastQuery() << " : " << q.lastError();

		//throw HDBException(QString("Error in MainDao::getAttentionStimulusId experiment_id=%1").arg(experiment_id));	// TODO All the getmethods should return void and throw exceptions!
	}
	else if (q.next())
	{
		if (q.size() > 1)
		{
			qCritical() << "Error in MainDao::getAttentionStimulusId experiment_id=" << experiment_id << ": There are " << q.size() << " AG stimuli." ;
		}
		else
		{
			stimulus_id = q.value(0).toInt();
			b = true;
		}
	}
	return b;
}

bool MainDao::getStimulusSettings(int stimulus_id, StimulusSettings& settings)
{
	bool b = false;
	QSqlQuery qsf("select id, position, filename, isLoop, volume, isBackground, isColor, color from stimfiles where stimulus_id=?");
	qsf.addBindValue(stimulus_id);
	if (!qsf.exec())
	{
		qCritical() << "Error in MainDao::getStimulusSettings stimulus_id=" << stimulus_id << "No rows found!";
	}
	else
	{
		while (qsf.next())
		{
			StimulusInfo info(qsf.value(2).toString(), qsf.value(3).toBool(), qsf.value(4).toInt(), qsf.value(5).toBool(), qsf.value(6).toBool(), qsf.value(7).toString());
			info.setId(qsf.value(0).toInt());
			settings.setStimulusInfo(info, getPlayerPositionType(qsf.value(1).toInt()));
		}
		b = true;
	}
	return b;
}


bool MainDao::deleteStimulus(int stimulus_id)
{
	bool b = false;
	QSqlQuery q("delete from stimfiles where stimulus_id=?");
	q.addBindValue(QVariant(stimulus_id));
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::deleteStimulus stimulus_id=" << stimulus_id;
		qCritical() << q.lastQuery() << " : " << q.lastError();
		b = false;
	}
	else
	{
		QSqlQuery q2("delete from stimulus where id=?");
		q2.addBindValue(QVariant(stimulus_id));
		q2.exec();
		if (q2.lastError().isValid())
		{
			qCritical() << "Error in MainDao::deleteStimulus stimulus_id=" << stimulus_id;
			qCritical() << q2.lastQuery() << " : " << q2.lastError();
			b = false;
		}
		else
		{
			b = true;
		}
	}
	return b;
}



Habit::HStimulusOrder MainDao::getStimulusOrder(const QSqlQuery& q)
{
	Habit::HStimulusOrder order;
	order.setContext(getStimContext(q.value(q.record().indexOf("context")).toInt()));
	order.setId(q.value(q.record().indexOf("id")).toInt());
	order.setName(q.value(q.record().indexOf("name")).toString());
	order.setList(q.value(q.record().indexOf("orderlist")).toString().split(QString(",")));
	return order;
}

bool MainDao::getStimulusOrderFromRecord(const QSqlRecord& record, Habit::StimulusOrder& order)
{
	bool b=false;
	order.setId(q.value(record.indexOf("id")).toInt());
	order.setPhaseId(record.indexOf("phase_id"));
	order.setName(record.indexOf("name")).toString());
	order.setList(record.indexOf("orderlist")).toString().split(QString(",")));
	return b;
}

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


bool MainDao::updateExperimentName(const QString& oldName, const QString& newName)
{
	bool b=false;
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
			b = false;
		}
		else
		{
			b = true;
		}
	}
	else
	{
		qCritical() << "cannot update experiment name from " << oldName << " to " << newName << " Experiment \"" << oldName<< "\" doesn't exist";
		b = false;
	}
	return b;
}

int MainDao::getExperimentId(const QString& name)
{
	int result = -1;
	QString sql = "select id from experiments where name=?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(name);
	q.exec();
	if (q.next())
	{
		result = q.value(q.record().indexOf("id")).toInt();
	}
	return result;
}

bool MainDao::getExperimentName(int id, QString& name)
{
	bool b = false;
	name = "";
	QSqlQuery q("select name from experiments where id=?");
	q.addBindValue(id);
	if (q.exec())
	{
		if (q.next())
		{
			name = q.value(q.record().indexOf("name")).toString();
			b = true;
		}
		else
		{
			qError() << "MainDao::getExperimentName(): Cannot find record in experiments table for id=" << id;
		}
	}
	else
	{
		qError() << "MainDao::getExperimentName(): " << q.lastQuery() << " : error - " << q.lastError().text();
	}
	return b;
}

bool MainDao::getExperimentID(const QString& name, int& id)
{
	bool b = false;
	id = -1;
	QSqlQuery q("select id from experiments where name=?");
	q.addBindValue(name);
	if (q.exec())
	{
		if (q.next())
		{
			id = q.value(q.record().indexOf("id")).toInt();
			b = true;
		}
		else
		{
			qError() << "Cannot find record in experiments table for name=" << name;
		}
	}
	else
	{
		qError() << "MainDao::getExperimentID(): " << q.lastQuery() << " : error - " << q.lastError().text();
	}
	return b;
}


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

bool MainDao::insertOrUpdateExperimentSettings(Habit::ExperimentSettings* experimentSettings)
{
	bool result = false;
	if(0 != experimentSettings) {
		QString name = experimentSettings->getName();
		int id = experimentSettings->getId();
		int hidden = experimentSettings->isHidden() ? 1 : 0;
		QSqlQuery q;
		if(id > 0) {
			QString sql = "update experiments set name=?, hidden=? where id=?";
			q.prepare(sql);
			q.addBindValue(name);
			q.addBindValue(hidden);
			q.addBindValue(id);
			q.exec();
		} else {
			QString sql = "INSERT INTO experiments (name, hidden) VALUES (?, ?)";
			q.prepare(sql);
			q.addBindValue(name);
			q.addBindValue(hidden);
			q.exec();
			experimentSettings->setId(q.lastInsertId().toInt());
		}
		result = !q.lastError().isValid();
	}
	return result;
}

bool MainDao::deleteFromTable(const QString table, const QString key, int id)
{
	QString s = QString("delete from %1 where %2=%3").arg(table).arg(key).arg(id);
	QSqlQuery q(s);
	qDebug() << "deleteFromTable: " << s << (q.lastError().isValid() ? "ERROR" : "OK");
	if (!q.lastError().isValid())
	{
		return true;
	}
	qDebug() << "deleteFromTable(table=" << table << ",key=" << key << ",id=" << id << "): sql error " << q.lastError().text();
	return false;
}

bool MainDao::deleteExperimentSettings(Habit::ExperimentSettings* settings)
{
	bool result = false;
	Q_ASSERT(0 != settings);
	if (settings->getId()>0)
	{
		result = 	deleteFromTable("experiments", "id", settings->getId()) &&
					deleteFromTable("attention_setup", "experiment_id", settings->getId()) &&
					deleteFromTable("controlbar_options", "experiment_id", settings->getId()) &&
					deleteFromTable("look_settings", "experiment_id", settings->getId()) &&
					deleteFromTable("phase_settings", "experiment_id", settings->getId()) &&
					deleteFromTable("habituation_settings", "experiment_id", settings->getId()) &&
					deleteFromTable("stimulus_display", "experiment_id", settings->getId()) &&
					deleteFromTable("run_settings", "experiment_id", settings->getId());

		// TODO: Must delete from tables "stimulus" and "stimfiles"
	}
	else
	{
		qDebug() << "Cannot delete experiment \"" << settings->getName() << "\" -- id<=0!";
	}
	return result;
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
