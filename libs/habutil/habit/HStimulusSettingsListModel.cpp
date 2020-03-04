/*
 * HStimulusSettingsListModel.cpp
 *
 *  Created on: Nov 4, 2014
 *      Author: Oakeslab
 */

#include "HStimulusSettingsListModel.h"
#include "HWorkspaceUtil.h"
#include "HStimulusUtil.h"
#include <QtDebug>
#include <QDir>
#include <QBrush>

using namespace GUILib;

QVariant HStimulusSettingsListModel::data(const QModelIndex & index, int role) const
{
	//qDebug() << "Data() for row " << index.row() << ":" << m_list[index.row()].getName() << ":";

	if ( role == Qt::DisplayRole ) {
		return QVariant(m_list[index.row()].getName());
    }
	else if (role == Qt::BackgroundRole)
	{
		// Make sure that the stimulus at this row has
		// 1) stim defined for each configured screen
		// 2) the stim file(s) exist
		if (!habutilStimulusFilesFound(m_list[index.row()], m_sdi))
			return QBrush(Qt::yellow);
	}
    return QVariant();
}

QVariant HStimulusSettingsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant("Stimulus");
}


int HStimulusSettingsListModel::rowCount(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return m_list.size();
};

void HStimulusSettingsListModel::changed(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
	emit dataChanged(topLeft, bottomRight);
}

void HStimulusSettingsListModel::append(const Habit::StimulusSettings& newStim)
{
	beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
	m_list.append(newStim);
	endInsertRows();
}

void HStimulusSettingsListModel::clobber(const Habit::StimulusSettings& stim)
{
	int i;
	// swap the input order into the underlying list
	for (i=0; i<m_list.size(); i++)
		if (m_list[i].getName() == stim.getName()) break;
	if (i<m_list.size())
		m_list.replace(i, stim);
}


void HStimulusSettingsListModel::append(const QList<Habit::StimulusSettings>& list)
{
	beginInsertRows(QModelIndex(), m_list.size(), m_list.size()+list.size()-1);
	m_list.append(list);
	endInsertRows();
}

void HStimulusSettingsListModel::remove(const QModelIndex& index)
{
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	m_list.removeAt(index.row());
	endRemoveRows();
}

void HStimulusSettingsListModel::removeAll()
{
	while (!m_list.isEmpty())
		remove(index(0));
}
