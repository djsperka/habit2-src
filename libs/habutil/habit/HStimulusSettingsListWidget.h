/*
 * HStimulusSettingsListWidget.h
 *
 *  Created on: Nov 4, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSSETTINGSLISTWIDGET_H_
#define HSTIMULUSSETTINGSLISTWIDGET_H_

#include "stimulisettings.h"
#include "HStimulusSettingsList.h"
#include "HStimulusSettingsListModel.h"
#include <QWidget>
#include <QListView>
#include <QPushButton>

namespace GUILib
{
	class HStimulusSettingsListWidget: public QWidget
	{
		Q_OBJECT
	private:
		Habit::HStimulusSettingsList& m_list;
		const HStimulusLayoutType* m_pLayoutType;
		HStimulusSettingsListModel *m_pmodel;
		QListView *m_pListView;
		QPushButton *m_pbNew;
		QPushButton *m_pbEdit;
		QPushButton *m_pbRemove;

		void create();
		void connections();
		void editItem(const QModelIndex& index);
	public:
		HStimulusSettingsListWidget(Habit::HStimulusSettingsList& list, const HStimulusLayoutType& layoutType = HStimulusLayoutType::HStimulusLayoutSingle, QWidget *parent=NULL);
		virtual ~HStimulusSettingsListWidget() { delete m_pmodel;};
		void setStimulusLayoutType(const HStimulusLayoutType& layoutType);
		void append(const Habit::HStimulusSettingsList& list);
		void append(const Habit::HStimulusSettings& ss);
		void clobber(const Habit::HStimulusSettings& ss);

	protected slots:
		void editClicked();
		void itemActivated(const QModelIndex& index);
		void newClicked();
		void removeClicked();
		void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	};

};



#endif /* HSTIMULUSSETTINGSLISTWIDGET_H_ */
