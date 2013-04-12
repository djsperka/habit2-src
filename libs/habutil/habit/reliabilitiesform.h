#ifndef RELIABILITIESFORM_H
#define RELIABILITIESFORM_H

#include <QtGui/QDialog>
#include "ui_reliabilitiesform.h"

namespace GUILib {

class ReliabilitiesForm : public QDialog
{
	Q_OBJECT

public:
	ReliabilitiesForm(QWidget *parent = 0);
	~ReliabilitiesForm();

public:
	QString getObserver();
	QDate	getDate();
	QString	getComment();

protected slots:
	void onClearInfo();

private:
	Ui::ReliabilitiesFormClass ui;
};
}

#endif // RELIABILITIESFORM_H
