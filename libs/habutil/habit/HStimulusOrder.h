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

namespace Habit
{
	class HStimulusOrder
	{
		int m_id;
		const HStimContext* m_pcontext;
		QString m_name;
		QStringList m_list;


	public:
		HStimulusOrder(const HStimContext& context = HStimContext::UnknownContext, QString name = QString(), QStringList list = QStringList());
		HStimulusOrder(int id, const HStimContext& context = HStimContext::UnknownContext, QString name = QString(), QStringList list = QStringList());
		HStimulusOrder(const HStimulusOrder& o);
		virtual ~HStimulusOrder() {};
		HStimulusOrder& operator=(const HStimulusOrder& rhs);
		void setId(int id) { m_id = id; };
		int getId() const { return m_id; };
		const HStimContext* getContext() const;
		void setContext(const HStimContext& context);
		void setName(const QString& name) { m_name = name; };
		QString getName() const { return m_name; };

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
