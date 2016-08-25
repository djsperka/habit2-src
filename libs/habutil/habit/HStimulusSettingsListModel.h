/*
 * HStimulusSettingsListModel.h
 *
 *  Created on: Nov 4, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSSETTINGSLISTMODEL_H_
#define HSTIMULUSSETTINGSLISTMODEL_H_

#include <QAbstractListModel>
#include <QList>
#include "stimulisettings.h"

namespace GUILib
{
	class HStimulusSettingsListModel: public QAbstractListModel
	{
		Q_OBJECT
	private:
		Habit::HStimulusSettingsList& m_list;
		const HStimulusLayoutType* m_pLayoutType;
	public:
		HStimulusSettingsListModel(Habit::HStimulusSettingsList& list, const HStimulusLayoutType& layoutType): m_list(list), m_pLayoutType(&layoutType) {};
		virtual ~HStimulusSettingsListModel() {};
		int rowCount(const QModelIndex& index) const;
		QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		void setLayoutType(const HStimulusLayoutType& layoutType) { m_pLayoutType = &layoutType; };
		void changed(const QModelIndex & topLeft, const QModelIndex & bottomRight);
		void append(const Habit::StimulusSettings& newStim);
		void clobber(const Habit::StimulusSettings& stim);
		void append(const QList<Habit::StimulusSettings>& list);
		void remove(const QModelIndex& index);
		void removeAll();
	};
}



#endif /* HSTIMULUSSETTINGSLISTMODEL_H_ */
