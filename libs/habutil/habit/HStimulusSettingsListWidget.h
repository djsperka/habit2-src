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
#include "stimulusdisplayinfo.h"
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
		const Habit::StimulusDisplayInfo& m_sdi;
		HStimulusSettingsListModel *m_pmodel;
		QListView *m_pListView;
		QPushButton *m_pbNew;
		QPushButton *m_pbEdit;
		QPushButton *m_pbRemove;

		void create();
		void connections();
		void editItem(const QModelIndex& index);
		const Habit::StimulusDisplayInfo& getStimulusDisplayInfo() const { return	 m_sdi; };
	public:
		HStimulusSettingsListWidget(Habit::HStimulusSettingsList& list, const Habit::StimulusDisplayInfo& sdi, QWidget *parent=NULL);
		virtual ~HStimulusSettingsListWidget() { delete m_pmodel;};
		void append(const Habit::HStimulusSettingsList& list);
		void append(const Habit::HStimulusSettings& ss);
		void clobber(const Habit::HStimulusSettings& ss);
		void clearSelection();
		void setStimulusLayoutType(const HStimulusLayoutType& layoutType);

	Q_SIGNALS:
		void previewStimulus(int);
		void clearStimulus();
		void currentChanged(const QModelIndex& current, const QModelIndex& previous);
		void stimulusSelectionChanged();
		void stimulusAdded(int row);
		void stimulusAboutToBeRemoved(int row);
		void stimulusSettingsChanged(int);

	protected Q_SLOTS:
		void editClicked();
		void itemActivated(const QModelIndex& index);
		void newClicked();
		void previewClicked();
		void removeClicked();
		void currentSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
		void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
		void rowsInserted(const QModelIndex &parent, int first, int last);
		void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
		void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	};

};



#endif /* HSTIMULUSSETTINGSLISTWIDGET_H_ */
