/*
 * HExperimentListModel.h
 *
 *  Created on: Nov 12, 2014
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTLISTMODEL_H_
#define HEXPERIMENTLISTMODEL_H_


#include <QStringListModel>

namespace GUILib
{
	class HExperimentListModel: public QStringListModel
	{
		Q_OBJECT
	public:
		HExperimentListModel();
		virtual ~HExperimentListModel() {};
		virtual bool setData (const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
		virtual QVariant data (const QModelIndex & index, int role) const;
		void reload();	// reloads the list of experiment names
	};
}




#endif /* HEXPERIMENTLISTMODEL_H_ */