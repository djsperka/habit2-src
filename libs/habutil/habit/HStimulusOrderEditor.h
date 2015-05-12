/*
 * HStimulusOrderEditor.h
 *
 *  Created on: Dec 2, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSORDEREDITOR_H_
#define HSTIMULUSORDEREDITOR_H_

#include <QDialog>
#include "stimulussettings.h"
#include "HStimulusOrder.h"
#include "HStimulusSettingsListModel.h"
#include "HStimulusNameListModel.h"
//#include "HStimulusNameLabelTableModel.h"

namespace Ui
{
	class HStimulusOrderEditorForm;
};



namespace GUILib
{

	class HStimulusOrderEditor: public QDialog
	{
		Q_OBJECT

		Ui::HStimulusOrderEditorForm *ui;
		Habit::HStimulusOrder m_originalOrder;
		Habit::HStimulusSettingsList m_stimlist;
		const HStimulusLayoutType& m_layoutType;
		GUILib::HStimulusSettingsListModel* m_pStimListModel;
		HStimulusNameListModel *m_pOrderModel;
		//HStimulusNameLabelTableModel *m_pOrderModel;

		void components();
		void connections();
		void add(const QString& name);

	public:
		HStimulusOrderEditor(const Habit::HStimulusOrder& order, const Habit::HStimulusSettingsList& stimlist, const HStimulusLayoutType& layoutType, QWidget* parent=0);
		virtual ~HStimulusOrderEditor();
		Habit::HStimulusOrder getOrder();	// get the value as currently configured

	signals:

	private slots:
		void addClicked();
		void moveUpClicked();
		void moveDownClicked();
		void removeClicked();
		void doneClicked();
		void itemActivated(const QModelIndex&);
	};
};



#endif /* HSTIMULUSORDEREDITOR_H_ */
