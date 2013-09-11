/*
 * HDBUtil.cpp
 *
 *  Created on: Aug 8, 2013
 *      Author: Oakeslab
 */


#include "HDButil.h"
#include <QtSql/QSqlDatabase>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QtDebug>


// select existing db(true) or create new one (false)
bool selectDB()
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
	selectedFileName = QFileDialog::getSaveFileName(NULL, "Select Habit database", fullname, "Habit Database Files (*.db3)");
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
				settings.setValue("database/dir", fileinfo.path());
				settings.setValue("database/filename", fileinfo.fileName());
			}
			else
			{
				qDebug() << "Database file open failed.";
			}
		}
	}
	return bval;
}


