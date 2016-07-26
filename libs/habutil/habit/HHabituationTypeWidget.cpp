/*
 * HHabituationTypeWidget.cpp
 *
 *  Created on: Jul 15, 2016
 *      Author: dan
 */

#include "HHabituationTypeWidget.h"
#include <QStringList>

namespace GUILib
{

	HHabituationTypeWidget::HHabituationTypeWidget(const HHabituationType& type, QWidget* w)
	: QWidget(w)
	, ui(new Ui::HHabituationTypeWidget)
	{
		ui->setupUi(this);

		// populate combo box
		QStringList items;
		items << HHabituationType::HHabituationTypeFixedN.label() << HHabituationType::HHabituationTypeCriterion.label() << HHabituationType::HHabituationTypeTotalLookingTime.label();
		ui->comboHabituationType->addItems(items);
		connect(ui->comboHabituationType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(currentIndexChanged(int)));
		ui->comboHabituationType->setCurrentIndex(type.number());
	}

	const HHabituationType& HHabituationTypeWidget::getHabituationType()
	{
		return ::getHabituationType(ui->comboHabituationType->currentIndex());
	}
};



