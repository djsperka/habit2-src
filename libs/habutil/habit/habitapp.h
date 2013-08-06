#ifndef HABITAPP_H
#define HABITAPP_H

#include <QMainWindow>
#include "ui_habitapp.h"

class HabitApp : public QMainWindow
{
    Q_OBJECT

public:
    HabitApp(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~HabitApp();

public slots:
    void newExperiment();
    void runSavedExperiment();
	void runExperimentResults();
	void runReliability();

private:
    Ui::HabitAppClass ui;

};

#endif // HABITAPP_H
