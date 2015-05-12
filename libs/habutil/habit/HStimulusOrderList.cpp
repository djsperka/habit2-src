/*
 * HStimulusOrderList.cpp
 *
 *  Created on: Dec 8, 2014
 *      Author: Oakeslab
 */

#include "HStimulusOrderList.h"
#include <QListIterator>

using namespace Habit;

QStringList HStimulusOrderList::names() const
{
	QStringList list;
	QListIterator<HStimulusOrder> it(*this);
	while (it.hasNext())
		list.append(it.next().getName());
	return list;
}

bool HStimulusOrderList::contains(const QString& name) const
{
	QListIterator<HStimulusOrder> it(*this);
	QStringList nameList = names();
	return nameList.contains(name);
}

bool HStimulusOrderList::contains(const QStringList& orderNames) const
{
	bool b=false;
	QStringList nameList = names();
	QStringListIterator it(orderNames);
	while (!b && it.hasNext())
		b = nameList.contains(it.next());
	return b;
}

bool HStimulusOrderList::find(const QString& name, HStimulusOrder& order) const
{
	bool b = false;
	QListIterator<HStimulusOrder> it(*this);
	while (!b && it.hasNext())
		b = it.next().getName() == name;
	if (b)
		order = it.peekPrevious();
	return b;
}



