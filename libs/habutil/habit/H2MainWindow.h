/*
 * H2MainWindow.h
 *
 *  Created on: Sep 5, 2014
 *      Author: Oakeslab
 */

#ifndef H2MAINWINDOW_H_
#define H2MAINWINDOW_H_


#include <QMainWindow>
#include <QMap>
#include <QObject>
#include "experimentsettings.h"
#include "HEventLog.h"

class QAction;
class QMenu;
class QPlainTextEdit;
class QItemSelection;
class QLabel;

class HGMM;
class HLookDetector;
class HStateMachine;
 namespace GUILib
 {
 	 class HExperimentListWidget;
 	 class HRunSettingsDialog;
 	 class HControlPanel;
	 class H2MainWindow : public QMainWindow
	 {
		 Q_OBJECT

	 public:
		 H2MainWindow(bool bDefaultTestRun=true, bool bShowTestingIcon=false, bool bEditTemplates = false, bool bStimInDialog = false);
		 static bool checkExperimentSettings(const Habit::ExperimentSettings& settings, QStringList& sProblems, bool bCheckMonitors = true);

	 protected:
		 void closeEvent(QCloseEvent *event);

		 // Get a default name for an experiment copy.
		 QString getExperimentCopyName(const QString& expt);

		 // Get a default name for an experiment import.
		 QString getExperimentImportName(const QString& expt);

		 // Get a default name for a new experiment.
		 QString getExperimentNewName();

		 // Query user for a valid experiment name.
//		 bool inputExperimentName(QString& newName, const QString defaultName);

	 public Q_SLOTS:
		 void newExperiment();
		 void cloneExperiment();
		 void deleteExperiment();
		 void runExperiment();
		 void importExperiment();
		 void openResultsFile();
		 void showResultsFile(const QString filename);
		 void experimentActivated(QString);
		 void editExperiment();
		 void editPreferences();
		 void testExperiment();
		 void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
		 void workspaceChanged();

	 private:
		 void createActions();
		 void createToolBars();
		 void run(bool bInputTesting);
		 void adaptVideoWidgets(HGMM *pmm);
		 QDialog *createStimulusWidget(HGMM *pmm);

	//    QMap<QString, HExperimentMain> m_mapExperiments;
		 bool m_bTestRunIsDefault;
		 bool m_bShowTestingIcon;
		 bool m_bEditTemplates;
		 bool m_bStimInDialog;
		 //Habit::ExperimentSettings m_experimentSettings;
		 //HGMM *m_pmm;
		 GUILib::HRunSettingsDialog *m_pRunSettingsDialog;
		 HControlPanel *m_pControlPanel;
		 HLookDetector* m_pld;
		 HStateMachine *m_psm;
		 //HEventLog m_eventLog;

		 GUILib::HExperimentListWidget *m_pExperimentListWidget;
		 QToolBar *m_pToolBar;
		 QAction *m_actionEdit;
		 QAction *m_actionNew;
		 QAction *m_actionClone;
		 QAction *m_actionDelete;
		 QAction *m_actionRun;
		 QAction *m_actionResults;
		 QAction *m_actionPreferences;
		 QAction *m_actionImport;
		 QAction *m_actionTesting;
		 QLabel *m_pLabelStatusBar;
	 };
 };


#endif /* H2MAINWINDOW_H_ */
