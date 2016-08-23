/*
 * HDBUtil.cpp
 *
 *  Created on: Aug 8, 2013
 *      Author: Oakeslab
 */


#include "HDButil.h"
#include "HTypes.h"
#include "maindao.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QtDebug>
#include <QDateTime>

bool populateStimulusTable(QSqlQuery& q, const HStimContext& context);
bool populateStimulusTableSingle(QSqlQuery& q, const HStimContext& context, int& stimulus_id);
static bool validateExperiment(int exptId);
static bool validateStimulus(int stimulus_id);

// Close database on default connection.

void closeDB()
{
	if (QSqlDatabase::database().isOpen())
	{
		QSqlDatabase::database().close();
		QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
	}
}

// Open the database file "habit.sqlite" in the given directory.

bool openDB(const QDir& dir)
{
	bool bval = false;
	QString filename;
	QFileInfo fileinfo;


	// close database if one is open
	closeDB();

	// check file attributes
	fileinfo.setFile(dir, "habit.sqlite");

	// Does file exist and can we write to it? If not, then get filename
	if (!fileinfo.exists() || !fileinfo.isWritable())
	{
		QMessageBox msgBox;
		QString msg("The database file \"habit.sqlite\" exists, but you do not have write permission. Please fix permissions or select another workspace.");
		msgBox.setText(msg);
		msgBox.exec();
	}
	else
	{
		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName(fileinfo.absoluteFilePath());
		qDebug() << "Opening database: " << fileinfo.absoluteFilePath();
		if(db.open())
		{
			bval = true;

			// Now update database if necessary. Do NOT change the database name!
			if (!updateDBVersion(db, fileinfo))
			{
				bval = false;
				qDebug() << "Database update failed.";
			}
			else
			{
				validateDB();
			}
		}
		else
		{
			qDebug() << "Database file open failed.";
		}
	}
	return bval;
}


bool validateStimulus(int stimulus_id)
{
	Habit::MainDao dao;
	bool bval = false;
	QList<QVariant> list;
	list = dao.getColumnValuesFromTable("stimulus", "id", "id", stimulus_id);
	if (list.size() == 1)
	{
//		qDebug() << "  found 1 stimulus record...OK...check stimfiles...";
		list = dao.getColumnValuesFromTable("stimfiles", "position", "stimulus_id", stimulus_id);
		if (list.size() == 4)
		{
			if (list.contains(QVariant(1)) && list.contains(QVariant(2)) && list.contains(QVariant(3)) && list.contains(QVariant(4)))
			{
//				qDebug () << "    stimfiles OK";
				bval = true;
			}
			else
			{
				qDebug() << "    stimfiles ERR (all positions not found)";
				bval = false;
			}

		}
		else
		{
			qDebug() << "    found " << list.size() << " stimfiles records (expect 4) for stimulus_id " << stimulus_id;
			bval = false;
		}
	}
	else
	{
		qDebug() << "  found " << list.size() << " stimulus records...ERR";
		bval = false;
	}
	return bval;
}

bool validateExperiment(int experimentId)
{
	Habit::MainDao dao;
	bool bval = true;
	QString sExptName;
	QList<QVariant> list;

	dao.getExperimentName(experimentId, sExptName);
	qDebug() << "expt id " << experimentId << " name: " << sExptName;

	// check look_settings
	list = dao.getColumnValuesFromTable("look_settings", "id", "experiment_id", QVariant(experimentId));
	if (list.size() == 1)
		qDebug() << " look_settings...OK";
	else
	{
		qDebug() << " look_settings...ERR: found " << list.size() << " records (expect 1)";
		bval = false;
	}

	// check stimulus_display
	list = dao.getColumnValuesFromTable("stimulus_display", "id", "experiment_id", QVariant(experimentId));
	if (list.size() == 1)
		qDebug() << " stimulus_display...OK";
	else
	{
		qDebug() << " stimulus_display...ERR: found " << list.size() << " records (expect 1)";
		bval = false;
	}

	// check controlbar_options
	list = dao.getColumnValuesFromTable("controlbar_options", "id", "experiment_id", QVariant(experimentId));
	if (list.size() == 1)
		qDebug() << " controlbar_options...OK";
	else
	{
		qDebug() << " controlbar_options...ERR: found " << list.size() << " records (expect 1)";
		bval = false;
	}

	// check attention_setup
	list = dao.getColumnValuesFromTable("attention_setup", "stimulus_id", "experiment_id", QVariant(experimentId));
	if (list.size() == 1)
	{
		qDebug() << " attention_setup...OK";
		if (validateStimulus(list.at(0).toInt()))
			qDebug() << "  attention stimulus...OK";
		else
		{
			qDebug() << "  attention stimulus...ERR";
			bval = false;
		}
	}
	else
	{
		qDebug() << " attention_setup...ERR: found " << list.size() << " records (expect 1)";
		bval = false;
	}

	// check phase_settings
	list = dao.getColumnValuesFromTable("phase_settings", "id", "experiment_id", QVariant(experimentId));
	qDebug() << " phase_settings: found " << list.size() << " phases...";
	QListIterator<QVariant> itPhases(list);
	while (itPhases.hasNext())
	{
		QList<QVariant> listHab;
		QList<QVariant> listStim;
		QVariant phaseId = itPhases.next();

		// habituation_settings
		listHab = dao.getColumnValuesFromTable("habituation_settings", "id", "phase_id", QVariant(phaseId));
		if (listHab.size() == 1)
			qDebug() << "  habituation_settings...OK";
		else
		{
			qDebug() << "  habituation_settings...ERR: found " << listHab.size() << " records (expect 1): " << listHab;
			bval = false;
		}

		// stimulus
		listStim = dao.getColumnValuesFromTable("stimulus", "id", "phase_id", phaseId);
		qDebug() << "  phase id " << phaseId.toInt() << " (" << dao.getHPhaseName(phaseId.toInt()) << ") has " << listStim.size() << " stimuli";
		QListIterator<QVariant> itStimuli(listStim);
		while (itStimuli.hasNext())
		{
			bool bval;
			QVariant stimid = itStimuli.next();
//			qDebug() << "     check stimulus id " << stimid.toInt();
			if (validateStimulus(stimid.toInt()))
			{
				qDebug() << "   stimulus id " << stimid.toInt() << "...OK";
			}
			else
			{
				qDebug() << "   stimulus id " << stimid.toInt() << "...ERR";
				bval = false;
			}
		}
	}
	return bval;
}

void validateDB()
{
	Habit::MainDao dao;
	qDebug() << "Validating database...";

	try
	{
		QList<QVariant> listExperimentIDs = dao.getColumnValuesFromTable("experiments", "id");
		QListIterator<QVariant> itExptId(listExperimentIDs);
		while (itExptId.hasNext())
		{
			// validate this experiment
			validateExperiment(itExptId.next().toInt());
		}

	}
	catch (const Habit::HDBException& e)
	{
		qCritical() << e.what();
	}

}

int getDBVersion()
{
	int version=2000000;

	// First check if habit_version table exists.
	if (getDBTableExists("habit_version"))
	{

		QSqlQuery sqlv(QString("select max(version) from habit_version"));
		if (sqlv.next())
		{
			version = sqlv.value(0).toInt();
			qDebug() << "Got version from habit_version: " << version;
		}
	}
	else
	{
		// Test if the phase_settings and look_settings table exist
		if (getDBTableExists("phase_settings") && getDBTableExists("look_settings"))
		{
			version = 2000006;
			qDebug() << "Got version based on phase_settings/look_settings: " << version;
		}
		else
		{
			version = 2000001;
			qDebug() << "Got (primitive) version: " << version;
		}
	}
	return version;
}

bool getDBTableExists(QString name)
{
	bool result = false;
	QSqlQuery sql(QString("select count(*) from sqlite_master where name='%1'").arg(name));
	if (sql.next())
	{
		result = (1 == sql.value(0).toInt());
	}
	return result;
}

bool updateDBVersion(QSqlDatabase& db, const QFileInfo& fileinfo)
{
	bool result = true;
	const int iMinVersion										= 2000006;
	const int iAddColumnsToHabituationSettings					= 2000009;
	const int iAddInclusiveLookTimeColumnToLookSettings			= 2000010;
	const int iAddStimulusDisplayColumns						= 2000011;
	const int iAddExperimentHiddenColumn						= 2000012;
	const int iAddOrderTable									= 2000013;
	const int iChangeStimEnabled								= 2000014;
	const int iChangeStimEnabledForgotAttentionGetter			= 2000015;
	const int iBigStimulusTableChange							= 2000016;
	const int iBigStimulusTableChangeAG							= 2000017;
	const int iRepeatTrialOnMaxLookAway							= 2000018;
	const int iAddColumnToControlBarOptions						= 2000019;
	const int iAddFixedISIAttentionGetter						= 2000020;
	const int iPhaseReOrg										= 2000021;
	const int iLatestVersion									= 2000021;
	int iVersion = getDBVersion();

	// Will any updates be required? If so, close db, make a copy, then reopen.
	if (iVersion < iMinVersion)
	{
		qCritical() << "Cannot update database with version < " << iMinVersion;
		result = false;
	}
	else if (iVersion < iLatestVersion)
	{
		qDebug() << "Updating database version...";
		db.close();

		QString copyname(QString("%1/saved-%2-%3").arg(fileinfo.dir().canonicalPath()).arg(QDateTime::currentDateTime().toString("yyyyMMddhhmm")).arg(fileinfo.fileName()));
		if (!QFile::copy(fileinfo.canonicalFilePath(), copyname))
		{
			qCritical() << "Cannot make copy of database in " << copyname;
			result = false;
		}
		else
		{
			qDebug() << "Made backup copy of current database in " << copyname;

			// reopen db
			if (db.open())
			{
				if (iVersion < iAddColumnsToHabituationSettings)
				{
					// create habit_version table
					// we must alter table habituation_settings
					// insert record into habit_version
					db.transaction();
					QSqlQuery q0("CREATE TABLE \"habit_version\"(id INTEGER NOT NULL DEFAULT \"-1\"  PRIMARY KEY AUTOINCREMENT, version INTEGER)");
					QSqlQuery q1("alter table habituation_settings add column exclude_basis_window INTEGER DEFAULT 0");
					QSqlQuery q2("alter table habituation_settings add column require_min_basis_value INTEGER DEFAULT 0");
					QSqlQuery q3("alter table habituation_settings add column min_basis_value INTEGER DEFAULT 0");
					QSqlQuery q4("insert into habit_version (version) values(?)");
					q4.bindValue(0, iAddColumnsToHabituationSettings);
					q4.exec();
					if (!q0.lastError().isValid() && !q1.lastError().isValid() && !q2.lastError().isValid() && !q3.lastError().isValid() && !q4.lastError().isValid())
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iAddColumnsToHabituationSettings;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iAddColumnsToHabituationSettings;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q1.lastQuery() << " : " << q1.lastError();
						qDebug() << q2.lastQuery() << " : " << q2.lastError();
						qDebug() << q3.lastQuery() << " : " << q3.lastError();
						qDebug() << q4.lastQuery() << " : " << q4.lastError();
						result = false;
						db.rollback();
					}
				}
				if (iVersion < iAddInclusiveLookTimeColumnToLookSettings)
				{
					// Add column inclusive_look to habituation_settings
					db.transaction();
					QSqlQuery q0("alter table look_settings add column inclusive_look_time INTEGER DEFAULT 1");
					QSqlQuery q1("insert into habit_version (version) values(?)");
					q1.bindValue(0, iAddInclusiveLookTimeColumnToLookSettings);
					q1.exec();
					if (!q0.lastError().isValid() && !q1.lastError().isValid())
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iAddInclusiveLookTimeColumnToLookSettings;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iAddInclusiveLookTimeColumnToLookSettings;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q1.lastQuery() << " : " << q1.lastError();
						result = false;
						db.rollback();
					}
				}
				if (iVersion < iAddStimulusDisplayColumns)
				{
					// Add column inclusive_look to habituation_settings
					db.transaction();
					QSqlQuery q0("alter table stimulus_display add column stimulus_layout INTEGER DEFAULT 0");
					QSqlQuery q1("alter table stimulus_display add column use_iss INTEGER DEFAULT 0");
					QSqlQuery q2("insert into habit_version (version) values(?)");
					q2.bindValue(0, iAddStimulusDisplayColumns);
					q2.exec();
					if (!q0.lastError().isValid() && !q1.lastError().isValid() && !q2.lastError().isValid())
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iAddStimulusDisplayColumns;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iAddStimulusDisplayColumns;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q1.lastQuery() << " : " << q1.lastError();
						qDebug() << q2.lastQuery() << " : " << q2.lastError();
						result = false;
						db.rollback();
					}
				}
				if (iVersion < iAddExperimentHiddenColumn)
				{
					// Add column inclusive_look to habituation_settings
					db.transaction();
					QSqlQuery q0("alter table experiments add column hidden INTEGER DEFAULT 0");
					QSqlQuery q2("insert into habit_version (version) values(?)");
					q2.bindValue(0, iAddExperimentHiddenColumn);
					q2.exec();
					if (!q0.lastError().isValid() && !q2.lastError().isValid())
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iAddExperimentHiddenColumn;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iAddExperimentHiddenColumn;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q2.lastQuery() << " : " << q2.lastError();
						result = false;
						db.rollback();
					}
				}
				if (iVersion < iAddOrderTable)
				{
					// create stimulus_order table
					db.transaction();
					QSqlQuery q0("CREATE TABLE \"stimulus_order\"(id INTEGER NOT NULL DEFAULT \"-1\"  PRIMARY KEY AUTOINCREMENT, experiment_id INTEGER, name VARCHAR, context INTEGER, orderlist VARCHAR)");
					QSqlQuery q2("insert into habit_version (version) values(?)");
					q2.bindValue(0, iAddOrderTable);
					q2.exec();
					if (!q0.lastError().isValid() && !q2.lastError().isValid())
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iAddOrderTable;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iAddOrderTable;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q2.lastQuery() << " : " << q2.lastError();
						result = false;
						db.rollback();
					}

				}
				if (iVersion < iChangeStimEnabled)
				{
					// Change column name in the three stimuli tables
					db.transaction();
					QSqlQuery q0("ALTER TABLE \"pretest_stimuli\" RENAME TO \"pstimtmp\"");
					QSqlQuery q1("ALTER TABLE \"habituation_stimuli\" RENAME TO \"hstimtmp\"");
					QSqlQuery q2("ALTER TABLE \"test_stimuli\" RENAME TO \"tstimtmp\"");
					QSqlQuery q3("CREATE TABLE \"pretest_stimuli\" (\"id\" INTEGER PRIMARY KEY  DEFAULT \"-1\" ,\"experiment_id\" INTEGER,\"name\" VARCHAR(50),\"is_left_background\" BOOL,\"left_filename\" TEXT,\"is_left_loop_playback\" BOOL,\"is_center_background\" BOOL,\"center_filename\" VARCHAR(50),\"is_center_loop_playback\" BOOL,\"is_right_background\" BOOL,\"right_filename\" VARCHAR(50),\"is_right_loop_playback\" BOOL,\"independent_sound_filename\" VARCHAR(50),\"is_independent_loop_playback\" BOOL,\"left_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"right_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"center_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"independent_audio_balance\" INTEGER NOT NULL  DEFAULT 100 )");
					QSqlQuery q4("CREATE TABLE \"habituation_stimuli\" (\"id\" INTEGER PRIMARY KEY  DEFAULT \"-1\" ,\"experiment_id\" INTEGER,\"name\" VARCHAR(50),\"is_left_background\" BOOL,\"left_filename\" TEXT,\"is_left_loop_playback\" BOOL,\"is_center_background\" BOOL,\"center_filename\" VARCHAR(50),\"is_center_loop_playback\" BOOL,\"is_right_background\" BOOL,\"right_filename\" VARCHAR(50),\"is_right_loop_playback\" BOOL,\"independent_sound_filename\" VARCHAR(50),\"is_independent_loop_playback\" BOOL,\"left_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"right_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"center_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"independent_audio_balance\" INTEGER NOT NULL  DEFAULT 100 )");
					QSqlQuery q5("CREATE TABLE \"test_stimuli\" (\"id\" INTEGER PRIMARY KEY  DEFAULT \"-1\" ,\"experiment_id\" INTEGER,\"name\" VARCHAR(50),\"is_left_background\" BOOL,\"left_filename\" TEXT,\"is_left_loop_playback\" BOOL,\"is_center_background\" BOOL,\"center_filename\" VARCHAR(50),\"is_center_loop_playback\" BOOL,\"is_right_background\" BOOL,\"right_filename\" VARCHAR(50),\"is_right_loop_playback\" BOOL,\"independent_sound_filename\" VARCHAR(50),\"is_independent_loop_playback\" BOOL,\"left_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"right_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"center_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"independent_audio_balance\" INTEGER NOT NULL  DEFAULT 100 )");
					QSqlQuery q6("INSERT INTO pretest_stimuli (id, experiment_id, name, is_left_background, left_filename, is_left_loop_playback, is_center_background, center_filename, is_center_loop_playback, is_right_background, right_filename, is_right_loop_playback, independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, independent_audio_balance)	SELECT id, experiment_id, name, \"false\", left_filename, is_left_loop_playback, \"false\", center_filename, is_center_loop_playback, \"false\", right_filename, is_right_loop_playback, independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, independent_audio_balance from pstimtmp");
					QSqlQuery q7("INSERT INTO habituation_stimuli (id, experiment_id, name, is_left_background, left_filename, is_left_loop_playback, is_center_background, center_filename, is_center_loop_playback, is_right_background, right_filename, is_right_loop_playback, independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, independent_audio_balance)	SELECT id, experiment_id, name, \"false\", left_filename, is_left_loop_playback, \"false\", center_filename, is_center_loop_playback, \"false\", right_filename, is_right_loop_playback, independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, independent_audio_balance from hstimtmp");
					QSqlQuery q8("INSERT INTO test_stimuli (id, experiment_id, name, is_left_background, left_filename, is_left_loop_playback, is_center_background, center_filename, is_center_loop_playback, is_right_background, right_filename, is_right_loop_playback, independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, independent_audio_balance)	SELECT id, experiment_id, name, \"false\", left_filename, is_left_loop_playback, \"false\", center_filename, is_center_loop_playback, \"false\", right_filename, is_right_loop_playback, independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, independent_audio_balance from tstimtmp");
					QSqlQuery q9("DROP TABLE pstimtmp");
					QSqlQuery q10("DROP TABLE hstimtmp");
					QSqlQuery q11("DROP TABLE tstimtmp");
					QSqlQuery q12("insert into habit_version (version) values(?)");
					q12.bindValue(0, iChangeStimEnabled);
					q12.exec();

					if (
							!q0.lastError().isValid() &&
							!q1.lastError().isValid() &&
							!q2.lastError().isValid() &&
							!q3.lastError().isValid() &&
							!q4.lastError().isValid() &&
							!q5.lastError().isValid() &&
							!q6.lastError().isValid() &&
							!q7.lastError().isValid() &&
							!q8.lastError().isValid() &&
							!q9.lastError().isValid() &&
							!q10.lastError().isValid() &&
							!q11.lastError().isValid() &&
							!q12.lastError().isValid()
						)
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iChangeStimEnabled;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iChangeStimEnabled;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q1.lastQuery() << " : " << q1.lastError();
						qDebug() << q2.lastQuery() << " : " << q2.lastError();
						qDebug() << q3.lastQuery() << " : " << q3.lastError();
						qDebug() << q4.lastQuery() << " : " << q4.lastError();
						qDebug() << q5.lastQuery() << " : " << q5.lastError();
						qDebug() << q6.lastQuery() << " : " << q6.lastError();
						qDebug() << q7.lastQuery() << " : " << q7.lastError();
						qDebug() << q8.lastQuery() << " : " << q8.lastError();
						qDebug() << q9.lastQuery() << " : " << q9.lastError();
						qDebug() << q10.lastQuery() << " : " << q10.lastError();
						qDebug() << q11.lastQuery() << " : " << q11.lastError();
						qDebug() << q12.lastQuery() << " : " << q12.lastError();
						result = false;
						db.rollback();
					}

				}

				if (iVersion < iChangeStimEnabledForgotAttentionGetter)
				{
					// Change column name in the attention_getting_stimulus table
					db.transaction();
					QSqlQuery q0("ALTER TABLE \"attention_getting_stimuli\" RENAME TO \"astimtmp\"");
					QSqlQuery q1("CREATE TABLE \"attention_getting_stimuli\" (\"id\" INTEGER PRIMARY KEY  DEFAULT \"-1\" ,\"attention_getter_id\" INTEGER,\"name\" VARCHAR(50),\"is_left_background\" BOOL,\"left_filename\" TEXT,\"is_left_loop_playback\" BOOL,\"is_center_background\" BOOL,\"center_filename\" VARCHAR(50),\"is_center_loop_playback\" BOOL,\"is_right_background\" BOOL,\"right_filename\" VARCHAR(50),\"is_right_loop_playback\" BOOL,\"independent_sound_filename\" VARCHAR(50),\"is_independent_loop_playback\" BOOL,\"left_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"right_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"center_audio_balance\" INTEGER NOT NULL  DEFAULT 100 ,\"independent_audio_balance\" INTEGER NOT NULL  DEFAULT 100 )");
					QSqlQuery q6("INSERT INTO attention_getting_stimuli (id, attention_getter_id, name, is_left_background, left_filename, is_left_loop_playback, is_center_background, center_filename, is_center_loop_playback, is_right_background, right_filename, is_right_loop_playback, independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, independent_audio_balance)	SELECT id, attention_getter_id, name, \"false\", left_filename, is_left_loop_playback, \"false\", center_filename, is_center_loop_playback, \"false\", right_filename, is_right_loop_playback, independent_sound_filename, is_independent_loop_playback, left_audio_balance, right_audio_balance, center_audio_balance, independent_audio_balance from astimtmp");
					QSqlQuery q9("DROP TABLE astimtmp");
					QSqlQuery q12("insert into habit_version (version) values(?)");
					q12.bindValue(0, iChangeStimEnabledForgotAttentionGetter);
					q12.exec();

					if (
							!q0.lastError().isValid() &&
							!q1.lastError().isValid() &&
							!q6.lastError().isValid() &&
							!q9.lastError().isValid() &&
							!q12.lastError().isValid()
						)
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iChangeStimEnabledForgotAttentionGetter;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iChangeStimEnabledForgotAttentionGetter;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q1.lastQuery() << " : " << q1.lastError();
						qDebug() << q6.lastQuery() << " : " << q6.lastError();
						qDebug() << q9.lastQuery() << " : " << q9.lastError();
						qDebug() << q12.lastQuery() << " : " << q12.lastError();
						result = false;
						db.rollback();
					}

				}

				if (iVersion < iBigStimulusTableChange)
				{
					// Create new stimulus tables and migrate from existing tables.
					db.transaction();
					QSqlQuery q0("CREATE TABLE \"stimulus\" (\"id\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE  DEFAULT -1, \"experiment_id\" INTEGER NOT NULL , \"name\" VARCHAR NOT NULL , \"context\" INTEGER NOT NULL )");
					QSqlQuery q1("CREATE  TABLE \"stimfiles\" (\"id\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , \"stimulus_id\" INTEGER NOT NULL , \"position\" INTEGER NOT NULL , \"filename\" VARCHAR, \"isLoop\" BOOL NOT NULL  DEFAULT 0, \"volume\" INTEGER NOT NULL  DEFAULT 0, \"isBackground\" BOOL NOT NULL  DEFAULT 0, \"isColor\" BOOL NOT NULL  DEFAULT 0, \"color\" VARCHAR)");

					/*
					 * Populate the tables...
					 */

					if (
							!q0.lastError().isValid() &&
							!q1.lastError().isValid()
						)
					{
						QSqlQuery q2("SELECT * FROM \"pretest_stimuli\"");
						QSqlQuery q3("SELECT * FROM \"habituation_stimuli\"");
						QSqlQuery q4("SELECT * FROM \"test_stimuli\"");
						bool bOK = false;

						bOK = 	populateStimulusTable(q2, HStimContext::PreTestPhase) &&
								populateStimulusTable(q3, HStimContext::HabituationPhase) &&
								populateStimulusTable(q4, HStimContext::TestPhase);

						if (!bOK)
						{
							qCritical() << "Error in updateDBVersion at version " << iBigStimulusTableChange;
							result = false;
							db.rollback();
						}
						else
						{
							QSqlQuery qversion("insert into habit_version (version) values(?)");
							qversion.bindValue(0, iBigStimulusTableChange);
							qversion.exec();
							if (!qversion.lastError().isValid())
							{
								result = true;
								db.commit();
								qDebug() << "Database updated to version " << iBigStimulusTableChange;
							}
							else
							{
								qCritical() << "Error in updateDBVersion at version " << iBigStimulusTableChange;
								qDebug() << qversion.lastQuery() << " : " << qversion.lastError();
								result = false;
								db.rollback();
							}
						}

					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iBigStimulusTableChange;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q1.lastQuery() << " : " << q1.lastError();
						result = false;
						db.rollback();
					}
				}


				if (result && iVersion < iBigStimulusTableChangeAG)
				{
					// Move attention getting stimuli from table attention_getting_stimuli
					// to stimulus/stimfiles. The attention getting stim will be tied to the
					// experiment by the experiment ID and the context. The old 'id' from
					// the attention_setup table was used in the attention_getting_stimuli table
					// to point back to the stim. That was redundant, because each experiment
					// has just one attention getting record (by default), and that link is
					// established by the experiment id.

					db.transaction();

					/*
					 * Each record in attention_getting_stimuli must be moved to stimuli/stimfiles.
					 */

					QSqlQuery q2("SELECT * from attention_getting_stimuli a, attention_setup e where a.attention_getter_id = e.id ");
					bool bOK = true;

					while (bOK && q2.next())
					{
						int stimulus_id = -1;	// not really used
						bOK = populateStimulusTableSingle(q2, HStimContext::AttentionGetter, stimulus_id);
					}

					if (!bOK)
					{
						qCritical() << "Error in updateDBVersion at version " << iBigStimulusTableChangeAG;
						result = false;
						db.rollback();
					}
					else
					{
						QSqlQuery qversion("insert into habit_version (version) values(?)");
						qversion.bindValue(0, iBigStimulusTableChangeAG);
						qversion.exec();
						if (!qversion.lastError().isValid())
						{
							result = true;
							db.commit();
							qDebug() << "Database updated to version " << iBigStimulusTableChangeAG;
						}
						else
						{
							qCritical() << "Error in updateDBVersion at version " << iBigStimulusTableChangeAG;
							qDebug() << qversion.lastQuery() << " : " << qversion.lastError();
							result = false;
							db.rollback();
						}
					}
				}


				if (result && iVersion < iRepeatTrialOnMaxLookAway)
				{
					// Add column repeat_on_max_lookaway to phase_settings
					db.transaction();
					QSqlQuery q0("alter table phase_settings add column repeat_trial_on_max_lookaway INTEGER DEFAULT 0");
					QSqlQuery q2("insert into habit_version (version) values(?)");
					q2.bindValue(0, iRepeatTrialOnMaxLookAway);
					q2.exec();
					if (!q0.lastError().isValid() && !q2.lastError().isValid())
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iRepeatTrialOnMaxLookAway;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iRepeatTrialOnMaxLookAway;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q2.lastQuery() << " : " << q2.lastError();
						result = false;
						db.rollback();
					}
				}

				if (result && iVersion < iAddColumnToControlBarOptions)
				{
					// Add column repeat_on_max_lookaway to phase_settings
					db.transaction();
					QSqlQuery q0("alter table controlbar_options add column display_looking_direction INTEGER DEFAULT 0");
					QSqlQuery q2("insert into habit_version (version) values(?)");
					q2.bindValue(0, iAddColumnToControlBarOptions);
					q2.exec();
					if (!q0.lastError().isValid() && !q2.lastError().isValid())
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iAddColumnToControlBarOptions;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iAddColumnToControlBarOptions;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q2.lastQuery() << " : " << q2.lastError();
						result = false;
						db.rollback();
					}
				}

				if (result && iVersion < iAddFixedISIAttentionGetter)
				{
					// add columns use_fixed_isi and isi_ms to attention_setup
					db.transaction();
					QSqlQuery q0("alter table attention_setup add column use_fixed_isi BOOL DEFAULT false");
					QSqlQuery q1("alter table attention_setup add column isi_ms INTEGER DEFAULT 0");
					QSqlQuery q2("insert into habit_version (version) values(?)");
					q2.bindValue(0, iAddFixedISIAttentionGetter);
					q2.exec();
					if (!q0.lastError().isValid() && !q1.lastError().isValid() && !q2.lastError().isValid())
					{
						result = true;
						db.commit();
						qDebug() << "Database updated to version " << iAddFixedISIAttentionGetter;
					}
					else
					{
						qCritical() << "Error in updateDBVersion at version " << iAddFixedISIAttentionGetter;
						qDebug() << q0.lastQuery() << " : " << q0.lastError();
						qDebug() << q1.lastQuery() << " : " << q1.lastError();
						qDebug() << q2.lastQuery() << " : " << q2.lastError();
						result = false;
						db.rollback();
					}
				}



				if (result && iVersion < iPhaseReOrg)
				{
					QSqlQuery qq;
					QStringList queries;

					// phase_settings
					QString q0("alter table phase_settings add column name VARCHAR NOT NULL DEFAULT unnamed");
					QString q1("alter table phase_settings add column seqno INTEGER NOT NULL DEFAULT -1");
					QString q2("update phase_settings set name=\"PreTest\" where phase_type=1");
					QString q3("update phase_settings set name=\"Habituation\" where phase_type=2");
					QString q4("update phase_settings set name=\"Test\" where phase_type=3");
					QString q5("update phase_settings set seqno=phase_type");

					// stimulus
					QString q6("alter table stimulus add column phase_id INTEGER NOT NULL DEFAULT -1");
					QString q7("UPDATE stimulus SET phase_id = "
								"(SELECT phase_settings.id from phase_settings "
									"WHERE stimulus.experiment_id = phase_settings.experiment_id AND stimulus.context = phase_settings.phase_type) "
								" WHERE EXISTS (SELECT * from phase_settings "
												"WHERE stimulus.experiment_id = phase_settings.experiment_id AND stimulus.context = phase_settings.phase_type)");

					// stimulus_order
					QString q8("alter table stimulus_order add column phase_id INTEGER NOT NULL DEFAULT -1");
					QString q9("UPDATE stimulus_order SET phase_id = "
								"(SELECT phase_settings.id from phase_settings "
									"WHERE stimulus_order.experiment_id = phase_settings.experiment_id AND stimulus_order.context = phase_settings.phase_type) "
								" WHERE EXISTS (SELECT * FROM phase_settings WHERE stimulus_order.experiment_id = phase_settings.experiment_id AND stimulus_order.context = phase_settings.phase_type)");

					// habituation_settings
					QString q10("alter table habituation_settings add column phase_id INTEGER NOT NULL DEFAULT -1");
					QString q11("UPDATE habituation_settings SET phase_id = (SELECT phase_settings.id from phase_settings WHERE habituation_settings.experiment_id = phase_settings.experiment_id and phase_settings.phase_type=2)");
					//					QString q11("UPDATE habituation_settings SET phase_id = (SELECT phase_settings.id from phase_settings WHERE habituation_settings.experiment_id = phase_settings.experiment_id) "
					//									" WHERE EXISTS (SELECT * from phase_settings WHERE habituation_settings.experiment_id = phase_settings.experiment_id AND phase_settings.phase_type = 2)");
					QString q12("INSERT into habituation_settings (experiment_id, habituation_type, phase_id) SELECT experiment_id,0,id from phase_settings where phase_type=1 or phase_type=3");

					// habituation_settings again
					QString q13("ALTER TABLE habituation_settings ADD COLUMN ntrials INTEGER NOT NULL DEFAULT 0");
					QString q14("UPDATE habituation_settings SET ntrials=(SELECT phase_settings.ntrials from phase_settings WHERE habituation_settings.phase_id = phase_settings.id)");

					// attention_setup
					QString q15("ALTER TABLE main.attention_setup ADD COLUMN stimulus_id INTEGER NOT NULL DEFAULT -1");
					QString q16("UPDATE attention_setup SET stimulus_id = "
								"(SELECT stimulus.id from stimulus WHERE stimulus.experiment_id = attention_setup.experiment_id AND stimulus.context = 4) "
								"WHERE EXISTS ( SELECT * FROM stimulus WHERE stimulus.experiment_id = attention_setup.experiment_id	AND stimulus.context = 4)");

					queries << q0 << q1 << q2 << q3 << q4 << q5 << q6 << q7 << q8 << q9 << q10 << q11 << q12 << q13 << q14 << q15 << q16;


					db.transaction();
					for (int i=0; i<queries.size(); i++)
					{
						result = qq.exec(queries.at(i));
						if (!result)
						{
							qCritical() << "Error in updateDBVersion at version " << iPhaseReOrg;
							qDebug() << qq.lastQuery() << " : " << qq.lastError();
							break;
						}
					}

					if (result)
					{
						QSqlQuery qv("insert into habit_version (version) values(?)");
						qv.bindValue(0, iPhaseReOrg);
						result = qv.exec();
						if (!result)
						{
							qCritical() << "Error in updateDBVersion at version " << iPhaseReOrg;
							qDebug() << qv.lastQuery() << " : " << qv.lastError();
						}
					}

					if (result)
					{
						qDebug() << "Database updated to version " << iPhaseReOrg;
						db.commit();
					}
					else
					{
						db.rollback();
					}
				}




				// If failed, revert to to original database
				if (!result)
				{
					closeDB();

					// rename the current database - it has a partial update only!
					QString partialname(QString("%1/error-%2-%3").arg(fileinfo.dir().canonicalPath()).arg(QDateTime::currentDateTime().toString("yyyyMMddhhmm")).arg(fileinfo.fileName()));
					if (!QFile::rename(fileinfo.canonicalFilePath(), partialname))
					{
						qCritical() << "Cannot rename existing habit.sqlite. This database may be partially updated and may not work. Must revert to database " << copyname << " manually.";
					}
					else
					{
						if (!QFile::copy(copyname, fileinfo.canonicalFilePath()))
						{
							qCritical() << "Cannot revert to to original database in " << copyname;
						}
						openDB(fileinfo.absoluteDir());
					}
				}
			}
			else
			{
				qCritical() << "Cannot re-open database " << db.databaseName();
			}
		}
	}
	else
	{
		result = true;
		qDebug() << "Database is at the latest version.";
	}

	return result;
}









bool populateStimulusTable(QSqlQuery& q, const HStimContext& context)
{
	bool result = true;
	int stimulus_id = -1;
	while (result && q.next())
	{
		result = populateStimulusTableSingle(q, context, stimulus_id);
	}
	return result;
}


// Populate a record in the 'stimulus' table using the results in the query given.
// I assume the query comes from one of the old stimuli tables (select * from one of
// pretest_stimuli, habituation_stimuli, test_stimuli, attention_getting_stimuli).

bool populateStimulusTableSingle(QSqlQuery& q, const HStimContext& context, int& stimulus_id)
{
	bool result = false;
	QSqlQuery qStimulus("insert into stimulus (experiment_id, name, context) values (?, ?, ?)");
	qStimulus.addBindValue(q.value(q.record().indexOf("experiment_id")).toInt());
	qStimulus.addBindValue(q.value(q.record().indexOf("name")).toString());
	qStimulus.addBindValue(QVariant(context.number()));
	qDebug() << "populateStimulusTableSingle: stim name " << q.value(q.record().indexOf("name")).toString();
	result = qStimulus.exec();
	if (result)
	{
		stimulus_id = q.lastInsertId().toInt();
		if (result)
		{
			QSqlQuery qLeft("insert into stimfiles (stimulus_id, position, filename, isLoop, volume, isBackground, isColor, color) values (?, ?, ?, ?, ?, ?, ?, ?)");
			qLeft.addBindValue(QVariant(stimulus_id));
			qLeft.addBindValue(QVariant(HPlayerPositionType::Left.number()));
			qLeft.addBindValue(q.value(q.record().indexOf("left_filename")).toString());
			qLeft.addBindValue(q.value(q.record().indexOf("is_left_loop_playback")).toBool());
			qLeft.addBindValue(q.value(q.record().indexOf("left_audio_balance")).toInt());
			qLeft.addBindValue(QVariant(false));
			qLeft.addBindValue(QVariant(false));
			qLeft.addBindValue(QVariant(QString("#000000")));
			result = qLeft.exec();
			//qDebug() << "Left " << qLeft.lastQuery() << " : " << qLeft.lastError();
			if (!result)
			{
				qCritical() << "Error in populateStimulusTableSingle";
				qDebug() << qLeft.lastQuery() << " : " << qLeft.lastError();
			}
		}

		if (result)
		{
			QSqlQuery qCenter("insert into stimfiles (stimulus_id, position, filename, isLoop, volume, isBackground, isColor) values (?, ?, ?, ?, ?, ?, ?)");
			qCenter.addBindValue(QVariant(stimulus_id));
			qCenter.addBindValue(QVariant(HPlayerPositionType::Center.number()));
			qCenter.addBindValue(q.value(q.record().indexOf("center_filename")).toString());
			qCenter.addBindValue(q.value(q.record().indexOf("is_center_loop_playback")).toBool());
			qCenter.addBindValue(q.value(q.record().indexOf("center_audio_balance")).toInt());
			qCenter.addBindValue(QVariant(false));
			qCenter.addBindValue(QVariant(false));
			result = qCenter.exec();
			//qDebug() << "Center " << qCenter.lastQuery() << " : " << qCenter.lastError();
			if (!result)
			{
				qCritical() << "Error in populateStimulusTableSingle";
				qDebug() << qCenter.lastQuery() << " : " << qCenter.lastError();
			}
		}

		if (result)
		{
			QSqlQuery qRight("insert into stimfiles (stimulus_id, position, filename, isLoop, volume, isBackground, isColor) values (?, ?, ?, ?, ?, ?, ?)");
			qRight.addBindValue(QVariant(stimulus_id));
			qRight.addBindValue(QVariant(HPlayerPositionType::Right.number()));
			qRight.addBindValue(q.value(q.record().indexOf("right_filename")).toString());
			qRight.addBindValue(q.value(q.record().indexOf("is_right_loop_playback")).toBool());
			qRight.addBindValue(q.value(q.record().indexOf("right_audio_balance")).toInt());
			qRight.addBindValue(QVariant(false));
			qRight.addBindValue(QVariant(false));
			result = qRight.exec();
			//qDebug() << "Right " << qRight.lastQuery() << " : " << qRight.lastError();
			if (!result)
			{
				qCritical() << "Error in populateStimulusTableSingle";
				qDebug() << qRight.lastQuery() << " : " << qRight.lastError();
			}
		}

		if (result)
		{
			QSqlQuery qSound("insert into stimfiles (stimulus_id, position, filename, isLoop, volume, isBackground, isColor) values (?, ?, ?, ?, ?, ?, ?)");
			qSound.addBindValue(QVariant(stimulus_id));
			qSound.addBindValue(QVariant(HPlayerPositionType::Sound.number()));
			qSound.addBindValue(q.value(q.record().indexOf("independent_sound_filename")).toString());
			qSound.addBindValue(q.value(q.record().indexOf("is_independent_loop_playback")).toBool());
			qSound.addBindValue(q.value(q.record().indexOf("independent_audio_balance")).toInt());
			qSound.addBindValue(QVariant(false));
			qSound.addBindValue(QVariant(false));
			result = qSound.exec();
			//qDebug() << "Sound " << qSound.lastQuery() << " : " << qSound.lastError();
			if (!result)
			{
				qCritical() << "Error in populateStimulusTableSingle";
				qDebug() << qSound.lastQuery() << " : " << qSound.lastError();
			}
		}
	}
	return result;
}
