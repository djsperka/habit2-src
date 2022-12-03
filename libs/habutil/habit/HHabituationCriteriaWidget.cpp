/*
 * HHabituationTypeWidget.cpp
 *
 *  Created on: Jul 15, 2016
 *      Author: dan
 */

#include "HHabituationCriteriaWidget.h"
#include "ui_HHabituationCriteriaForm.h"
#include "HTypes.h"
#include <QStringList>
#include <QIntValidator>
#include <QStringList>
#include <QtDebug>

GUILib::HHabituationCriteriaWidget::HHabituationCriteriaWidget(int ntrials, const Habit::CriterionSettings& cs, int totalLookingTime, QWidget* w)
: QStackedWidget(w)
, ui(new Ui::HHabituationCriteriaForm)
{
	ui->setupUi(this);

	// pageNTrials
	ui->spinBoxNTrials->setValue(ntrials);

	// pageCriterion
	ui->spinBoxMaxNumberOfHabituationTrials->setValue(ntrials);
	if (cs.getBasis() == HCriterionBasisType::HCriterionBasisLongestN) ui->rbUseMaxWindow->setChecked(true);
	else ui->rbUseFirstWindow->setChecked(true);
	ui->cbxRequireMinBasisValue->setChecked(cs.getRequireMinBasisValue());
	ui->lineeditMinBasisValue->setText(QString("%1").arg(cs.getMinBasisValue()));
	ui->lineeditMinBasisValue->setValidator(new QIntValidator(1, 1000000));
	ui->spinPercentage->setValue(cs.getPercent());
	ui->cbxExcludeBasisWindow->setChecked(cs.getExcludeBasisWindow());
	ui->spinWindowSize->setValue(cs.getWindowSize());


	// pageTotalLookingTime
	ui->spinBoxMaximumNumberOfTotalLookingTrials->setValue(ntrials);
	ui->lineeditTotalLookingTime->setValidator(new QIntValidator(1, 1000000));
	ui->lineeditTotalLookingTime->setText(QString("%1").arg(totalLookingTime));

	// populate window type combo box
	QStringList items;
	items << HCriterionWindowType::HCriterionWindowFixed.label() << HCriterionWindowType::HCriterionWindowSliding.label();
	ui->comboWindowType->addItems(items);
	ui->comboWindowType->setCurrentIndex(cs.getWindowType().number());

	// some connections
	connect(ui->cbxRequireMinBasisValue, SIGNAL(clicked(bool)), ui->lineeditMinBasisValue, SLOT(setEnabled(bool)));

	// the NTrials spin boxes should be kept in sync. The widgets will re-emit valueChanged, but only if the value is
	// different. Hopefully that means this will not lead to infinite calls...
	connect(ui->spinBoxNTrials,								SIGNAL(valueChanged(int)),	ui->spinBoxMaximumNumberOfTotalLookingTrials, 	SLOT(setValue(int)));
	connect(ui->spinBoxNTrials, 								SIGNAL(valueChanged(int)), 	ui->spinBoxMaxNumberOfHabituationTrials, 		SLOT(setValue(int)));

	connect(ui->spinBoxMaximumNumberOfTotalLookingTrials,		SIGNAL(valueChanged(int)), 	ui->spinBoxNTrials, 								SLOT(setValue(int)));
	connect(ui->spinBoxMaximumNumberOfTotalLookingTrials,		SIGNAL(valueChanged(int)), 	ui->spinBoxMaxNumberOfHabituationTrials, 		SLOT(setValue(int)));

	connect(ui->spinBoxMaxNumberOfHabituationTrials,			SIGNAL(valueChanged(int)), 	ui->spinBoxMaximumNumberOfTotalLookingTrials, 	SLOT(setValue(int)));
	connect(ui->spinBoxMaxNumberOfHabituationTrials,			SIGNAL(valueChanged(int)), 	ui->spinBoxNTrials, 								SLOT(setValue(int)));

}

// this assumes that the above connections for the spinboxes work....
int GUILib::HHabituationCriteriaWidget::getNTrials()
{
//	qDebug() << "GUILib::HabituationCriteriaWidget::getNTrials() " << ui->spinBoxNTrials->value();
//	qDebug() << "GUILib::HabituationCriteriaWidget::getNTrials() currentIndex " << currentIndex();
	return ui->spinBoxNTrials->value();
}

int GUILib::HHabituationCriteriaWidget::getTotalLookLength()
{
	return ui->lineeditTotalLookingTime->text().toInt();
}

const Habit::CriterionSettings GUILib::HHabituationCriteriaWidget::getCriterionSettings()
{
	Habit::CriterionSettings cs;
	if (ui->rbUseMaxWindow->isChecked()) cs.setBasis(HCriterionBasisType::HCriterionBasisLongestN);
	if (ui->rbUseFirstWindow->isChecked()) cs.setBasis(HCriterionBasisType::HCriterionBasisFirstN);
	cs.setPercent(ui->spinPercentage->value());
	cs.setWindowSize(ui->spinWindowSize->value());
	cs.setWindowType(::getCriterionWindowType(ui->comboWindowType->currentIndex()));
	cs.setExcludeBasisWindow(ui->cbxExcludeBasisWindow->isChecked());
	cs.setRequireMinBasisValue(ui->cbxRequireMinBasisValue->isChecked());
	cs.setMinBasisValue(ui->lineeditMinBasisValue->text().toInt());
	return cs;
}



