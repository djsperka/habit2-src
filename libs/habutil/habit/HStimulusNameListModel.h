/*
 * HStimulusNameListModel.h
 *
 *  Created on: Dec 5, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSNAMELISTMODEL_H_
#define HSTIMULUSNAMELISTMODEL_H_

#include <QStringListModel>

namespace GUILib
{
	class HStimulusNameListModel: public QStringListModel
	{
		QStringList m_allStimNames;
	public:
		HStimulusNameListModel(const QStringList &names, const QStringList& allStimNames, QObject * parent = 0);
		virtual ~HStimulusNameListModel() {};
		virtual QVariant data (const QModelIndex & index, int role) const;
	};
}



#endif /* HSTIMULUSNAMELISTMODEL_H_ */
