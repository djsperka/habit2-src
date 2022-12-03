/*
 * HStimulusOrderList.h
 *
 *  Created on: Dec 8, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSORDERLIST_H_
#define HSTIMULUSORDERLIST_H_

#include "HStimulusOrder.h"
#include <QList>
#include <QString>
#include <QStringList>

namespace Habit
{
	class HStimulusOrderList: public QList<HStimulusOrder>
	{
	public:
		HStimulusOrderList(): QList<HStimulusOrder>() {};
		//HStimulusOrderList(const HStimulusOrderList& list) : QList<HStimulusOrder>(list) {};
		bool contains(const QString& name) const;
		bool contains(const QStringList& names) const;
		QStringList names() const;
		bool find(const QString& name, HStimulusOrder& order) const;
	};
};



#endif /* HSTIMULUSORDERLIST_H_ */
