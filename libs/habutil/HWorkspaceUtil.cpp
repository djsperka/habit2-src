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
#include <QRegExp>

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

			// move sample stim files into stim folder
			QDir resourceStimDir(":/resources/stim");
			QDir stimDir(dir);
			stimDir.cd(QString("stim"));

			QFileInfoList list = resourceStimDir.entryInfoList();
			for (int i = 0; i < list.size(); ++i)
			{
				QFileInfo fileInfo = list.at(i);
				if (!QFile::copy(fileInfo.absoluteFilePath(), stimDir.absoluteFilePath(fileInfo.fileName())))
				{
					qCritical() << "Cannot copy image file " << fileInfo.absoluteFilePath();
					return false;
				}
				else
				{
					qDebug() << "Copied " << fileInfo.absoluteFilePath() << " to " << stimDir.absoluteFilePath(fileInfo.fileName());
				}
			}

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

	//qDebug() << "getWorkspaceDir: " << dir << ":" << habutilIsValidWorkspace(dir);

	return habutilIsValidWorkspace(dir);
}


QString habutilGetWorkspaceDir()
{
	QDir dir;
	habutilGetWorkspaceDir(dir);
	return dir.absolutePath();
}


// get results dir. Assumes that the directory exists. As long as this is called on an open workspace that is a safe assumption.

QDir habutilGetResultsDir(const QString expt)
{
	QDir dir;
	habutilGetWorkspaceDir(dir);
	dir.setPath(dir.filePath("results"));
	if (!expt.isEmpty())
	{
		if (!dir.exists(expt))
		{
			if (!dir.mkdir(expt))
			{
				qCritical() << "Cannot create results directory for experiment \"" << expt << "\". Results will be put in \"" << dir.absolutePath() << "\"";
			}
			else
			{
				qDebug() << "Created results directory for experiment \"" << expt << "\"";
				dir.setPath(dir.filePath(expt));
			}
		}
		else
		{
			dir.setPath(dir.filePath(expt));
		}
	}
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



bool habutilGetStimulusRootDir(QDir& stimRootDir)
{
	QSettings settings;

	if (!habutilGetUseDefaultStimRoot())
	{
		stimRootDir = habutilGetStimDir();
		if (!settings.contains("stimroot"))
		{
			settings.setValue("stimroot", stimRootDir.absolutePath());
		}
		else
		{
			stimRootDir.setPath(settings.value("stimroot").toString());
		}
	}
	else
	{
		stimRootDir = habutilGetStimDir();
	}
	return stimRootDir.exists();
}


QString habutilGetStimulusRootDir()
{
	QDir dir;
	habutilGetStimulusRootDir(dir);
	return dir.absolutePath();
}

// Prompt user to select a stimulus root folder.
bool habutilSelectStimulusRootDir()
{
	bool b = false;
	QDir dir;
	QString selectedDir;

	habutilGetStimulusRootDir(dir);

	selectedDir = QFileDialog::getExistingDirectory(0, "Select Habit Stimulus Root Folder", dir.absolutePath());
	if (selectedDir.isEmpty() || selectedDir.isNull())
	{
		b = false;
	}
	else
	{
		habutilSetStimulusRootDir(selectedDir);
		b = true;
	}
	return b;
}

bool habutilGetUseDefaultStimRoot()
{
	QSettings settings;
	if (!settings.contains("stimroot_use_default"))
	{
		settings.setValue("stimroot_use_default", false);
	}
	return settings.value("stimroot_use_default").toBool();
}

void habutilSetUseDefaultStimRoot(bool b)
{
	QSettings settings;
	settings.setValue("stimroot_use_default", b);
	return;
}



// Testing utility. Call to delete the "stimroot" setting from settings.
void habutilClearStimulusRootDir()
{
	QSettings settings;
	settings.remove("stimroot");
	return;
}


// Set stimroot to given dir
void habutilSetStimulusRootDir(const QString& d)
{
	QSettings settings;
	settings.setValue("stimroot", QDir(d).absolutePath());
	return;
}


// get last dir that a stim was selected from
bool habutilGetLastDir(QDir& dir)
{
	QSettings settings;
	dir = QDir::root();
	if (!settings.contains("lastdir"))
	{
		settings.setValue("lastdir", dir.absolutePath());
	}
	else
	{
		dir.setPath(settings.value("lastdir").toString());
	}

	return dir.exists();
}

// set last dir that a stim was selected from
void habutilSetLastDir(const QString& d)
{
	QSettings settings;
	settings.setValue("lastdir", QDir(d).absolutePath());
	return;
}


// Get the local machine's monitor ID for given position.
int habutilGetMonitorID(const HPlayerPositionType& type)
{
	int num = -1;	// default is NO MONITOR
	QSettings settings;
	if (type == HPlayerPositionType::Control)
		num = settings.value("position/control", QVariant(num)).toInt();
	else if (type == HPlayerPositionType::Left)
		num = settings.value("position/left", QVariant(num)).toInt();
	else if (type == HPlayerPositionType::Center)
		num = settings.value("position/center", QVariant(num)).toInt();
	else if (type == HPlayerPositionType::Right)
		num = settings.value("position/right", QVariant(num)).toInt();
	return num;
}


// Set the local machine's monitor ID for given position.
void habutilSetMonitorID(const HPlayerPositionType& type, int id)
{
	QSettings settings;
	if (type == HPlayerPositionType::Control)
		settings.setValue("position/control", QVariant(id));
	else if (type == HPlayerPositionType::Left)
		settings.setValue("position/left", QVariant(id));
	else if (type == HPlayerPositionType::Center)
		settings.setValue("position/center", QVariant(id));
	else if (type == HPlayerPositionType::Right)
		settings.setValue("position/right", QVariant(id));
	return;
}

bool habutilCompareVersions(QString v1, QString v2, int &result)
{
	bool b = false;
	int v1_a, v1_b, v1_c;
	int v2_a, v2_b, v2_c;
	QString v1_s, v2_s;

	result = 0;

	// parse the two strings
	QRegExp re("([0-9]+)[.]([0-9]+)[.]([0-9]+)(-([a-zA-Z0-9]+))?");
	if (re.exactMatch(v1))
	{
		v1_a = re.cap(1).toInt();
		v1_b = re.cap(2).toInt();
		v1_c = re.cap(3).toInt();
		v1_s = re.cap(4);
		b = true;
	}
	else
	{
		b = false;
	}

	if (b && re.exactMatch(v2))
	{
		v2_a = re.cap(1).toInt();
		v2_b = re.cap(2).toInt();
		v2_c = re.cap(3).toInt();
		v2_s = re.cap(4);
	}
	else
	{
		b = false;
	}

	// now do the comparison
	if (b)
	{
		if      (v1_a > v2_a)	result = 1;
		else if (v1_a < v2_a) 	result = -1;
		else if (v1_b > v2_b)	result = 1;
		else if (v1_b < v2_b)	result = -1;
		else if (v1_c > v2_c)	result = 1;
		else if (v1_c < v2_c)	result = -1;
		else if (v1_s > v2_s)	result = 1;
		else if (v1_s < v2_s)	result = -1;
		else result = 0;
	}


	qDebug() << "compare " << v1 << " " << v2 << " b " << b << " result " << result;

	return b;
}
