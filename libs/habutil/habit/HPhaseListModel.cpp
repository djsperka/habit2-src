/*
 * HPhaseListModel.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: dan
 */

#include "HPhaseListModel.h"
#include <QtDebug>
#include <QBrush>
#include <QListIterator>

using namespace GUILib;
using namespace Habit;

QVariant HPhaseListModel::data(const QModelIndex & index, int role) const
{
	QVariant v;
	if ( role == Qt::DisplayRole )
	{
		v = QVariant(m_phases[index.row()].getName());
    }
	else if (role == Qt::BackgroundRole)
	{
		if (m_phases[index.row()].getIsEnabled()) v = QBrush(Qt::white);
		else v = QBrush(Qt::gray);
	}
    return v;
}


// TODO: probably get rid of header?
QVariant HPhaseListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant("Phase");
}


int HPhaseListModel::rowCount(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return m_phases.size();
};

void HPhaseListModel::changed(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
	emit dataChanged(topLeft, bottomRight);
}

void HPhaseListModel::append(const Habit::HPhaseSettings& phase)
{
	beginInsertRows(QModelIndex(), m_phases.size(), m_phases.size()+1);
	m_phases.append(phase);
	endInsertRows();
}

void HPhaseListModel::remove(const QModelIndex& index)
{
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	m_phases.removeAt(index.row());
	endRemoveRows();
}

void HPhaseListModel::replace(const QModelIndex& index, const Habit::HPhaseSettings& phase)
{
	m_phases[index.row()] = phase;
	emit dataChanged(index, index);
}






