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
	
private:
    Ui::HabitAppClass ui;
};

#endif // HABITAPP_H