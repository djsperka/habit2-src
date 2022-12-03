/*
 * HStimulusNameLabelTableModel.cpp
 *
 *  Created on: Apr 21, 2015
 *      Author: Oakeslab
 */


#include "HStimulusNameLabelTableModel.h"
#include "HStimulusOrder.h"

using namespace Habit;
using namespace GUILib;


HStimulusNameLabelTableModel::HStimulusNameLabelTableModel(const QStringList &namesAndLabels, const QStringList& allStimNames, QObject * parent)
: QAbstractTableModel(parent)
, m_stimNamesAndLabels(namesAndLabels)
, m_allStimNames(allStimNames)
{
}

QVariant HStimulusNameLabelTableModel::data(const QModelIndex & index, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (index.column() == 0)
		{
			qDebug() << "HStimulusNameLabelTableModel::data(" << index.row() << "," << index.column() << "): StimulusOrder::getStim(" << m_stimNamesAndLabels.at(index.row()) << ") = " << HStimulusOrder::getStim(m_stimNamesAndLabels.at(index.row()));
			return QVariant(HStimulusOrder::getStim(m_stimNamesAndLabels.at(index.row())));
		}
		else
		{
			qDebug() << "HStimulusNameLabelTableModel::data(" << index.row() << "," << index.column() << "): StimulusOrder::getLabel(" << m_stimNamesAndLabels.at(index.row()) << ") = " << HStimulusOrder::getLabel(m_stimNamesAndLabels.at(index.row()));
			return QVariant(HStimulusOrder::getLabel(m_stimNamesAndLabels.at(index.row())));
		}
	}
	else return QVariant();
}

Qt::ItemFlags GUILib::HStimulusNameLabelTableModel::flags(const QModelIndex &) const
{
	return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant GUILib::HStimulusNameLabelTableModel::headerData(int section, Qt::Orientation, int) const
{
	if (section == 0) return QString("Stimulus");
	else if (section == 1) return QString("Label");
	else return QVariant();
}

int GUILib::HStimulusNameLabelTableModel::rowCount(const QModelIndex &) const
{
	return m_stimNamesAndLabels.length();
}

int GUILib::HStimulusNameLabelTableModel::columnCount(const QModelIndex &) const
{
	return 2;
}



