/*
 *  HApp.h
 *  habutil
 *
 *  Created by Oakes Lab on 9/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HAPP_H
#define HAPP_H

#include <QtGui/QMainWindow>
#include "ui_habitapp.h"

namespace Habit
{
	class ExperimentSettings;
	class RunSettings;
	class SubjectSettings;
};

class HEventLog;
class QMenu;
class QAction;

class HMainWindow : public QMainWindow
{
    Q_OBJECT
	
public:
    HMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~HMainWindow() {};
	
	public slots:
    void newExperiment();
    void runSavedExperiment();
	void runExperimentResults();
	void runReliability();
	void showResultsFile(QString filename);
	
private slots:
	void openDBFile();

private:
	void createMenus();
	void createActions();
    Ui::HabitAppClass ui;
    QMenu *m_fileMenu;
    QAction *m_openDBAct;
};

#endif // HABITAPP_H
