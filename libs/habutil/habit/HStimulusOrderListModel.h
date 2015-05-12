/*
 * HStimulusOrderListModel.h
 *
 *  Created on: Nov 20, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSORDERLISTMODEL_H_
#define HSTIMULUSORDERLISTMODEL_H_

#include <QAbstractListModel>
#include <QStringList>
#include "stimulisettings.h"

namespace GUILib
{
	class HStimulusOrderListModel: public QAbstractListModel
	{
		Q_OBJECT
	private:
		Habit::HStimulusOrderList& m_list;
		const Habit::HStimulusSettingsList& m_ssList;
	public:

		// note a non-const ref to the order list is supplied and uesd. It is modified here,
		// so the caller should make sure to provide the correct ref to capture changes made.
		HStimulusOrderListModel(Habit::HStimulusOrderList& list, const Habit::HStimulusSettingsList& ssList): m_list(list), m_ssList(ssList) {};
		virtual ~HStimulusOrderListModel() {};
		int rowCount(const QModelIndex& index) const;
		QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		void changed(const QModelIndex & topLeft, const QModelIndex & bottomRight);
		void append(const Habit::HStimulusOrder& order);
		void clobber(const Habit::HStimulusOrder& order);
		void append(const Habit::HStimulusOrderList& list);
		void remove(const QModelIndex& index);
		void replace(const QModelIndex& index, const Habit::HStimulusOrder& order);
		QStringList names();
	};
}




#endif /* HSTIMULUSORDERLISTMODEL_H_ */
