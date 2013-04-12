#ifndef SUBJECTINFORMATIONWIDGET_H
#define SUBJECTINFORMATIONWIDGET_H

#include "subjectsettings.h"
#include <QtGui/QWidget>
#include "ui_subjectinformationwidget.h"

class SubjectInformationWidget : public QWidget
{
	Q_OBJECT

public:
	SubjectInformationWidget(QWidget *parent = 0);
	~SubjectInformationWidget();

	void setConfigurationObject(const Habit::SubjectSettings& settings);
	Habit::SubjectSettings getConfigurationObject();
	bool validate();
protected slots:
	void onClearInfo();
private:
	Habit::SubjectSettings settings_;

private:
	Ui::SubjectInformationWidgetClass ui;
};

#endif // SUBJECTINFORMATIONWIDGET_H
