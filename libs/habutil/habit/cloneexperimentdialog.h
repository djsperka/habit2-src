#ifndef CLONEEXPERIMENTDIALOG_H
#define CLONEEXPERIMENTDIALOG_H

#include <QtGui/QDialog>
#include "ui_cloneexperimentdialog.h"

namespace GUILib {

class CloneExperimentDialog : public QDialog
{
	Q_OBJECT

public:
	CloneExperimentDialog(QWidget *parent = 0);
	~CloneExperimentDialog();

	QString	getExperimentName();
private:
	Ui::CloneExperimentDialogClass ui;
};

}

#endif // CLONEEXPERIMENTDIALOG_H
