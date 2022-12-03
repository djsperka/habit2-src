/*
 * HStimulusOrder.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: Oakeslab
 */

#include "HStimulusOrder.h"
#include "HNameValidator.h"
#include <QDebug>
#include <QRegExp>
#include <QDataStream>

//static const QRegExp re("(\\w+)(/(\\w+))?");
//static const QString sreWordWithSpaces("((\\w)|(\\w[ '_-\\w]*\\w))");
//static const QString sreWordWithPunctuation("((\\w)|(\\w[ \\(\\)\\!\\#\\$\\%\\&\\*\\[\\]\\{\\}\\:\\<\\>'_-\\w]*\\w))");
//static const QRegExp reStimAndLabel(GUILib::HNameAndLabelValidator::getNameAndLabelRE());
//static const QRegExp reHabitNames(sreWordWithPunctuation + "(/" + sreWordWithPunctuation + ")?");

//const QRegExp& Habit::HStimulusOrder::getStimlusNameLabelRE()
//{
//	return reStimAndLabel;
//}


QString Habit::HStimulusOrder::getStim(const QString& stimAndLabel)
{
	QString result;

#if 0
	// test
	qDebug() << "HStimulusOrder::getStim(" << stimAndLabel << "):";
	if (reStimAndLabel.indexIn(stimAndLabel) > -1)
	{
		qDebug() << "cap count is " << reStimAndLabel.captureCount();
		qDebug() << "stim cap 1 2 3 " << reStimAndLabel.cap(1) << " " << reStimAndLabel.cap(2) << " " << reStimAndLabel.cap(3);
		qDebug() << "label cap 5 6 7 " << reStimAndLabel.cap(5) << " " << reStimAndLabel.cap(6) << " " << reStimAndLabel.cap(7);
	}
	else
	{
		qDebug() << "No Match.";
	}
#endif

	if (GUILib::HName::getNameAndLabelRE().indexIn(stimAndLabel) > -1)
	{
		result = GUILib::HName::getNameAndLabelRE().cap(1);
	}

	return result;
}

QString Habit::HStimulusOrder::getLabel(const QString& stimAndLabel)
{
	QString result;
	if (GUILib::HName::getNameAndLabelRE().indexIn(stimAndLabel) > -1)
	{
		result = GUILib::HName::getNameAndLabelRE().cap(3);
	}
	return result;
}

QString Habit::HStimulusOrder::formatStimLabel(const QString& stim, const QString& label)
{
	if (label.trimmed().isEmpty()) return stim.trimmed();
	else return QString("%1/%2").arg(stim.trimmed()).arg(label.trimmed());
}



Habit::HStimulusOrder::HStimulusOrder(QString name, QStringList list)
: m_id(-1)
, m_name(name)
, m_list(list)
{
};

Habit::HStimulusOrder::HStimulusOrder(int id, QString name, QStringList list)
: m_id(id)
, m_name(name)
, m_list(list)
{
};

Habit::HStimulusOrder::HStimulusOrder(const HStimulusOrder& o)
: m_id(o.getId())
, m_name(o.getName())
, m_list(o.getList())
{
};


QDebug Habit::operator<<(QDebug dbg, const Habit::HStimulusOrder& order)
{
	dbg.nospace() << "name=" << order.getName() << " order=" << order.getList().join(QString(",")) << Qt::endl;
	return dbg.nospace();
}

// to maintain compatibility, write out an int in lieu of the context(which has been removed).
QDataStream & Habit::operator<< (QDataStream& stream, Habit::HStimulusOrder order)
{
	int i=0;
	stream << order.getId() << order.getName() << i << order.getList();
	return stream;
}

QDataStream & Habit::operator>> (QDataStream& stream, Habit::HStimulusOrder& settings)
{
	int id, icontext;
	QString name;
	QStringList list;
	stream >> id >> name >> icontext >> list;
	settings.setId(id);
	// djs context removed. We ignore the value read in. Output method dumps a zero here. //settings.setContext(getStimContext(icontext));
	settings.setName(name);
	settings.setList(list);
	return stream;
}

bool Habit::operator==(const Habit::HStimulusOrder&lhs, const Habit::HStimulusOrder& rhs)
{
	return (lhs.getName() == rhs.getName() &&
			lhs.getList() == rhs.getList() &&
			lhs.getId() == rhs.getId());
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
