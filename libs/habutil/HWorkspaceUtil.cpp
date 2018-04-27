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
#include <QtGlobal>
#if QT_VERSION < 0x050000
#include <QDesktopServices>
#endif



// These are all set when habutilInitWorkspace is called
// You can call habutilSetWorkspace and habutilCreateWorkspace, then call this to set up.
// Once this is called, you are "in" the workspace.
// The way this is done: We form a key using the workspace dir (with path separators replaced with % or something like that -
// we won't use it to re-form the dir anyways.
//
// When this is unleashed, all existing installations will not have any of these settings. I will
// leave the existing ones in place, and automatically create new ones for each workspace entered using those
// values as the default.

QDir f_workspaceDir;						//	"workspace"
bool f_bWorkspaceDirOK = false;
QDir f_stimulusRootDir;						//	"stimroot" saved value. Not used if default specified.
bool f_stimulusRootUseDefault = false;		//	"stimroot_use_default"
QDir f_lastVideoImageDir;					//	"lastdir/selectVideoImage"
QDir f_lastAudioDir;						//	"lastdir/selectAudio"
int f_iControlMonitor = -1;					//	"position/control"
int f_iLeftMonitor = -1;					//	"position/left"
int f_iCenterMonitor = -1;					//	"position/center"
int f_iRightMonitor = -1;					//	"position/right"

void _getWorkspaceDir()
{
	QSettings settings;
	if (!settings.contains("workspace"))
	{
#if QT_VERSION >= 0x050000
		QString defaultDir = QString("%1/habit").arg(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]);
#else
		QString defaultDir = QString("%1/habit").arg(QDesktopServices::standardLocations(QDesktopServices::DocumentsLocation));
#endif
		f_workspaceDir.setPath(defaultDir);
		settings.setValue("workspace", f_workspaceDir.absolutePath());
	}
	else
	{
		f_workspaceDir.setPath(settings.value("workspace").toString());
	}
}

// Gets current workspace dir, then converts to QSettings-friendly key.
QString _getWorkspaceKey()
{
	_getWorkspaceDir();
	return QDir::toNativeSeparators(f_workspaceDir.canonicalPath()).replace(QDir::separator(), '%');
}

void _loadSettings()
{
	QString key = _getWorkspaceKey();	// this calls _getWorkspaceDir
	QSettings settings;
	if (settings.childGroups().contains(key))
	{
		settings.beginGroup(key);
		qDebug() << "_loadSettings: " << key << " found";
	}
	else
		qDebug() << "_loadSettings: " << key << " NOT found";

	qDebug() << "All keys " << settings.allKeys();
	qDebug() << "stimnroot " << settings.value("stimroot").toString();
	// stim root
	f_stimulusRootDir.setPath(settings.value("stimroot").toString());
	f_stimulusRootUseDefault = settings.value("stimroot_use_default", false).toBool();
	f_lastVideoImageDir.setPath(settings.value("lastdir/selectVideoImage").toString());
	f_lastAudioDir.setPath(settings.value("lastdir/selectAudio").toString());
	f_iControlMonitor = settings.value("position/control", -1).toInt();
	f_iLeftMonitor = settings.value("position/left", -1).toInt();
	f_iCenterMonitor = settings.value("position/center", -1).toInt();
	f_iRightMonitor = settings.value("position/right", -1).toInt();

	qDebug() << "_loadSettings: stimroot " << f_stimulusRootDir;
	qDebug() << "_loadSettings: use defaiult " << f_stimulusRootUseDefault;
	qDebug() << "_loadSettings: last vi " << f_lastVideoImageDir;
	qDebug() << "_loadSettings: last audio " << f_lastAudioDir;
	qDebug() << "_loadSettings: control/left/center/right " << f_iControlMonitor << "/" << f_iLeftMonitor << "/" << f_iCenterMonitor << "/" << f_iRightMonitor;

	if (settings.contains(key))
	{
		settings.endGroup();
	}
}


void _saveSettings()
{
	QString key = _getWorkspaceKey();
	QSettings settings;
	settings.beginGroup(key);
	settings.setValue("stimroot", f_stimulusRootDir.absolutePath());
	settings.setValue("stimroot_use_default", f_stimulusRootUseDefault);
	settings.setValue("lastdir/selectVideoImage", f_lastVideoImageDir.absolutePath());
	settings.setValue("lastdir/selectAudio", f_lastAudioDir.absolutePath());
	settings.setValue("position/control", f_iControlMonitor);
	settings.setValue("position/left", f_iLeftMonitor);
	settings.setValue("position/center", f_iCenterMonitor);
	settings.setValue("position/right", f_iRightMonitor);
	settings.endGroup();
}

void _saveWorkspaceDir(const QDir& dir)
{
	QSettings settings;
	settings.setValue("workspace", dir.absolutePath());
}

void habutilSaveSettings()
{
	_saveSettings();
}

// Test whether a directory is a valid workspace folder. Return true if it is, otherwise false.
// A dir is a valid workspace if all of the following are true
// 1. dir exists
// 2. dir contains a file called "habit.sqlite" or "habit22.sqlite"
// 3. dir contains a folder called "log"
// 4. dir contains a folder called "results"
// 5. dir contains a folder called "stim"

bool habutilIsValidWorkspace(QDir dir)
{
	qDebug() << "valid? " << dir;
	qDebug() << dir.exists() << "/" << (dir.exists("habit.sqlite") || dir.exists("habit22.sqlite")) << "/" << dir.exists("results") << "/" << dir.exists("log") << "/" << dir.exists("stim");
	return (dir.exists() && (dir.exists("habit.sqlite") || dir.exists("habit22.sqlite")) && dir.exists("results") && dir.exists("log") && dir.exists("stim"));
}


// Create a new workspace in the dir 'dir'.
// Returns false if a workspace already exists in this location, so better to call
// habutilIsValidWorkspace first.
bool habutilCreateWorkspace(const QDir& dir)
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
	if (!dir.exists("habit22.sqlite"))
	{
		// copy database from resources
		if (!QFile::copy(":/resources/habit22.sqlite", dir.absoluteFilePath("habit22.sqlite")))
		{
			qCritical() << "Cannot create database file " << dir.absoluteFilePath("habit22.sqlite");
			return false;
		}
		else
		{
			QFile newfile(dir.absoluteFilePath("habit22.sqlite"));
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

	// stim dir exists, so create a link to the stock images etc.
	qDebug() << "Check link to example stimuli...";
#ifdef Q_OS_MACOS
	// Note that this folder is _named_ in the distribution folder - it is named there and deployed during install to the
	// machine.
	QFile appSupport("/Library/Application Support/habit2/stim/examples");
	QFile examples(dir.absoluteFilePath("stim/examples"));
	if (!appSupport.exists())
	{
		qWarning() << "Application Support folder " << appSupport.fileName() << " not found.";
	}
	else
	{
		if (examples.exists())
		{
			qWarning() << "Examples folder already exists in this workspace.";
		}
		else
		{
			if (appSupport.link(examples.fileName()))
			{
				qDebug() << "Created link " << examples.fileName() << " --> " << appSupport.fileName();
			}
			else
			{
				qWarning() << "Cannot create link " << examples.fileName() << " --> " << appSupport.fileName();
			}
		}
	}
#elif Q_OS_WIN
    QProcess process;
    process.start("mklink /D");

    // Wait for it to start
    if(!process.waitForStarted())
        return 0;

    bool retval = false;
    QByteArray buffer;
    while ((retval = process.waitForFinished()));
        buffer.append(process.readAll());

    if (!retval) {
        qDebug() << "Process error:" << process.errorString();
        qDebug() << "Output:" << buffer;
        return 1;
    }
#endif

	return true;
}


// Called on startup.
// 1. Check settings for saved workspace "workspace". If the setting doesn't exist, create default workspace
//    in documents folder and add setting.
// 2. Open database

bool habutilInitWorkspace()
{
	bool b = true;
	_loadSettings();
	qDebug() << "Habit is opening workspace directory \"" << f_workspaceDir.absolutePath() << "\"";
	if (!habutilIsValidWorkspace(f_workspaceDir))
	{
		qDebug() << "The workspace directory is not a valid Habit workspace. Creating ...";
		b = habutilCreateWorkspace(f_workspaceDir);
	}

	// if all is well, open the database
	if (b)
	{
		b = openDB(f_workspaceDir);
	}

	return b;
}


// Prompt user to select a workspace folder.
// A new workspace is created in that folder if needed, and settings are updated.
// The database is not opened, call habutilInitWorkspace() to do that.

bool habutilSelectWorkspace()
{
	bool b = false;
	QString selectedDir;

	selectedDir = QFileDialog::getExistingDirectory(0, "Select Habit Workspace Folder", f_workspaceDir.absolutePath());
	if (selectedDir.isEmpty() || selectedDir.isNull())
		return false;
	else
	{
		b = habutilCreateWorkspace(QDir(selectedDir));
	}

	if (b)
	{
		// save new workspace dir
		_saveWorkspaceDir(selectedDir);

		// load new workspace settings
		_loadSettings();
	}
	return b;
}


// get workspace dir.
// If settings "workspace" exists, use it
// If that setting is not set, use default location (and create setting).
// Returns true if the folder is a valid workspace, false if not.

const QDir& habutilGetWorkspaceDir()
{
	return f_workspaceDir;
}


// Get "results" subfolder for the given experiment within the current workspace. It will be "results/expt" inside
// current workspace.
QDir habutilGetResultsDir(const QString expt)
{
	QDir dir(f_workspaceDir);
	bool b = dir.cd("results");
	Q_ASSERT(b);
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
				dir.cd(expt);
			}
		}
		else
		{
			dir.cd(expt);
		}
	}
	return dir;
}

// Get the "log" folder for current workspace.
QDir habutilGetLogDir()
{
	QDir dir(f_workspaceDir);
	bool b = dir.cd("log");
	Q_ASSERT(b);
	return dir;
}

// Get "stim" folder for current workspace.
QDir habutilGetStimDir()
{
	QDir dir(f_workspaceDir);
	bool b = dir.cd("stim");
	Q_ASSERT(b);
	return dir;
}

// Set "workspace" setting. The dir is not tested for validity.
void habutilSetWorkspace(const QString& d)
{
	QSettings settings;
	settings.setValue("workspace", d);
	_loadSettings();
	return;
}

QDir habutilGetStimulusRootDir()
{
	QDir dir(f_stimulusRootDir);
	if (f_stimulusRootUseDefault)
	{
		dir = f_workspaceDir;
		dir.cd("stim");
	}
	return dir;
}

bool habutilSelectStimulusRootDir(QString& sDir)
{
	bool b = false;
	QString selectedDir;
	selectedDir = QFileDialog::getExistingDirectory(0, "Select Habit Stimulus Root Folder", sDir);
	if (selectedDir.isEmpty() || selectedDir.isNull())
	{
		b = false;
	}
	else
	{
		sDir = selectedDir;
		b = true;
	}
	return b;
}

void habutilSetUseDefaultStimRoot(bool b)
{
	f_stimulusRootUseDefault = b;
	_saveSettings();
	return;
}


// Set stimroot to given dir
void habutilSetStimulusRootDir(const QString& d)
{
	f_stimulusRootDir.setPath(d);
	_saveSettings();
	return;
}


// get last dir that a stim was selected from
QDir habutilGetLastDir(bool isVideoImage)
{
	if (isVideoImage)
		return f_lastVideoImageDir;
	else
		return f_lastAudioDir;
}

// set last dir that a stim was selected from
void habutilSetLastDir(const QString& d, bool isVideoImage)
{
	if (isVideoImage) f_lastVideoImageDir.setPath(d);
	else f_lastAudioDir.setPath(d);
	_saveSettings();
	return;
}


// Get the local machine's monitor ID for given position.
int habutilGetMonitorID(const HPlayerPositionType& type)
{
	int num = -1;	// default is NO MONITOR
	if (type == HPlayerPositionType::Control)
		num = f_iControlMonitor;
	else if (type == HPlayerPositionType::Left)
		num = f_iLeftMonitor;
	else if (type == HPlayerPositionType::Center)
		num = f_iCenterMonitor;
	else if (type == HPlayerPositionType::Right)
		num = f_iRightMonitor;
	return num;
}


// Set the local machine's monitor ID for given position.
void habutilSetMonitorID(const HPlayerPositionType& type, int id)
{
	if (type == HPlayerPositionType::Control)
		f_iControlMonitor = id;
	else if (type == HPlayerPositionType::Left)
		f_iLeftMonitor = id;
	else if (type == HPlayerPositionType::Center)
		f_iCenterMonitor = id;
	else if (type == HPlayerPositionType::Right)
		f_iRightMonitor = id;
	_saveSettings();
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
