/*
 * ResultsExplorerDialog.h
 *
 *  Created on: Jul 30, 2021
 *      Author: dan
 */

#ifndef HABIT_RESULTSSCANNERDIALOG_H_
#define HABIT_RESULTSSCANNERDIALOG_H_

#include <QDialog>

namespace Ui
{
	class ResultsScannerForm;
};

namespace GUILib {

class ResultsScannerDialog: public QDialog {
	Ui::ResultsScannerForm *ui;

public:
	ResultsScannerDialog(QWidget *parent, const QString& text);
	virtual ~ResultsScannerDialog();
};

} /* namespace GUILib */

#endif /* HABIT_RESULTSSCANNERDIALOG_H_ */


