/*
 * HStimulusNameLabelTableModel.h
 *
 *  Created on: April 21, 2015
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSNAMELABELTABLEMODEL_H_
#define HSTIMULUSNAMELABELTABLEMODEL_H_

#include <QAbstractTableModel>
#include <QStringList>

namespace GUILib
{
	class HStimulusNameLabelTableModel: public QAbstractTableModel
	{
		QStringList m_stimNamesAndLabels;
		QStringList m_allStimNames;
	public:
		HStimulusNameLabelTableModel(const QStringList &namesAndLabels, const QStringList& allStimNames, QObject * parent = 0);
		virtual ~HStimulusNameLabelTableModel() {};

		virtual QVariant data (const QModelIndex & index, int role) const;
		virtual Qt::ItemFlags flags(const QModelIndex & index) const;
		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

		QStringList stringList() const { return m_stimNamesAndLabels; };

	};
}



#endif /* HSTIMULUSNAMELABELTABLEMODEL_H_ */
