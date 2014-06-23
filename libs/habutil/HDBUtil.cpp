/*
 * HDBUtil.cpp
 *
 *  Created on: Aug 8, 2013
 *      Author: Oakeslab
 */


#include "HDButil.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QtDebug>
#include <QDateTime>

#if 0

This function superceded by functions in HWorkspaceUtil

// select existing db(true) or create new one (false)
bool selectDB(bool bExisting)
{
	bool bval = false;
	QSettings settings;
	QString dir, filename;
	QString default_filename("habit.db3");
	QString selectedFileName;
	if (settings.contains("database/dir") && settings.contains("database/filename"))
	{
		dir = settings.value("database/dir").toString();
		filename = settings.value("database/filename").toString();
	}
	else
	{
		// Settings do not have current database filename and dir.
		// Check default location for database file.
		dir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
		filename = default_filename;
	}
	QFileInfo initial(QDir(dir), filename);
	QString fullname = initial.absoluteFilePath();

	if (bExisting)
		selectedFileName = QFileDialog::getOpenFileName(NULL, "Select Habit database", fullname, "Habit Database Files (*.db3)");
	else
		selectedFileName = QFileDialog::getSaveFileName(NULL, "New Habit database filename", fullname, "Habit Database Files (*.db3)");

	// getOpenFileName() returns a null string if user cancelled.
	// If they didn't cancel, then check if the file exists. If it doesn't exist, ask the user
	// if they'd like to create a new file.

	if (!selectedFileName.isNull())
	{
		QFileInfo selected(selectedFileName);
		if (selected.exists())
		{
			bval = true;
			settings.setValue("database/dir", selected.dir().canonicalPath());
			settings.setValue("database/filename", selected.fileName());
		}
		else
		{
			QMessageBox msgBox;
			msgBox.setText(QString("The database file \"%1\" does not exist. Would you like to create this file?").arg(selectedFileName));
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			int iret = msgBox.exec();
			if (iret == QMessageBox::Yes)
			{
				QFile::copy(":/resources/habit.db3", selectedFileName);
				// set perms
				QFile newfile(selectedFileName);
				newfile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::WriteGroup);
				bval = true;
				settings.setValue("database/dir", selected.dir().canonicalPath());
				settings.setValue("database/filename", selected.fileName());
			}
		}
	}

	return bval;
}

#endif


// Close database on default connection.

void closeDB()
{
	// See if there is a database connection currently open....
//	QSqlDatabase db = QSqlDatabase::database();
//	if (db.isOpen())
//	{
//		db.close();
//	}
	if (QSqlDatabase::database().isOpen())
	{
		QSqlDatabase::database().close();
		QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
	}
}

// Open the database file "habit.sqlite" in the given directory.

bool openDB(QDir& dir)
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
		}
		else
		{
			qDebug() << "Database file open failed.";
		}
	}
	return bval;
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
	const int iMinVersion                                      = 2000006;
	const int iAddColumnsToHabituationSettings                 = 2000009;
	const int iAddInclusiveLookTimeColumnToLookSettings        = 2000010;
	const int iLatestVersion                                   = 2000010;
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
