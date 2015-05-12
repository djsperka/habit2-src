/*
 * HStimulusSettingsList.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: Oakeslab
 */

#include "HStimulusSettingsList.h"
#include <QListIterator>
using namespace Habit;

QStringList HStimulusSettingsList::names() const
{
	QStringList list;
	QListIterator<StimulusSettings> it(*this);
	while (it.hasNext())
		list.append(it.next().getName());
	return list;
}

bool HStimulusSettingsList::contains(const QString& name) const
{
	QListIterator<StimulusSettings> it(*this);
	QStringList nameList = names();
	return nameList.contains(name);
}

bool HStimulusSettingsList::contains(const QStringList& stimNames) const
{
	bool b=false;
	QStringList nameList = names();
	QStringListIterator it(stimNames);
	while (!b && it.hasNext())
		b = nameList.contains(it.next());
	return b;
}


