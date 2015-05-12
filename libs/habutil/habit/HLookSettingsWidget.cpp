/*
 * HLookSettingsWidget.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: Oakeslab
 */

#include "HLookSettingsWidget.h"
#include "ui_HLookSettingsForm.h"
using namespace GUILib;
using namespace Habit;


HLookSettingsWidget::~HLookSettingsWidget()
{
	delete ui;
}

HLookSettingsWidget::HLookSettingsWidget()
: QWidget()
, ui(new Ui::HLookSettingsForm)
, m_settings()
{
	setValidators();
	initialize();
}

HLookSettingsWidget::HLookSettingsWidget(const HLookSettings& settings, QWidget* parent)
: QWidget(parent)
, ui(new Ui::HLookSettingsForm)
, m_settings(settings)
{
	ui->setupUi(this);
	setValidators();
	initialize();
}

void HLookSettingsWidget::setHLookSettings(const HLookSettings& settings)
{
	m_settings = settings;
	initialize();
}

const Habit::HLookSettings& HLookSettingsWidget::getHLookSettings()
{
	m_settings.setMinLookTime(ui->lineeditMinimumLookingTime->text().toUInt());
	m_settings.setMinLookAwayTime(ui->lineeditMinimumLookingAwayTime->text().toUInt());
	m_settings.setInclusiveLookTime(ui->cbxInclusiveLookTime->isChecked());
	return m_settings;
}


void HLookSettingsWidget::initialize()
{
	ui->lineeditMinimumLookingTime->setText(QString("%1").arg(m_settings.getMinLookTime()));
	ui->lineeditMinimumLookingAwayTime->setText(QString("%1").arg(m_settings.getMinLookAwayTime()));
	ui->cbxInclusiveLookTime->setChecked(m_settings.getInclusiveLookTime());
}

void HLookSettingsWidget::setValidators()
{
	ui->lineeditMinimumLookingTime->setValidator(new QIntValidator(0, 9999999, this));
	ui->lineeditMinimumLookingAwayTime->setValidator(new QIntValidator(0, 9999999, this));
}

