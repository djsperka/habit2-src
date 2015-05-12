/*
 * HStimulusNameListModel.cpp
 *
 *  Created on: Dec 5, 2014
 *      Author: Oakeslab
 */

#include "HStimulusNameListModel.h"
#include "HStimulusOrder.h"
#include <QBrush>

using namespace GUILib;
using namespace Habit;

HStimulusNameListModel::HStimulusNameListModel(const QStringList &names, const QStringList& allStimNames, QObject * parent)
: QStringListModel(names, parent)
, m_allStimNames(allStimNames)
{
};

QVariant HStimulusNameListModel::data(const QModelIndex & index, int role) const
{
	if (role != Qt::BackgroundRole) return QStringListModel::data(index, role);
	else
	{
		if (!m_allStimNames.contains(HStimulusOrder::getStim(stringList().at(index.row()))))
			return QBrush(Qt::yellow);
	}
	return QVariant();
}



