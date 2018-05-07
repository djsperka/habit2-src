/*
 * HPhaseListModel.h
 *
 *  Created on: Jun 24, 2016
 *      Author: dan
 */

#ifndef HPHASELISTMODEL_H_
#define HPHASELISTMODEL_H_

#include <QAbstractListModel>
#include <QStringList>
#include "HPhaseSettings.h"

namespace GUILib
{
	class HPhaseListModel: public QAbstractListModel
	{
		Q_OBJECT
	private:
		QList<Habit::HPhaseSettings> m_phases;

	public:

		HPhaseListModel(QList<Habit::HPhaseSettings>& list): m_phases(list) {};
		virtual ~HPhaseListModel() {};
		int rowCount(const QModelIndex& index) const;
		QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		void changed(const QModelIndex & topLeft, const QModelIndex & bottomRight);
		void append(const Habit::HPhaseSettings& phase);
		void remove(const QModelIndex& index);
		void replace(const QModelIndex& index, const Habit::HPhaseSettings& phase);
	};
};




#endif /* HPHASELISTMODEL_H_ */
