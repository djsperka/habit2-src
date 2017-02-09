/*
 * HStimulusOrder.h
 *
 *  Created on: Nov 20, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSORDER_H_
#define HSTIMULUSORDER_H_

#include <QStringList>
#include <QDebug>
#include "HTypes.h"
#include "HNameValidator.h"

namespace Habit
{
	class HStimulusOrder
	{
		int m_id;
		QString m_name;
		QStringList m_list;
		GUILib::HNameAndLabelRegExp m_re;

	public:
		HStimulusOrder(QString name = QString(), QStringList list = QStringList());
		HStimulusOrder(int id, QString name = QString(), QStringList list = QStringList());
		HStimulusOrder(const HStimulusOrder& o);
		virtual ~HStimulusOrder() {};
		HStimulusOrder& operator=(const HStimulusOrder& rhs);
		void setId(int id) { m_id = id; };
		int getId() const { return m_id; };
		void setName(const QString& name) { m_name = name; };
		QString getName() const { return m_name; };

		// Convenience - this is the clearing house for correct spelling of stimulus names and labels!
		static const QRegExp& getStimlusNameLabelRE();

		// the List is the list of strings as they should be stored in the database.
		// That means the strings may carry labels with them, i.e. "stimname/label" instead of
		// just "stimname". See getStim(), getLabel()

		void setList(const QStringList& list) { m_list = list; };
		const QStringList& getList() const { return m_list; };
		HStimulusOrder clone() const;

		static QString getStim(const QString& stimAndLabel);
		static QString getLabel(const QString& stimAndLabel);
		static QString formatStimLabel(const QString& stim, const QString& label = QString());

	};

//	typedef QList<HStimulusOrder> HStimulusOrderList;

	QDebug operator<<(QDebug dbg, const HStimulusOrder& ss);
	QDataStream & operator<< (QDataStream& stream, HStimulusOrder settings);
	QDataStream & operator>> (QDataStream& stream, HStimulusOrder& settings);
	bool operator==(const Habit::HStimulusOrder& lhs, const Habit::HStimulusOrder& rhs);

};

#endif /* HSTIMULUSORDER_H_ */
