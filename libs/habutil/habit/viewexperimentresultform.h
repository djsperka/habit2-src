#ifndef VIEWEXPERIMENTRESULTFORM_H
#define VIEWEXPERIMENTRESULTFORM_H

#include <QtGui/QWidget>
#include "ui_viewexperimentresultform.h"

class ViewExperimentResultForm : public QDialog
{
	Q_OBJECT

public:
	ViewExperimentResultForm(QWidget *parent = 0);
	~ViewExperimentResultForm();

private slots:
	void onViewExperimentResults();
	void onExportMultipleResults();
private:
	Ui::ViewExperimentResultFormClass ui;
};

#endif // VIEWEXPERIMENTRESULTFORM_H
