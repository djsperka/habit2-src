/*
 * HOrderSettings.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: Oakeslab
 */

#include "HOrderSettings.h"
#include <QListIterator>
#include <QTextStream>
#include <QtDebug>

using namespace Habit;

HOrderSettings::HOrderSettings(const HOrderSettings& order)
{
	*this = order;
}


HOrderSettings& HOrderSettings::operator=(const HOrderSettings& rhs)
{
	if (this != &rhs)
	{
		this->setIsRandomized(rhs.getIsRandomized());
		this->setRandomizationType(rhs.getRandomizationType());
		QList<int> list;
		rhs.getList(list);
		this->setList(list);
	}
	return *this;
};


// return list as a comma-separated string
QString HOrderSettings::getCanonicalList() const
{
	QString s;
	QTextStream stream(&s);
	QListIterator<int> it(m_list);
	if (it.hasNext())
		stream << it.next();
	while (it.hasNext())
		stream << "," << it.next();
	return s;
}

// clear current list and use the supplied list
void HOrderSettings::setList(const QList<int>& list)
{
	m_list.clear();
	m_list.append(list);
}


// copy current list to the supplied list (append, don't clear)
void HOrderSettings::getList(QList<int>& list) const
{
	list = m_list;
}




QDebug Habit::operator<<(QDebug dbg, const HOrderSettings& os)
{
	dbg.nospace().noquote() << "HOrderSettings: list " << os.getCanonicalList() << " randomized? " << os.getIsRandomized() << "(" << os.getRandomizationType().name() << ")" << "\n";
	return dbg.nospace();
}
