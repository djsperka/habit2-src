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
		virtual ~HExperimentMain();

	protected:
		void closeEvent(QCloseEvent *event);

	private slots:

		void generalListViewItemClicked(const QModelIndex& index);
		void cancelButtonClicked();
		void saveButtonClicked();
		void exportButtonClicked();
		void phaseListViewItemClicked(const QModelIndex&);
		void addPhase();
		void delPhase();
		void upPhase();
		void downPhase();

	signals:
		void stimulusDisplayInfoChanged(const Habit::StimulusDisplayInfo&);

	private:
		void components(bool bReadOnly = false);
		void connections();
		bool isModified();
		Habit::ExperimentSettings getSettings();	// get settings from current state of all forms
		Habit::StimulusSettings getTestSS();
		void checkStimulusDisplayInfo();			// check if sdi has changed, and if so notify preview widgets or their owners
		Habit::ExperimentSettings m_settings;
		QListView *m_pGeneralListView;
		QTreeWidget *m_pContentsWidget;
		QStackedWidget *m_pPagesWidget;
		int m_sdiPageIndex;
		Habit::StimulusDisplayInfo m_oldStimulusDisplayInfo;	// keep latest value each time page changes

		GUILib::HStimulusDisplayInfoWidget* m_pStimulusDisplayInfoWidget;
		GUILib::ControlBarOptionsForm* m_pControlBarOptionsForm;
		GUILib::HLookSettingsWidget* m_pLookSettingsWidget;
		GUILib::AttentionSetupForm* m_pAttentionSetupForm;

		QVector<int> m_vecStackPages;
		HPhaseListWidget *m_pPhaseListWidget;

		QPushButton* m_pbCancel;
		QPushButton* m_pbSave;
		QPushButton* m_pbExport;
	};
};



#endif /* HEXPERIMENTMAIN_H_ */
