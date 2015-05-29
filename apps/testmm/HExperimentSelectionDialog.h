/*
 * HExperimentSelectionDialog.h
 *
 *  Created on: May 29, 2015
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTSELECTIONDIALOG_H_
#define HEXPERIMENTSELECTIONDIALOG_H_

#include <QtGui/QDialog>
#include "HExperimentListWidget.h"
#include "experimentsettings.h"
#include "HMediaManager.h"

class HExperimentSelectionDialog : public QDialog
{
	Q_OBJECT

public:
	HExperimentSelectionDialog(QWidget* w = 0);
	~HExperimentSelectionDialog() {};

private:
	GUILib::HExperimentListWidget* m_pelw;

private slots:
	void experimentActivated(QString);

signals:
};




#endif /* HEXPERIMENTSELECTIONDIALOG_H_ */
