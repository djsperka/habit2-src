/*
 * HWorkspaceUtil.h
 *
 *  Created on: Jun 4, 2014
 *      Author: Oakeslab
 */

#ifndef HWORKSPACEUTIL_H_
#define HWORKSPACEUTIL_H_

#include <QDir>

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

// These assume you've already opened a workspace (called habutilInitWorkspace).
QDir habutilGetResultsDir();
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

#endif /* HWORKSPACEUTIL_H_ */
