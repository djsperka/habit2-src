/*
 * HExperimentMain.h
 *
 *  Created on: Sep 17, 2014
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTMAIN_H_
#define HEXPERIMENTMAIN_H_

#include "experimentsettings.h"
#include "HPhaseListWidget.h"
#include <QDialog>
#include <QTreeWidget>
#include <QListView>
#include <QStackedWidget>
#include <QVector>
#include <QToolBar>
#include <QAction>

namespace GUILib
{
	class HStimulusDisplayInfoWidget;
	class ControlBarOptionsForm;
	class HLookSettingsWidget;
	class AttentionSetupForm;
	class HPhaseSettingsTabWidget;
	//class HPhaseSettingsWidget;
	//class HabituationSetupForm;
	//class HStimuliSettingsWidget;
	class HHabituationSetupWidget;

	class HExperimentMain : public QDialog
	{
	Q_OBJECT

	public:
		HExperimentMain(const Habit::ExperimentSettings& experimentSettings, QWidget *parent = 0, bool bReadOnly = false);
		virtual ~HExperimentMain() {};

	protected:
		void closeEvent(QCloseEvent *event);

	private slots:
		void currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
		void generalItemClicked(const QModelIndex& index);
		void cancelButtonClicked();
		void saveButtonClicked();
		void exportButtonClicked();

	private:
		void createComponents();
		void makeConnections();
		bool isModified();
		Habit::ExperimentSettings getSettings();	// get settings from current state of all forms
		Habit::StimulusSettings getTestSS();

		Habit::ExperimentSettings m_settings;
		QListView *m_pGeneralListView;
		QTreeWidget *m_pContentsWidget;
		QStackedWidget *m_pPagesWidget;
		//QWidget* m_pBlank;
		GUILib::HStimulusDisplayInfoWidget* m_pStimulusDisplayInfoWidget;
		GUILib::ControlBarOptionsForm* m_pControlBarOptionsForm;
		GUILib::HLookSettingsWidget* m_pLookSettingsWidget;
		GUILib::AttentionSetupForm* m_pAttentionSetupForm;
		QVector<GUILib::HPhaseSettingsTabWidget*> m_pvecPhaseSettingsWidgets;
		QVector<int> m_vecStackPages;
		HPhaseListWidget *m_pPhaseListWidget;

		/*
		GUILib::HPhaseSettingsWidget* m_pPreTestPhaseWidget;
		GUILib::HStimuliSettingsWidget* m_pPreTestStimuliWidget;
		GUILib::HPhaseSettingsWidget* m_pHabituationPhaseWidget;
		GUILib::HStimuliSettingsWidget*  m_pHabituationStimuliWidget;
		GUILib::HHabituationSetupWidget* m_pHabituationSetupWidget;
		GUILib::HPhaseSettingsWidget* m_pTestPhaseWidget;
		GUILib::HStimuliSettingsWidget*  m_pTestStimuliWidget;
		*/
		QPushButton* m_pbCancel;
		QPushButton* m_pbSave;
		QPushButton* m_pbExport;
	};
};



#endif /* HEXPERIMENTMAIN_H_ */
