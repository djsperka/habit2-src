/*
 * HAboutHabitDialog.cpp
 *
 *  Created on: Apr 26, 2018
 *      Author: dan
 */

#include "HAboutHabitDialog.h"
#include "ui_AboutHabitDialog.h"

namespace GUILib {

HAboutHabitDialog::HAboutHabitDialog(QWidget *parent)
: QDialog(parent)
, ui(new Ui::AboutHabitDialog)
{
	ui->setupUi(this);
	ui->textBrowser->setSource(QUrl("qrc:/resources/doc/about.html"));


//	QPushButton *pb = new QPushButton("View license");
//	ui->buttonBox->addButton(pb, QDialogButtonBox::ActionRole);
//	connect(pb, SIGNAL(clicked()), this, SLOT(viewLicenseClicked()));
}

HAboutHabitDialog::~HAboutHabitDialog()
{
}


} /* namespace GUILib */
