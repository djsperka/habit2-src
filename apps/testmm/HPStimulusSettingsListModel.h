/*
 * HPStimulusSettingsListModel.h
 *
 *  Created on: Jun 2, 2015
 *      Author: Oakeslab
 */

#ifndef HPSTIMULUSSETTINGSLISTMODEL_H_
#define HPSTIMULUSSETTINGSLISTMODEL_H_

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
#include <QPair>
#include "stimulisettings.h"

namespace GUILib
{
	class HPStimulusSettingsListModel: public QAbstractListModel
	{
		Q_OBJECT
	private:
		QList< QPair<unsigned int, const Habit::StimulusSettings* > > m_list;
	public:
		HPStimulusSettingsListModel(const QMap<unsigned int, const Habit::StimulusSettings *>& map);
		virtual ~HPStimulusSettingsListModel() {};
		int rowCount(const QModelIndex& index) const;
		QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

		unsigned int getStimulusKey(int row);


		void append(const Habit::StimulusSettings& newStim);
		void append(const Habit::StimulusSettingsList& list);

/*
		void changed(const QModelIndex & topLeft, const QModelIndex & bottomRight);
		void append(const Habit::StimulusSettings& newStim);
		void clobber(const Habit::StimulusSettings& stim);
		void append(const QList<Habit::StimulusSettings>& list);
		void remove(const QModelIndex& index);
 */
	};
}



#endif /* HSTIMULUSSETTINGSLISTMODEL_H_ */




#endif /* HPSTIMULUSSETTINGSLISTMODEL_H_ */
