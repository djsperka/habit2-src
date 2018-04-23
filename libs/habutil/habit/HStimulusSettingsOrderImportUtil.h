/*
 * HStimulusSettingsOrderImportUtil.h
 *
 *  Created on: Nov 20, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSSETTINGSORDERIMPORTUTIL_H_
#define HSTIMULUSSETTINGSORDERIMPORTUTIL_H_

#include "stimulussettings.h"
#include "HStimulusOrder.h"
#include "HStimulusOrderList.h"
#include "HStimulusSettingsList.h"
#include "HTypes.h"

// import stim settings and orders from a file.
// Returns true if the file was found and opened and no parsing errors were encountered.
bool importStimulusSettingsAndOrders(const QString& filename, Habit::HStimulusSettingsList& settingsList, Habit::HStimulusOrderList& orderList);

#endif /* HSTIMULUSSETTINGSORDERIMPORTUTIL_H_ */
