/*
 * ResultsExplorerDialog.cpp
 *
 *  Created on: Jul 30, 2021
 *      Author: dan
 */

#include <ResultsScannerDialog.h>
#include "ui_ResultsScannerForm.h"

namespace GUILib {

ResultsScannerDialog::ResultsScannerDialog(QWidget *parent, const QString& html)
: QDialog(parent)
, ui(new Ui::ResultsScannerForm())
{
	ui->setupUi(this);
	ui->textEditScannerOutput->setHtml(html);
}

ResultsScannerDialog::~ResultsScannerDialog() {
	delete ui;
}

} /* namespace GUILib */
