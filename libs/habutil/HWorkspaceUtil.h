/*
 * HWorkspaceUtil.h
 *
 *  Created on: Jun 4, 2014
 *      Author: Oakeslab
 */

#ifndef HWORKSPACEUTIL_H_
#define HWORKSPACEUTIL_H_

#include <QDir>
#include "HTypes.h"

// A dir is a valid workspace if all of the following are true
// 1. dir exists
// 2. dir contains a file called "habit.sqlite"
// 3. dir contains a folder called "log"
// 4. dir contains a folder called "results"
// 5. dir contains a folder called "stim"

bool habutilIsValidWorkspace(QDir dir);


// Create a new workspace in the dir 'dir'. A new database and the necessary folders will be created.
// It is safe to call this function with an already-existing workspace dir. No new files/folders
// will be created in that case, and true will be returned.

bool habutilCreateWorkspace(QDir& dir);


// Called on startup.
// 1. Check settings for saved workspace "workspace". If the setting doesn't exist, create default workspace
//    in documents folder and add setting.
// 2. Open database

bool habutilInitWorkspace();

// get workspace dir.
// If settings "workspace" exists, use it
// If that setting is not set, use default location (and create setting).
// Returns true if the folder is a valid workspace, false if not.

bool habutilGetWorkspaceDir(QDir& dir);

// get workspace dir currently set in preferences. Returns absolute dir as string.
QString habutilGetWorkspaceDir();

// These assume you've already opened a workspace (called habutilInitWorkspace).
QDir habutilGetResultsDir(const QString expt = QString());
QDir habutilGetLogDir();
QDir habutilGetStimDir();


// Prompt user to select a workspace folder.
// A new workspace is created in that folder if needed, and settings are updated.
// The database is not opened, call habutilInitWorkspace() to do that.

bool habutilSelectWorkspace();


// Testing utility. Call to delete the "workspace" setting from settings.

void habutilClearWorkspace();


// Set workspace to given dir
void habutilSetWorkspace(const QString& d);


// get stimulus root dir.
// If settings "stimroot" exists, use it
// If that setting is not set, use default location getStimDir() (and create setting).
// Returns true (false) if the stim root dir exists (does not exist).
bool habutilGetStimulusRootDir(QDir& dir);

// just gimme the dir, nevermind whether it exists.
QString habutilGetStimulusRootDir();

// Prompt user to select a stimulus root folder.

bool habutilSelectStimulusRootDir();


// Testing utility. Call to delete the "stimroot" setting from settings.
void habutilClearStimulusRootDir();


// Set stimroot to given dir
void habutilSetStimulusRootDir(const QString& d);

// get/set state of "use default stim root" checkbox
bool habutilGetUseDefaultStimRoot();
void habutilSetUseDefaultStimRoot(bool b);

// get last dir that a stim was selected from
bool habutilGetLastDir(QDir& dir, bool isVideoImage = true);

// set last dir that a stim was selected from
void habutilSetLastDir(const QString& d, bool isVideoImage = true);

// get/set monitor id for Control, left, right, etc
int habutilGetMonitorID(const HPlayerPositionType& type);
void habutilSetMonitorID(const HPlayerPositionType& type, int id);

// Compare version v1 to v2. If both version numbers follow pattern
// A.B.C[-Z], where A, B, C are numbers. Z can be any numbers and letters.
// If A, B, C are all equal, then regular alpha comparison on Z wins.
bool habutilCompareVersions(QString v1, QString v2, int &result);



#endif /* HWORKSPACEUTIL_H_ */
