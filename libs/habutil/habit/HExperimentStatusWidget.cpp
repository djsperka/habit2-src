/*
 * HExperimentStatusWidget.cpp
 *
 *  Created on: Jan 5, 2015
 *      Author: Oakeslab
 */


#include "HExperimentStatusWidget.h"
#include "ui_HExperimentStatusForm.h"
#include <QtDebug>

using namespace GUILib;

HExperimentStatusWidget::HExperimentStatusWidget(const QString& name, bool bDisplayCurrentPhaseTrial, bool bDisplayLookingDirection, QWidget* parent)
: QWidget(parent)
, ui(new Ui::HExperimentStatusForm)
, m_bDisplayCurrentPhaseTrial(bDisplayCurrentPhaseTrial)
, m_bDisplayLookingDirection(bDisplayLookingDirection)
{
	ui->setupUi(this);
	initialize(name);
}

HExperimentStatusWidget::~HExperimentStatusWidget()
{
	qDebug() << "HExperimentStatusWidget::~HExperimentStatusWidget()";
	delete ui;
}

void HExperimentStatusWidget::connections()
{
	qDebug() << "connections";
}


void HExperimentStatusWidget::initialize(const QString& name)
{
	ui->labelExperimentName->setText(name);
}


void HExperimentStatusWidget::setExperimentName(QString name)
{
	ui->labelExperimentName->setText(name);
}

void HExperimentStatusWidget::setStatus(QString status)
{
	ui->labelStatus->setText(status);
}

void HExperimentStatusWidget::setPhase(QString phase)
{
	qDebug() << "setPhase " << m_bDisplayCurrentPhaseTrial;
	if (m_bDisplayCurrentPhaseTrial)
	{
		ui->labelPhase->setText(phase);
	}
	else
	{
		ui->labelPhase->setText(QString("Not displayed"));
	}
}

void HExperimentStatusWidget::setTrial(int trial, int repeat)
{
	if (m_bDisplayCurrentPhaseTrial)
	{
		// DJS 11-2-2015. Display trial number (1, 2, ...), not trial index (0, 1, 2, ...)
		ui->labelTrial->setText(QString("%1 (repeat %2)").arg(trial+1).arg(repeat));
	}
	else
	{
		ui->labelTrial->setText(QString("Not displayed"));
	}
}

void HExperimentStatusWidget::setLooking(QString sLookingAt)
{
	if (m_bDisplayLookingDirection)
	{
		ui->labelLooking->setText(sLookingAt);
	}
	else
	{
		ui->labelLooking->setText(QString("Not displayed"));
	}
}
