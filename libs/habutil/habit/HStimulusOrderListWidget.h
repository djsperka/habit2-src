/*
 * HStimulusOrderListWidget.h
 *
 *  Created on: Nov 20, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSORDERLISTWIDGET_H_
#define HSTIMULUSORDERLISTWIDGET_H_

#include "HStimulusOrderListModel.h"
#include "HStimulusSettingsList.h"
#include "stimulusdisplayinfo.h"
#include <QWidget>
#include <QListView>
#include <QPushButton>

namespace GUILib
{
	class HStimulusOrderListWidget: public QWidget
	{
		Q_OBJECT
	private:
		Habit::HStimulusOrderList& m_list;
		const Habit::HStimulusSettingsList& m_ssList;
		const Habit::StimulusDisplayInfo& m_sdi;
		HStimulusOrderListModel *m_pmodel;
		QListView *m_pListView;
		QPushButton *m_pbNew;
		QPushButton *m_pbEdit;
		QPushButton *m_pbRemove;

		void create();
		void connections();
		void editItem(const QModelIndex& index);
	public:
		HStimulusOrderListWidget(Habit::HStimulusOrderList& list, const Habit::HStimulusSettingsList& ssList, const Habit::StimulusDisplayInfo& sdi, QWidget *parent=NULL);
		virtual ~HStimulusOrderListWidget() { delete m_pmodel;};
		void append(const Habit::HStimulusOrderList& list);
		void append(const Habit::HStimulusOrder& order);
		void clobber(const Habit::HStimulusOrder& order);
		void setStimulusLayoutType(int ilayoutType);
		void clearSelection();

	signals:
		void previewOrder(int);
		void currentChanged(const QModelIndex& current, const QModelIndex& previous);
		void clearStimulus();
		void orderSelectionChanged();

	protected slots:
		void editClicked();
		void itemActivated(const QModelIndex& index);
		void newClicked();
		void removeClicked();
		void currentSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
		void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	};
};



#endif /* HSTIMULUSORDERLISTWIDGET_H_ */
