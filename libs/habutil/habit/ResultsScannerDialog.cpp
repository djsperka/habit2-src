/*
 * ResultsExplorerDialog.cpp
 *
 *  Created on: Jul 30, 2021
 *      Author: dan
 */

#include <ResultsScannerDialog.h>
#include "ui_ResultsScannerForm.h"

namespace GUILib {

ResultsScannerDialog::ResultsScannerDialog(QWidget *parent, const QString& text)
: QDialog(parent)
, ui(new Ui::ResultsScannerForm())
{
	ui->setupUi(this);
	ui->textEditScannerOutput->setText(text);
}

ResultsScannerDialog::~ResultsScannerDialog() {
	delete ui;
}

} /* namespace GUILib */
