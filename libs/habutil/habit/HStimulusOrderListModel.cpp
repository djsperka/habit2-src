/*
 * HStimulusOrderListModel.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: Oakeslab
 */


#include "HStimulusOrderListModel.h"
#include <QtDebug>
#include <QBrush>
#include <QListIterator>

using namespace GUILib;
using namespace Habit;

QStringList HStimulusOrderListModel::names()
{
	QStringList names;
	Habit::HStimulusOrder order;
	foreach (order, m_list)
	{
		names.append(order.getName());
	}
	return names;
}

QVariant HStimulusOrderListModel::data(const QModelIndex & index, int role) const
{
	if ( role == Qt::DisplayRole )
	{
		return QVariant(m_list[index.row()].getName());
    }
	else if (role == Qt::BackgroundRole)
	{
		// Determine if the order at this index is valid (i.e. all stimuli in the
		// order are in fact found in the stimulus settings list.
		//qDebug() << "HStimulusOrderListModel::data(BackgroundRole) for order " << m_list[index.row()].getName();
		QStringList names = m_ssList.names();
		QStringList stimNamesThisOrder(m_list[index.row()].getList());
		//qDebug() << "allStimNames=" << allStimNames.join(" | ");
		//qDebug() << "stimNamesThisOrder=" << stimNamesThisOrder.join(" | ");
		QStringListIterator it(stimNamesThisOrder);
		bool b = true;
		while (b && it.hasNext())
		{
			QString stimLabel = it.next();
			b = names.contains(HStimulusOrder::getStim(stimLabel));
			//qDebug() << "Look for " << stimLabel << " stim= " << HStimulusOrder::getStim(stimLabel) << " in stim list?  " << b;
		}

		if (!b)
		{
			qDebug() << "YELLOW";
			qDebug() << "stimNames in order: " << stimNamesThisOrder;
			qDebug() << "stim names in sslist: " << names;
		}
		if (!b) return QBrush(Qt::yellow);
	}
    return QVariant();
}

QVariant HStimulusOrderListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant("Order");
}


int HStimulusOrderListModel::rowCount(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return m_list.size();
};

void HStimulusOrderListModel::changed(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
	emit dataChanged(topLeft, bottomRight);
}

void HStimulusOrderListModel::append(const Habit::HStimulusOrder& order)
{
	beginInsertRows(QModelIndex(), m_list.size(), m_list.size()+1);
	m_list.append(order);
	endInsertRows();
}

void HStimulusOrderListModel::append(const Habit::HStimulusOrderList& list)
{
	beginInsertRows(QModelIndex(), m_list.size(), m_list.size()+1);
	m_list.append(list);
	endInsertRows();
}

void HStimulusOrderListModel::clobber(const Habit::HStimulusOrder& order)
{
	int i;
	// swap the input order into the underlying list
	for (i=0; i<m_list.size(); i++)
		if (m_list[i].getName() == order.getName()) break;
	if (i<m_list.size())
		m_list.replace(i, order);
}


void HStimulusOrderListModel::remove(const QModelIndex& index)
{
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	m_list.removeAt(index.row());
	endRemoveRows();
}

void HStimulusOrderListModel::replace(const QModelIndex& index, const Habit::HStimulusOrder& order)
{
	m_list[index.row()] = order;
	emit dataChanged(index, index);
}



