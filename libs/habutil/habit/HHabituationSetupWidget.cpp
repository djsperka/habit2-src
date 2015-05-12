/*
 * HHabituationSetupWidget.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: Oakeslab
 */

#include "HHabituationSetupWidget.h"
#include "ui_HHabituationSetupForm.h"

using namespace GUILib;
using namespace Habit;

HHabituationSetupWidget::HHabituationSetupWidget(QWidget* w)
: QWidget(w)
, ui(new Ui::HHabituationSetupForm)
, m_settings()
{
	ui->setupUi(this);
	initializeComponents();
	initialize();
}

HHabituationSetupWidget::HHabituationSetupWidget(const Habit::HabituationSettings& settings, QWidget* w)
: QWidget(w)
, ui(new Ui::HHabituationSetupForm)
, m_settings(settings)
{
	ui->setupUi(this);
	initializeComponents();
	initialize();
}

HHabituationSetupWidget::~HHabituationSetupWidget()
{
	delete ui;
}

void HHabituationSetupWidget::initializeComponents()
{
	// Initialize items in combo boxes...
	QStringList items;
	items << HHabituationType::HHabituationTypeFixedN.label() << HHabituationType::HHabituationTypeCriterion.label() << HHabituationType::HHabituationTypeTotalLookingTime.label();
	ui->comboHabituationType->addItems(items);

	items.clear();
	items << HCriterionWindowType::HCriterionWindowFixed.label() << HCriterionWindowType::HCriterionWindowSliding.label();
	ui->comboWindowType->addItems(items);
}

void HHabituationSetupWidget::initialize()
{
	ui->comboHabituationType->setCurrentIndex(m_settings.getHabituationType().number());
	ui->stackHabituationSettings->setCurrentIndex(m_settings.getHabituationType().number());

	ui->lineeditTotalLookingTime->setValidator(new QIntValidator(1, 999999));
	ui->lineeditTotalLookingTime->setText(QString("%1").arg(m_settings.getTotalLookLengthToEnd()));

	ui->lineeditMinBasisValue->setValidator(new QIntValidator(1, 999999));
	ui->lineeditMinBasisValue->setText(QString("%1").arg(m_settings.getCriterionSettings().getMinBasisValue()));

	ui->rbUseFirstWindow->setChecked(m_settings.getCriterionSettings().getBasis() == HCriterionBasisType::HCriterionBasisFirstN);
	ui->rbUseMaxWindow->setChecked(m_settings.getCriterionSettings().getBasis() == HCriterionBasisType::HCriterionBasisLongestN);
	ui->cbxRequireMinBasisValue->setChecked(m_settings.getCriterionSettings().getRequireMinBasisValue());

	ui->spinPercentage->setValue(m_settings.getCriterionSettings().getPercent());
	ui->cbxExcludeBasisWindow->setChecked(m_settings.getCriterionSettings().getExcludeBasisWindow());
	ui->spinWindowSize->setValue(m_settings.getCriterionSettings().getWindowSize());

	ui->comboWindowType->setCurrentIndex(m_settings.getCriterionSettings().getWindowType().number());
}

Habit::HabituationSettings HHabituationSetupWidget::getHabituationSettings()
{
	m_settings.setHabituationType(getHabituationType(ui->comboHabituationType->currentIndex()));
	m_settings.setTotalLookLengthToEnd(ui->lineeditTotalLookingTime->text().toInt());

	Habit::CriterionSettings criteria;
	criteria.setBasis(ui->rbUseFirstWindow->isChecked() ? HCriterionBasisType::HCriterionBasisFirstN : HCriterionBasisType::HCriterionBasisLongestN);
	criteria.setExcludeBasisWindow(ui->cbxExcludeBasisWindow->isChecked());
	criteria.setMinBasisValue(ui->lineeditMinBasisValue->text().toInt());
	criteria.setPercent(ui->spinPercentage->value());
	criteria.setRequireMinBasisValue(ui->cbxRequireMinBasisValue->isChecked());
	criteria.setWindowSize(ui->spinWindowSize->value());
	criteria.setWindowType(getCriterionWindowType(ui->comboWindowType->currentIndex()));
	m_settings.setCriterionSettings(criteria);
	return m_settings;
}


void HHabituationSetupWidget::setHabituationSettings(const Habit::HabituationSettings& s)
{
	m_settings = s;
	initialize();
}

