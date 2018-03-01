/*
 * HExperimentListModel.h
 *
 *  Created on: Nov 12, 2014
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTLISTMODEL_H_
#define HEXPERIMENTLISTMODEL_H_


#include <QStringListModel>
#include <QVector>
#include <QPair>
#include <QColor>

namespace GUILib
{
	class HExperimentListModel: public QStringListModel
	{
		Q_OBJECT
		typedef QPair<QColor, QStringList> CSLPair;
		QList<CSLPair> m_colorProblemPairList;
	public:
		HExperimentListModel(bool bExperiments = true, bool bTemplates = false);
		virtual ~HExperimentListModel() {};
		virtual bool setData (const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
		virtual QVariant data (const QModelIndex & index, int role) const;
		void reload();	// reloads the list of experiment names

	private:
		bool m_bExperiments;
		bool m_bTemplates;
	};
}




#endif /* HEXPERIMENTLISTMODEL_H_ */
