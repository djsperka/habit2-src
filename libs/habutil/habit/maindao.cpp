
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
				"set enabled=?, phase_type=?, ntrials=?, "
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
				"enabled, phase_type, ntrials, use_looking_criteria, is_single_look, is_max_accumulated_look_time, "
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
	q.addBindValue(settings->getPhaseType().number());
	q.addBindValue(settings->getNTrials());
	q.addBindValue(settings->getUseLookingCriteria() ? 1 : 0);
	q.addBindValue(settings->getIsSingleLook() ? 1 : 0);
	q.addBindValue(settings->getIsMaxAccumulatedLookTime() ? 1 : 0);
	q.addBindValue(settings->getMaxAccumulatedLookTime());
	q.addBindValue(settings->getIsMaxLookAwayTime() ? 1 : 0);
	q.addBindValue(settings->getMaxLookAwayTime());
	q.addBindValue(settings->getRepeatTrialOnMaxLookAwayTime() ? 1 : 0);
	qDebug() << "MainDao::addOrUpdateHPhaseSettings(" << experimentId << ", " << settings->getPhaseType().number() << ") repeat on max lookaway " << (settings->getRepeatTrialOnMaxLookAwayTime() ? 1 : 0);
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
		sql = "update attention_setup set use_attention_stimulus=?, stimulus_name=?, background_color=?"
			" where id=? and experiment_id=?";
	}
	else
	{
		sql = "insert into attention_setup (use_attention_stimulus, stimulus_name, background_color, experiment_id)"
			" values(?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(settings->isAttentionGetterUsed());
	q.addBindValue(settings->getAttentionGetterStimulus().getName());
	q.addBindValue(settings->getBackGroundColor().name());
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


bool MainDao::addOrUpdateHabituationSetting(size_t experimentId, Habit::HabituationSettings* settings)
{
	QString sql;
	if(settings->getId() > 0)
	{
		sql = "update habituation_settings set habituation_type=?, criterion_basis=?, criterion_percent=?, window_size=?, window_type=?, total_look=?, exclude_basis_window=?, require_min_basis_value=?, min_basis_value=?"
			" where id=? and experiment_id=?";
	}
	else
	{
		sql = "insert into habituation_settings (habituation_type, criterion_basis, criterion_percent, window_size, window_type, total_look, exclude_basis_window, require_min_basis_value, min_basis_value, experiment_id)"
			" values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
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

bool MainDao::addOrUpdateStimuliSettings(int experimentId, Habit::StimuliSettings& stimuli)
{
	bool result = true;
	QSet<int> original_ids;
	QSet<int> edited_ids;
	QSet<int> deleted_ids;

	// See if any stimuli were deleted. First, get stimuli settings from db...
	Habit::StimuliSettings original = getStimuliSettings(experimentId, stimuli.getStimContext());

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
			result = addOrUpdateStimulusSettings(experimentId, stimuli.stimuli()[i]);
		}
		if (!result) qDebug() << "MainDao::addOrUpdateStimuliSettings(" << stimuli.getStimContext().name() << ") error in adding/updating stim settings";
	}


	// handle orders
	if (result)
	{
		for (int i=0; result && i<stimuli.orders().size(); i++)
		{
			result = addOrUpdateStimulusOrder(experimentId, stimuli.orders()[i]);
		}
		if (!result) qDebug() << "MainDao::addOrUpdateStimuliSetting(" << stimuli.getStimContext().name() << ") error adding/updating orders";
	}

	return result;
}

bool MainDao::addOrUpdateStimulusSettings(int experiment_id, Habit::StimulusSettings& ss)
{
	bool b = false;
	QString sql;
	QSqlQuery q;
	if (ss.getId() > 0)
	{
		// update stimulus table
		sql = "update stimulus set name=?, experiment_id=?, context=? where id=?";
	}
	else
	{
		// insert into stimulus table. Make sure to set Id in ss object - it will be used to add/update stimfiles.
		sql = "insert into stimulus (name, experiment_id, context) values (?, ?, ?)";
	}
	q.prepare(sql);
	q.addBindValue(QVariant(ss.getName()));
	q.addBindValue(QVariant(experiment_id));
	q.addBindValue(QVariant(ss.getContext()->number()));
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


#if 0
bool MainDao::addOrUpdateStimuliSetting(size_t experiment_id, Habit::StimuliSettings* settings)
{
	bool result = false;
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

	if(!table_name.isEmpty())
	{
		result = true;
		QSet<int> original_ids;
		QSet<int> edited_ids;
		QSet<int> deleted_ids;

		// See if any stimuli were deleted. First, get stimuli settings from db...
		Habit::StimuliSettings original = getStimuliSettings(table_name, experiment_id, settings->getStimContext());

		for (int i=0; i<original.stimuli().size(); i++)
			original_ids.insert(original.stimuli()[i].getId());
		for (int i=0; i<settings->stimuli().size(); i++)
			edited_ids.insert(settings->stimuli()[i].getId());
		deleted_ids = original_ids.subtract(edited_ids);

		if (!deleted_ids.isEmpty())
		{
			qDebug() << "addOrUpdateStimuliSetting: deleting " << deleted_ids.size() << " stimuli from table " << table_name;
			QSetIterator<int> it(deleted_ids);
			while (result && it.hasNext())
				result = deleteStimulus(table_name, it.next());
		}

		if (result)
		{
			// See if any orders were deleted.
			original_ids.clear();
			edited_ids.clear();
			deleted_ids.clear();

			for (int i=0; i<original.orders().size(); i++)
				original_ids.insert(original.orders()[i].getId());
			for (int i=0; i<settings->orders().size(); i++)
				edited_ids.insert(settings->orders()[i].getId());
			deleted_ids = original_ids.subtract(edited_ids);

			if (!deleted_ids.isEmpty())
			{
				qDebug() << "addOrUpdateStimuliSetting: deleting " << deleted_ids.size() << " orders";
				QSetIterator<int> it(deleted_ids);
				while (result && it.hasNext())
					result = deleteOrder(it.next());
			}
		}

		// Now update or add what is in settings
		if (result)
		{
			for (int i=0; result && i<settings->stimuli().size(); i++)
			{
				result = addOrUpdateStimulusSetting(experiment_id, settings->stimuli()[i], table_name, "experiment_id");
			}
			if (!result) qDebug() << "MainDao::addOrUpdateStimuliSetting(" << settings->getStimContext().name() << ") error in adding/updating stim settings";
		}


		// handle orders
		if (result)
		{
			for (int i=0; result && i<settings->orders().size(); i++)
			{
				result = addOrUpdateStimulusOrder(experiment_id, settings->orders()[i]);
			}
			if (!result) qDebug() << "MainDao::addOrUpdateStimuliSetting(" << settings->getStimContext().name() << ") error adding/updating orders";
		}

	}
	else
	{
		result = false;
	}

	return result;
}



bool MainDao::addOrUpdateStimuliSettings(size_t experiment_id, Habit::StimuliSettings* settings)
{
	bool result = false;
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

	if(!table_name.isEmpty())
	{
		result = true;
		QSet<int> original_ids;
		QSet<int> edited_ids;
		QSet<int> deleted_ids;

		// See if any stimuli were deleted. First, get stimuli settings from db...
		Habit::StimuliSettings original = getStimuliSettings(table_name, experiment_id, settings->getStimContext());

		for (int i=0; i<original.stimuli().size(); i++)
			original_ids.insert(original.stimuli()[i].getId());
		for (int i=0; i<settings->stimuli().size(); i++)
			edited_ids.insert(settings->stimuli()[i].getId());
		deleted_ids = original_ids.subtract(edited_ids);

		if (!deleted_ids.isEmpty())
		{
			qDebug() << "addOrUpdateStimuliSetting: deleting " << deleted_ids.size() << " stimuli from table " << table_name;
			QSetIterator<int> it(deleted_ids);
			while (result && it.hasNext())
				result = deleteStimulus(table_name, it.next());
		}

		if (result)
		{
			// See if any orders were deleted.
			original_ids.clear();
			edited_ids.clear();
			deleted_ids.clear();

			for (int i=0; i<original.orders().size(); i++)
				original_ids.insert(original.orders()[i].getId());
			for (int i=0; i<settings->orders().size(); i++)
				edited_ids.insert(settings->orders()[i].getId());
			deleted_ids = original_ids.subtract(edited_ids);

			if (!deleted_ids.isEmpty())
			{
				qDebug() << "addOrUpdateStimuliSetting: deleting " << deleted_ids.size() << " orders";
				QSetIterator<int> it(deleted_ids);
				while (result && it.hasNext())
					result = deleteOrder(it.next());
			}
		}

		// Now update or add what is in settings
		if (result)
		{
			for (int i=0; result && i<settings->stimuli().size(); i++)
			{
				result = addOrUpdateStimulusSetting(experiment_id, settings->stimuli()[i], table_name, "experiment_id");
			}
			if (!result) qDebug() << "MainDao::addOrUpdateStimuliSetting(" << settings->getStimContext().name() << ") error in adding/updating stim settings";
		}


		// handle orders
		if (result)
		{
			for (int i=0; result && i<settings->orders().size(); i++)
			{
				result = addOrUpdateStimulusOrder(experiment_id, settings->orders()[i]);
			}
			if (!result) qDebug() << "MainDao::addOrUpdateStimuliSetting(" << settings->getStimContext().name() << ") error adding/updating orders";
		}

	}
	else
	{
		result = false;
	}

	return result;
}

#endif



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

#if 0
bool MainDao::addOrUpdateStimulusSetting(int parentId, Habit::StimulusSettings& ss, const QString& tableName, const QString& parentKeyName)
{
	QString sql;
	if(ss.getId() > 0)
	{
		sql = "update " + tableName + " set name=?, is_left_background=?, left_filename=?, is_left_loop_playback=?, is_center_background=?, "
			" center_filename=?, is_center_loop_playback=?, is_right_background=?, right_filename=?, is_right_loop_playback=?, "
			" independent_sound_filename=?, is_independent_loop_playback=?, left_audio_balance=?, right_audio_balance=?, center_audio_balance=?,"
			" independent_audio_balance=?"
			" where id=? and " + parentKeyName + "=?";
	}
	else
	{
		sql = "insert into " + tableName + " (name, is_left_background, left_filename, is_left_loop_playback, is_center_background, "
			" center_filename, is_center_loop_playback, is_right_background, right_filename, is_right_loop_playback, "
			" independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, "
			" independent_audio_balance, " + parentKeyName + ")"
			" values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
	}
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(ss.getName());
	q.addBindValue(ss.getLeftStimulusInfo().isBackground());
	q.addBindValue(ss.getLeftStimulusInfo().getFileName());
	q.addBindValue(ss.getLeftStimulusInfo().isLoopPlayBack());
	q.addBindValue(ss.getCenterStimulusInfo().isBackground());
	q.addBindValue(ss.getCenterStimulusInfo().getFileName());
	q.addBindValue(ss.getCenterStimulusInfo().isLoopPlayBack());
	q.addBindValue(ss.getRightStimulusInfo().isBackground());
	q.addBindValue(ss.getRightStimulusInfo().getFileName());
	q.addBindValue(ss.getRightStimulusInfo().isLoopPlayBack());
	q.addBindValue(ss.getIndependentSoundInfo().getFileName());
	q.addBindValue(ss.getIndependentSoundInfo().isLoopPlayBack());
	q.addBindValue(ss.getLeftStimulusInfo().getAudioBalance());
	q.addBindValue(ss.getRightStimulusInfo().getAudioBalance());
	q.addBindValue(ss.getCenterStimulusInfo().getAudioBalance());
	q.addBindValue(ss.getIndependentSoundInfo().getAudioBalance());
	if (ss.getId() > 0)
	{
		q.addBindValue(ss.getId());
	}
	q.addBindValue(parentId);
	q.exec();
	if (!q.lastError().isValid())
	{
		if (ss.getId() < 1) ss.setId(q.lastInsertId().toInt());
		return true;
	}
	qDebug() << "MainDao::addOrUpdateStimulusSetting  sql error: " << q.lastError().text();
	return false;
}

#endif


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

void MainDao::getHLookSettingsForExperiment(int experimentId, HLookSettings* lookSettings)
{
	Q_ASSERT(0 != lookSettings);
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
	}
	else
	{
		qCritical() << "No look_settings record found for id=" << experimentId;
	}
}

void MainDao::getHPhaseSettingsForExperiment(int experimentId, int type, HPhaseSettings* phaseSettings)
{
	Q_ASSERT(0 != phaseSettings);
	QString sql = "select * from phase_settings where experiment_id = ? and phase_type = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(experimentId);
	q.addBindValue(type);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getHPhaseSettingsForExperiment id=" << experimentId << " for phase " << getPhaseType(type).name();
		qDebug() << q.lastQuery() << " : " << q.lastError();
	}
	else if(q.next())
	{
		phaseSettings->setId(q.value(q.record().indexOf("id")).toInt());
		phaseSettings->setPhaseType(getPhaseType(type));
		phaseSettings->setIsEnabled(q.value(q.record().indexOf("enabled")).toBool());
		phaseSettings->setNTrials(q.value(q.record().indexOf("ntrials")).toInt());
		phaseSettings->setUseLookingCriteria(q.value(q.record().indexOf("use_looking_criteria")).toBool());
		phaseSettings->setIsSingleLook(q.value(q.record().indexOf("is_single_look")).toBool());
		phaseSettings->setIsMaxAccumulatedLookTime(q.value(q.record().indexOf("is_max_accumulated_look_time")).toBool());
		phaseSettings->setMaxAccumulatedLookTime(q.value(q.record().indexOf("max_accumulated_look_time")).toUInt());
		phaseSettings->setIsMaxLookAwayTime(q.value(q.record().indexOf("is_max_lookaway_time")).toBool());
		phaseSettings->setMaxLookAwayTime(q.value(q.record().indexOf("max_lookaway_time")).toUInt());
		phaseSettings->setRepeatTrialOnMaxLookAwayTime(q.value(q.record().indexOf("repeat_trial_on_max_lookaway")).toBool());
		phaseSettings->setIsMaxStimulusTime(q.value(q.record().indexOf("is_max_stimulus_time")).toBool());
		phaseSettings->setMaxStimulusTime(q.value(q.record().indexOf("max_stimulus_time")).toUInt());
		phaseSettings->setMeasureStimulusTimeFromOnset(q.value(q.record().indexOf("measure_from_onset")).toBool());
		phaseSettings->setMeasureStimulusTimeFromLooking(q.value(q.record().indexOf("measure_from_looking")).toBool());
		phaseSettings->setIsMaxNoLookTime(q.value(q.record().indexOf("is_max_no_look_time")).toBool());
		phaseSettings->setMaxNoLookTime(q.value(q.record().indexOf("max_no_look_time")).toUInt());
	}
	else
	{
		qCritical() << "No phase_settings record found for id=" << experimentId << " for phase " << getPhaseType(type).name();
	}
}

void MainDao::getMonitorSettingsForExperiment(size_t experimentId, MonitorSettings* monitorSettings)
{
	Q_ASSERT(0 != monitorSettings);
	QString sql = "select * from monitor_settings where experiment_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
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

void MainDao::getAttentionGetterSettings(int experimentId, AttentionGetterSettings* attentionGetter)
{
	Q_ASSERT(0 != attentionGetter);
	StimuliSettings dummyStimuliSettings = getStimuliSettings(experimentId, HStimContext::AttentionGetter);
	Q_ASSERT(dummyStimuliSettings.stimuli().count() == 1);
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
		attentionGetter->setId(id);
		attentionGetter->setUseAttentionGetter(useAttentionStimulus);
		attentionGetter->setBackGroundColor(QColor(backgroundColor));
		attentionGetter->setAttentionGetterStimulus(dummyStimuliSettings.stimuli().at(0));
	}
	else
	{
		// There should be a record!
		qCritical() << "MainDao::getAttentionGetterSettings, no attention_setup record for experiment_id=" << experimentId;
	}
}



#if 0
void MainDao::getAttentionGetterSettingsForExperiment(size_t experimentId, AttentionGetterSettings* attentionGetter)
{
	Q_ASSERT(0 != attentionGetter);
	QString sql = "select * from attention_setup where experiment_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getAttentionGetterSettingsForExperiment id=" << experimentId;
		qDebug() << q.lastQuery() << " : " << q.lastError();
	}
	else if(q.next())
	{
		int id = q.value(q.record().indexOf("id")).toInt();
		attentionGetter->setId(id);
		size_t useAttentionStimulus = q.value(q.record().indexOf("use_attention_stimulus")).toBool();
		attentionGetter->setUseAttentionGetter(useAttentionStimulus);
		QString backgroundColor = q.value(q.record().indexOf("background_color")).toString();
		attentionGetter->setBackGroundColor(QColor(backgroundColor));
		sql = "select * from attention_getting_stimuli where attention_getter_id = ?";
		QSqlQuery q2;
		q2.prepare(sql);
		q2.addBindValue(id);
		q2.exec();
		if (q2.lastError().isValid())
		{
			qCritical() << "Error in MainDao::getAttentionGetterSettingsForExperiment id=" << experimentId;
			qCritical() << q2.lastQuery() << " : " << q2.lastError();
		}
		else if(q2.next())
		{
			Habit::StimulusSettings ss = getStimulusSettings(q2);
			attentionGetter->setAttentionGetterStimulus(ss);
		}
		else
		{
			qCritical() << "No attention_getting_stimuli record found in MainDao::getAttentionGetterSettingsForExperiment id=" << experimentId;
		}
	}
	else
	{
		qCritical() << "No attention_setup record found in MainDao::getAttentionGetterSettingsForExperiment id=" << experimentId;
	}
}
#endif

void MainDao::getControlBarOptionsForExperiment(size_t experimentId, ControlBarOptions* controlBarOptions)
{
	Q_ASSERT(0 != controlBarOptions);
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
	}
	else
	{
		qCritical() << "No controlbar_options record found in MainDao::getControlBarOptionsForExperiment id=" << experimentId;
	}
}


void MainDao::getHabituationSettingsForExperiment(size_t experimentId, HabituationSettings* habituationSettings)
{
	int number;
	Q_ASSERT(0 != habituationSettings);
	QString sql = "select * from habituation_settings where experiment_id = ?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue((uint)experimentId);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getHabituationSettingsForExperiment id=" << experimentId;
		qCritical() << q.lastQuery() << " : " << q.lastError();
	}
	else if (q.next())
	{
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
		number = q.value(q.record().indexOf("exclude_basis_window")).toInt();
		criterionSettings.setExcludeBasisWindow(number==1);
		number = q.value(q.record().indexOf("require_min_basis_value")).toInt();
		criterionSettings.setRequireMinBasisValue(number==1);
		number = q.value(q.record().indexOf("min_basis_value")).toInt();
		criterionSettings.setMinBasisValue(number);

		habituationSettings->setCriterionSettings(criterionSettings);
		number = q.value(q.record().indexOf("total_look")).toInt();
		habituationSettings->setTotalLookLengthToEnd(number);
	}
	else
	{
		qCritical() << "No habituation_settings record found for id=" << experimentId;
	}
}

void MainDao::getStimulusDisplayInfoForExperiment(size_t experimentId, Habit::StimulusDisplayInfo* stimulusDisplayInfo)
{
	Q_ASSERT(0 != stimulusDisplayInfo);
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
	}
	else
	{
		qCritical() << "No stimulus_display record found for id=" << experimentId;
	}
}

StimuliSettings MainDao::getStimuliSettings(int experiment_id, const HStimContext& context)
{
	StimuliSettings stimuli(context);

	/*
	 * Fetch rows from stimulus table
	 */

	QSqlQuery q("select id, name from stimulus where experiment_id=? and context=?");
	q.addBindValue(experiment_id);
	q.addBindValue(context.number());
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getStimuliSettings experiment_id=" << experiment_id << " context=" << context.name() << "(" << context.number() << ")";
		qCritical() << q.lastQuery() << " : " << q.lastError();
	}
	else if (q.next())
	{
		do
		{
			int stimulus_id = q.value(0).toInt();
			QString stimulus_name = q.value(1).toString();
			StimulusSettings settings(stimulus_name, context);
			settings.setId(stimulus_id);
			if (getStimulusSettings(settings, stimulus_id))
			{
				stimuli.addStimulus(settings);
			}
		} while (q.next());
	}


	// get the HStimulusOrderList

	QSqlQuery q2("select * from stimulus_order where experiment_id=? and context=?");
	q2.addBindValue(experiment_id);
	q2.addBindValue(context.number());
	q2.exec();
	if (q2.lastError().isValid())
	{
		qCritical() << "Error getting orders in MainDao::getStimuliSettings id=" << experiment_id;
		qCritical() << q2.lastQuery() << " : " << q2.lastError();
	}
	else
	{
		while (q2.next())
		{
			Habit::HStimulusOrder order = getStimulusOrder(q2);
			order.setContext(context);
			stimuli.addOrder(order);
		}
	}
	return stimuli;
}

bool MainDao::getStimulusSettings(StimulusSettings& settings, int stimulus_id)
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






#if OLD_STIMULUS_CODE

	
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

Habit::StimuliSettings MainDao::getStimuliSettings(const QString& table_name, const QString& experiment, const HStimContext& context)
{
	return getStimuliSettings(table_name, getExperimentId(experiment), context);
}


Habit::StimuliSettings MainDao::getStimuliSettings(const QString& table_name, int experiment_id, const HStimContext& context)
{
	Habit::StimuliSettings result(context);

	// get the StimulusSettingsList

	QString sql = "select * from " + table_name + " where experiment_id=?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(experiment_id);
	q.exec();
	if (q.lastError().isValid())
	{
		qCritical() << "Error in MainDao::getStimuliSettings id=" << experiment_id << " table " << table_name;
		qCritical() << q.lastQuery() << " : " << q.lastError();
	}
	else
	{
		while(q.next())
		{
			Habit::StimulusSettings ss = getStimulusSettings(q);
			ss.setContext(context);
			result.addStimuli(ss);
		}
	}

	// get the HStimulusOrderList

	QString sql2("select * from stimulus_order where experiment_id=? and context=?");
	QSqlQuery q2;
	q2.prepare(sql2);
	q2.addBindValue(experiment_id);
	q2.addBindValue(context.number());
	q2.exec();
	if (q2.lastError().isValid())
	{
		qCritical() << "Error getting orders in MainDao::getStimuliSettings id=" << experiment_id;
		qCritical() << q2.lastQuery() << " : " << q2.lastError();
	}
	else
	{
		while (q2.next())
		{
			Habit::HStimulusOrder order = getStimulusOrder(q2);
			order.setContext(context);
			result.addOrder(order);
		}
	}
	return result;
}
#endif


Habit::HStimulusOrder MainDao::getStimulusOrder(const QSqlQuery& q)
{
	Habit::HStimulusOrder order;
	order.setContext(getStimContext(q.value(q.record().indexOf("context")).toInt()));
	order.setId(q.value(q.record().indexOf("id")).toInt());
	order.setName(q.value(q.record().indexOf("name")).toString());
	order.setList(q.value(q.record().indexOf("orderlist")).toString().split(QString(",")));
	return order;
}


#if OLD_STIMULUS_CODE

Habit::StimulusSettings MainDao::getStimulusSettings(const QSqlQuery& q)
{
	Habit::StimulusSettings ss;
	QString name = q.value(q.record().indexOf("name")).toString();

	bool is_left_background = q.value(q.record().indexOf("is_left_background")).toBool();
	QString left_filename = q.value(q.record().indexOf("left_filename")).toString();
	bool is_left_loop_playback = q.value(q.record().indexOf("is_left_loop_playback")).toBool();

	bool is_center_background = q.value(q.record().indexOf("is_center_background")).toBool();
	QString center_filename = q.value(q.record().indexOf("center_filename")).toString();
	bool is_center_loop_playback = q.value(q.record().indexOf("is_center_loop_playback")).toBool();

	bool is_right_background = q.value(q.record().indexOf("is_right_background")).toBool();
	QString right_filename = q.value(q.record().indexOf("right_filename")).toString();
	bool is_right_loop_playback = q.value(q.record().indexOf("is_right_loop_playback")).toBool();

	QString independent_sound_filename = q.value(q.record().indexOf("independent_sound_filename")).toString();
	bool is_independent_loop_playback = q.value(q.record().indexOf("is_independent_loop_playback")).toBool();

	int left_audio_balance = q.value(q.record().indexOf("left_audio_balance")).toInt();
	int center_audio_balance = q.value(q.record().indexOf("center_audio_balance")).toInt();
	int right_audio_balance = q.value(q.record().indexOf("right_audio_balance")).toInt();
	int independent_audio_balance = q.value(q.record().indexOf("independent_audio_balance")).toInt();

	int id = q.value(q.record().indexOf("id")).toInt();

	ss.setId(id);
	ss.setName(name);

	Habit::StimulusInfo si_left(name, left_filename, is_left_loop_playback, left_audio_balance, is_left_background);
	Habit::StimulusInfo si_center(name, center_filename, is_center_loop_playback, center_audio_balance, is_center_background);
	Habit::StimulusInfo si_right(name, right_filename, is_right_loop_playback, right_audio_balance, is_right_background);
	Habit::StimulusInfo si_ind(name, independent_sound_filename, is_independent_loop_playback, independent_audio_balance);	// no background setting for sound

	ss.setLeftStimulusInfo(si_left);
	ss.setCenterStimulusInfo(si_center);
	ss.setRightStimulusInfo(si_right);
	ss.setIndependentSoundInfo(si_ind);

	return ss;
}

#endif





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
					(settings->getAttentionGetterSettings().getId() <= 0 ||
							deleteFromTable("attention_getting_stimuli", "attention_getter_id", settings->getAttentionGetterSettings().getId())) &&
					deleteFromTable("controlbar_options", "experiment_id", settings->getId()) &&
					deleteFromTable("look_settings", "experiment_id", settings->getId()) &&
					deleteFromTable("phase_settings", "experiment_id", settings->getId()) &&
					deleteFromTable("habituation_settings", "experiment_id", settings->getId()) &&
					deleteFromTable("stimulus_display", "experiment_id", settings->getId()) &&
					deleteFromTable("pretest_stimuli", "experiment_id", settings->getId()) &&
					deleteFromTable("habituation_stimuli", "experiment_id", settings->getId()) &&
					deleteFromTable("test_stimuli", "experiment_id", settings->getId()) &&
					deleteFromTable("run_settings", "experiment_id", settings->getId());
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

QStringList MainDao::getAllSubjectsNames()
{
	QSqlQuery q("select name from subject_settings");
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
	QString sql = "insert into subject_settings (name, birthdate, testdate, condition, observer, cell_number, comments)"
		" values(?, ?, ?, ?, ?, ?, ?)";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(subjectSettings->getSubjectName());
	q.addBindValue(subjectSettings->getBirthDate());
	q.addBindValue(subjectSettings->getTestDate());
	q.addBindValue(subjectSettings->getCondition());
	q.addBindValue(subjectSettings->getObserver());
	q.addBindValue(subjectSettings->getCellNumber());
	q.addBindValue(subjectSettings->getComments());
	q.exec();
	subjectSettings->setId(q.lastInsertId().toInt());
}

void MainDao::updateSubject( const Habit::SubjectSettings& subjectSettings )
{
	QString sql = "update subject_settings set name=?, birthdate=?, testdate=?, condition=?, observer=?, cell_number=?, comments=?"
		" where id=?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(subjectSettings.getSubjectName());
	q.addBindValue(subjectSettings.getBirthDate());
	q.addBindValue(subjectSettings.getTestDate());
	q.addBindValue(subjectSettings.getCondition());
	q.addBindValue(subjectSettings.getObserver());
	q.addBindValue(subjectSettings.getCellNumber());
	q.addBindValue(subjectSettings.getComments());
	q.addBindValue(subjectSettings.getId());
	q.exec();
}

bool MainDao::isSubjectUnique(const Habit::SubjectSettings& subjectSettings)
{
	QString sql = "select * from subject_settings where name=?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(subjectSettings.getSubjectName());
	q.exec();
	return !q.next();
}

Habit::SubjectSettings MainDao::getSubjectSettings(const QString& subj) {
	QString sql = "select * from subject_settings where name=?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(subj);
	q.exec();
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
	QString sql = "select * from subject_settings where id=?";
	QSqlQuery q;
	q.prepare(sql);
	q.addBindValue(id);
	q.exec();
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

} // namespace Habit
