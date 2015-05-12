/*
 * HStimulusOrder.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: Oakeslab
 */

#include "HStimulusOrder.h"
#include <QDebug>
#include <QRegExp>

static const QRegExp re("(\\w+)(/(\\w+))?");

QString Habit::HStimulusOrder::getStim(const QString& stimAndLabel)
{
	QString result;
	if (re.indexIn(stimAndLabel) > -1)
	{
		result = re.cap(1);
	}
	return result;
}

QString Habit::HStimulusOrder::getLabel(const QString& stimAndLabel)
{
	QString result;
	if (re.indexIn(stimAndLabel) > -1 && re.captureCount() == 3 && re.cap(3).length()>0)
	{
		result = re.cap(3);
	}
	return result;
}

QString Habit::HStimulusOrder::formatStimLabel(const QString& stim, const QString& label)
{
	if (label.isEmpty()) return stim;
	else return QString("%1/%2").arg(stim).arg(label);
}



Habit::HStimulusOrder::HStimulusOrder(const HStimContext& context, QString name, QStringList list)
: m_id(-1)
, m_pcontext(&context)
, m_name(name)
, m_list(list)
{
};

Habit::HStimulusOrder::HStimulusOrder(int id, const HStimContext& context, QString name, QStringList list)
: m_id(id)
, m_pcontext(&context)
, m_name(name)
, m_list(list)
{
};

Habit::HStimulusOrder::HStimulusOrder(const HStimulusOrder& o)
: m_id(o.getId())
, m_pcontext(o.getContext())
, m_name(o.getName())
, m_list(o.getList())
{
};


QDebug Habit::operator<<(QDebug dbg, const Habit::HStimulusOrder& order)
{
	dbg.nospace() << "name=" << order.getName() << " context=" << order.getContext()->name() << " order=" << order.getList().join(QString(",")) << endl;
	return dbg.nospace();
}

QDataStream & Habit::operator<< (QDataStream& stream, Habit::HStimulusOrder order)
{
	stream << order.getId() << order.getName() << order.getContext()->number() << order.getList();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::HStimulusOrder& settings)
{
	int id, icontext;
	QString name;
	QStringList list;
	stream >> id >> name >> icontext >> list;
	settings.setId(id);
	settings.setContext(getStimContext(icontext));
	settings.setName(name);
	settings.setList(list);
	return stream;
}

bool Habit::operator==(const Habit::HStimulusOrder&lhs, const Habit::HStimulusOrder& rhs)
{
	return (lhs.getContext() == rhs.getContext() &&
			lhs.getName() == rhs.getName() &&
			lhs.getList() == rhs.getList() &&
			lhs.getId() == rhs.getId());
}

const HStimContext* Habit::HStimulusOrder::getContext() const
{
	return m_pcontext;
}

void Habit::HStimulusOrder::setContext(const HStimContext& context)
{
	m_pcontext = &context;
}


Habit::HStimulusOrder& Habit::HStimulusOrder::operator=(const HStimulusOrder& rhs)
{
	if (&rhs != this)
	{
		setId(rhs.getId());
		setName(rhs.getName());
		setList(rhs.getList());
	}
	return *this;
};

Habit::HStimulusOrder Habit::HStimulusOrder::clone() const
{
	Habit::HStimulusOrder order(*this);
	order.setId(-1);
	return order;
}
