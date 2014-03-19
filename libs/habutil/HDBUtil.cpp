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
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QtDebug>
#include <QDateTime>


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

	qDebug() << "Selected db " << selectedFileName;

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

void closeDB()
{
	// See if there is a database connection currently open....
	QSqlDatabase db = QSqlDatabase::database();
	if (db.isOpen())
	{
		db.close();
	}
}

bool openDB()
{
	bool bval = false;
	QSettings settings;
	QString dir, filename;
	QString default_dir;
	QString default_filename("habit.db3");
	QFileInfo fileinfo;


	// close database if one is open
	closeDB();

	// First, determine a dir and filename to check.
	// This will be taken from settings file (if found),
	// or else we assign the default values.
	//
	// Next, check if that directory/file exists.
	// If it exists then we'll open that file.
	//

	if (settings.contains("database/dir") && settings.contains("database/filename"))
	{
		dir = settings.value("database/dir").toString();
		filename = settings.value("database/filename").toString();
		fileinfo.setFile(QDir(dir), filename);
	}
	else
	{
		// Settings do not have current database filename and dir.
		// Check default location for database file.
		default_dir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
		fileinfo.setFile(QDir(default_dir), default_filename);
	}

	// Does file exist and can we write to it? If not, then get filename
	qDebug() << "Opening database file: " << fileinfo.absoluteFilePath();
	if (fileinfo.exists())
	{
		if (!fileinfo.isWritable())
		{
			QMessageBox msgBox;
			msgBox.setText("The database file %1 exists, but you do not have write permission. Please fix permissions or select another file.");
		}
		else
		{
			QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
			db.setDatabaseName(fileinfo.absoluteFilePath());
			qDebug() << "Opening database: " << fileinfo.absoluteFilePath();
			if(db.open())
			{
				bval = true;
				qDebug() << "Database opened.";

				// Now update database if necessary. Do NOT change the database name!
				if (updateDBVersion(db, fileinfo))
				{
					// Update settings to save database name and location for next time.
					settings.setValue("database/dir", fileinfo.path());
					settings.setValue("database/filename", fileinfo.fileName());
				}
				else
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
	const int iMinVersion = 2000006;
	const int iLatestVersion = 2000009;
	const int iAddColumnsToHabituationSettings = 2000009;
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
