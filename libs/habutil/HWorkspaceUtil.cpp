/*
 * HWorkspaceUtil.cpp
 *
 *  Created on: May 30, 2014
 *      Author: Oakeslab
 */


#include "HWorkspaceUtil.h"
#include "HDBUtil.h"
#include <QDesktopServices>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QtDebug>
#include <QMessageBox>

// A dir is a valid workspace if all of the following are true
// 1. dir exists
// 2. dir contains a file called "habit.sqlite"
// 3. dir contains a folder called "log"
// 4. dir contains a folder called "results"
// 5. dir contains a folder called "stim"

bool habutilIsValidWorkspace(QDir dir)
{
	return (dir.exists() && dir.exists("habit.sqlite") && dir.exists("results") && dir.exists("log") && dir.exists("stim"));
}


// Create a new workspace in the dir 'dir'.

bool habutilCreateWorkspace(QDir& dir)
{
	// dir must exist!
	if (!dir.exists())
	{
		// Create dir
		if (!dir.mkpath(dir.absolutePath()))
		{
			qCritical() << "Cannot create directory " << dir.absolutePath();
			QMessageBox msg;
			msg.setText("Cannot create workspace directry.");
			msg.exec();
			return false;
		}
		else
		{
			qDebug() << "Created workspace directory: " << dir.absolutePath();
		}
	}

	// see if dir already contains db
	if (!dir.exists("habit.sqlite"))
	{
		// copy database from resources
		if (!QFile::copy(":/resources/habit.sqlite", dir.absoluteFilePath("habit.sqlite")))
		{
			qCritical() << "Cannot create database file " << dir.absoluteFilePath("habit.sqlite");
			return false;
		}
		else
		{
			QFile newfile(dir.absoluteFilePath("habit.sqlite"));
			newfile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::WriteGroup);
			qDebug() << "Created database " << newfile.fileName();
		}
	}

	// See if logs dir exists
	if (!dir.exists("log"))
	{
		if (!dir.mkdir("log"))
		{
			qCritical() << "Cannot create log directory " << dir.absoluteFilePath("log");
			return false;
		}
		else
		{
			qDebug() << "Created log directory " << dir.absoluteFilePath("log");
		}
	}

	// See if results dir exists
	if (!dir.exists("results"))
	{
		if (!dir.mkdir("results"))
		{
			qCritical() << "Cannot create results directory " << dir.absoluteFilePath("results");
			return false;
		}
		else
		{
			qDebug() << "Created results directory " << dir.absoluteFilePath("results");
		}
	}

	// See if stim dir exists
	if (!dir.exists("stim"))
	{
		if (!dir.mkdir("stim"))
		{
			qCritical() << "Cannot create stim directory " << dir.absoluteFilePath("stim");
			return false;
		}
		else
		{
			qDebug() << "Created stim directory " << dir.absoluteFilePath("stim");
		}
	}

	return true;
}


// Called on startup.
// 1. Check settings for saved workspace "workspace". If the setting doesn't exist, create default workspace
//    in documents folder and add setting.
// 2. Open database

bool habutilInitWorkspace()
{
	bool b = false;
	QSettings settings;
	QDir workspaceDir;
	b = habutilGetWorkspaceDir(workspaceDir);
	qDebug() << "Habit is opening workspace directory \"" << workspaceDir.absolutePath() << "\"";
	if (!b)
		b = habutilCreateWorkspace(workspaceDir);

	// if all is well, open the database
	if (b)
	{
		b = openDB(workspaceDir);
	}

	return b;
}


// Prompt user to select a workspace folder.
// A new workspace is created in that folder if needed, and settings are updated.
// The database is not opened, call habutilInitWorkspace() to do that.

bool habutilSelectWorkspace()
{
	bool b = false;
	QDir dir;
	QString selectedDir;

	habutilGetWorkspaceDir(dir);

	selectedDir = QFileDialog::getExistingDirectory(0, "Select Habit Workspace Folder", dir.absolutePath());
	if (selectedDir.isEmpty() || selectedDir.isNull())
		return false;
	else
	{
		habutilSetWorkspace(selectedDir);
	}

	dir.setPath(selectedDir);
	b = habutilCreateWorkspace(dir);
	return b;
}


// get workspace dir.
// If settings "workspace" exists, use it
// If that setting is not set, use default location (and create setting).
// Returns true if the folder is a valid workspace, false if not.

bool habutilGetWorkspaceDir(QDir& dir)
{
	QSettings settings;
	if (!settings.contains("workspace"))
	{
		QString defaultDir = QString("%1/habit").arg(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation));
		dir.setPath(defaultDir);
		settings.setValue("workspace", dir.absolutePath());
	}
	else
	{
		dir.setPath(settings.value("workspace").toString());
	}

	return habutilIsValidWorkspace(dir);
}

// get results dir. Assumes that the directory exists. As long as this is called on an open workspace that is a safe assumption.

QDir habutilGetResultsDir()
{
	QDir dir;
	habutilGetWorkspaceDir(dir);
	dir.setPath(dir.filePath("results"));
	return dir;
}

QDir habutilGetLogDir()
{
	QDir dir;
	habutilGetWorkspaceDir(dir);
	dir.setPath(dir.filePath("log"));
	return dir;
}

QDir habutilGetStimDir()
{
	QDir dir;
	habutilGetWorkspaceDir(dir);
	dir.setPath(dir.filePath("stim"));
	return dir;
}

// Testing utility. Call to delete the "workspace" setting from settings.

void habutilClearWorkspace()
{
	QSettings settings;
	settings.remove("workspace");
	return;
}

// Set workspace to supplied directory. No testing made to determine if its a directory.
void habutilSetWorkspace(const QString& d)
{
	QSettings settings;
	settings.setValue("workspace", d);
	return;
}
