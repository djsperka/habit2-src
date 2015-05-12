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
class QAction;
class QMenu;
class QPlainTextEdit;
class QItemSelection;

namespace Habit
{
	class ExperimentSettings;
};

 namespace GUILib
 {
 	 class HExperimentListWidget;

	 class H2MainWindow : public QMainWindow
	 {
		 Q_OBJECT

	 public:
		 H2MainWindow(bool bTestRunDefault=true);
		 static bool checkExperimentSettings(const Habit::ExperimentSettings& settings, QStringList& sProblems);

	 protected:
		 void closeEvent(QCloseEvent *event);

		 // Get a default name for an experiment copy.
		 QString getExperimentCopyName(const QString& expt);

		 // Get a default name for an experiment import.
		 QString getExperimentImportName(const QString& expt);

		 // Get a default name for a new experiment.
		 QString getExperimentNewName();

		 // Query user for a valid experiment name.
		 bool inputExperimentName(QString& newName, const QString defaultName);

	 private slots:
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
		 void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

	 private:
		 void createActions();
	//     void createMenus();
		 void createToolBars();
	//     void createStatusBar();

	//    QMap<QString, HExperimentMain> m_mapExperiments;
		 bool m_bTestRunDefault;
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
	 };
 };


#endif /* H2MAINWINDOW_H_ */
