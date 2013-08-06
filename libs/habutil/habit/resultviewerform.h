#ifndef RESULTVIEWERFORM_H
#define RESULTVIEWERFORM_H

#include <QWidget>
#include <QString>
#include "ui_resultviewerform.h"

class ResultViewerForm : public QDialog
{
	Q_OBJECT

public:
	ResultViewerForm(QWidget *parent = 0);
	~ResultViewerForm();

	void setLogText(const QStringList& logText);

private slots:
	void onSavePressed();
	void onPrintPressed();

private:
	Ui::ResultViewerFormClass ui;
};

#endif // RESULTVIEWERFORM_H
