/*
 * HExperimentSelectionDialog.h
 *
 *  Created on: May 29, 2015
 *      Author: Oakeslab
 */

#ifndef HEXPERIMENTSELECTIONDIALOG_H_
#define HEXPERIMENTSELECTIONDIALOG_H_


#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
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
	QLineEdit *m_plineeditRoot;
	QPushButton *m_pbSelect;

private slots:
	void experimentActivated(QString);
	void selectClicked();

};




#endif /* HEXPERIMENTSELECTIONDIALOG_H_ */
