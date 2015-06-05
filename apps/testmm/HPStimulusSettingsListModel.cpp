/*
 * HPStimulusSettingsListModel.cpp
 *
 *  Created on: Jun 2, 2015
 *      Author: Oakeslab
 */

#include "HPStimulusSettingsListModel.h"
#include "HWorkspaceUtil.h"
#include "HStimulusUtil.h"
#include <QtDebug>
#include <QDir>
#include <QBrush>
#include <QMapIterator>
using namespace GUILib;

HPStimulusSettingsListModel::HPStimulusSettingsListModel(const QMap<unsigned int, const Habit::StimulusSettings *>& map)
{
	QMapIterator<unsigned int, const Habit::StimulusSettings *> it(map);
	while (it.hasNext())
	{
		it.next();
		m_list.append(QPair<unsigned int, const Habit::StimulusSettings *>(it.key(), it.value()));
	}
}


QVariant HPStimulusSettingsListModel::data(const QModelIndex & index, int role) const
{
	if ( role == Qt::DisplayRole )
	{
		return QVariant(m_list[index.row()].second->getName());
    }

/*
	else if (role == Qt::BackgroundRole)
	{
		// Make sure that the stimulus at this row has
		// 1) stim defined for each configured screen
		// 2) the stim file(s) exist
		if (!habutilStimulusFilesFound(m_list[index.row()], m_layoutType))
			return QBrush(Qt::yellow);
	}
	*/
    return QVariant();
}

QVariant HPStimulusSettingsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant("Stimulus");
}


int HPStimulusSettingsListModel::rowCount(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return m_list.size();
};


unsigned int HPStimulusSettingsListModel::getStimulusKey(int row)
{
	if (row >= 0 && row < m_list.size())
	{
		return m_list.at(row).first;
	}
	else
		return 0;
}


#if 0
void HStimulusSettingsListModel::changed(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
	emit dataChanged(topLeft, bottomRight);
}

void HStimulusSettingsListModel::append(const Habit::StimulusSettings& newStim)
{
	beginInsertRows(QModelIndex(), m_list.size(), m_list.size()+1);
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
	beginInsertRows(QModelIndex(), m_list.size(), m_list.size()+1);
	m_list.append(list);
	endInsertRows();
}

void HStimulusSettingsListModel::remove(const QModelIndex& index)
{
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	m_list.removeAt(index.row());
	endRemoveRows();
}
#endif


