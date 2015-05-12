/*
 * HStimulusSettingsList.h
 *
 *  Created on: Dec 4, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSSETTINGSLIST_H_
#define HSTIMULUSSETTINGSLIST_H_

#include "stimulussettings.h"
#include <QList>
#include <QString>
#include <QStringList>

namespace Habit
{
	class HStimulusSettingsList: public QList<StimulusSettings>
	{
	public:
		HStimulusSettingsList(): QList<StimulusSettings>() {};
		HStimulusSettingsList(const HStimulusSettingsList& list) : QList<StimulusSettings>(list) {};
		bool contains(const QString& name) const;
		bool contains(const QStringList& names) const;
		QStringList names() const;
	};
}



#endif /* HSTIMULUSSETTINGSLIST_H_ */
