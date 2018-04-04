/*
 * HRunSettingsTestingWidget.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: dan
 */

#include "HRunSettingsTestingWidget.h"
#include "ui_HRunSettingsTestingForm.h"

GUILib::HRunSettingsTestingWidget::HRunSettingsTestingWidget(QWidget *parent)
: QWidget(parent)
, ui(new Ui::HRunSettingsTestingForm)
{
	ui->setupUi(this);
}

GUILib::HRunSettingsTestingWidget::~HRunSettingsTestingWidget()
{
	delete ui;
}

bool GUILib::HRunSettingsTestingWidget::isDisplayStimInWindow()
{
	return ui->cbTesting->isChecked();
}

