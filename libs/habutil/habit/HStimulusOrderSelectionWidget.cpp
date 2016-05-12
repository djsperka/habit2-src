/*
 * HStimulusOrderSelectionWidget.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: Oakeslab
 */


#include "HStimulusOrderSelectionWidget.h"
#include "ui_HStimulusOrderSelectionForm.h"
#include "HTypes.h"

using namespace GUILib;

HStimulusOrderSelectionWidget::HStimulusOrderSelectionWidget(const Habit::StimuliSettings& s, const QString& phaseName, QWidget *parent)
: QWidget(parent)
, ui(new Ui::HStimulusOrderSelectionForm)
{
	ui->setupUi(this);
	for (unsigned int i=0; i<sizeof(HRandomizationType::A)/sizeof(HRandomizationType*); i++)
		ui->cbxRandomizationType->addItem(HRandomizationType::A[i]->label(), QVariant(HRandomizationType::A[i]->number()));
	connect(ui->rbDefault, SIGNAL(toggled(bool)), this, SLOT(updateStatusLabel()));
	connect(ui->rbSelect, SIGNAL(toggled(bool)), this, SLOT(updateStatusLabel()));
	connect(ui->cbxOrders, SIGNAL(currentIndexChanged(int)), this, SLOT(updateStatusLabel()));
	connect(ui->checkboxRandomize, SIGNAL(toggled(bool)), this, SLOT(updateStatusLabel()));
	connect(ui->cbxRandomizationType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateStatusLabel()));
	initialize(s, phaseName);
}

HStimulusOrderSelectionWidget::~HStimulusOrderSelectionWidget()
{
	delete m_pmodel;
	delete ui;
}


const HRandomizationType& HStimulusOrderSelectionWidget::getRandomizationType()
{
	return ::getRandomizationType(ui->cbxRandomizationType->itemData(ui->cbxRandomizationType->currentIndex()).toInt());
}

bool HStimulusOrderSelectionWidget::isRandomized()
{
	return ui->checkboxRandomize->isChecked();
}

bool HStimulusOrderSelectionWidget::isDefaultOrder()
{
	return ui->rbDefault->isChecked();
}

bool HStimulusOrderSelectionWidget::isDefinedOrder()
{
	return ui->rbSelect->isChecked();
}

QString HStimulusOrderSelectionWidget::getDefinedOrderName()
{
	if (isDefinedOrder())
		return ui->cbxOrders->currentText();
	return QString();
}

void HStimulusOrderSelectionWidget::initialize(const Habit::StimuliSettings& s, const QString& phaseName)
{
	ui->labelMain->setText(QString("%1 Phase Stimulus Order").arg(phaseName));
	m_pmodel = new HStimulusOrderListModel(s.orders(), s.stimuli());
	ui->cbxOrders->setModel(m_pmodel);
	ui->cbxOrders->setEnabled(false);
	ui->checkboxRandomize->setChecked(false);
	ui->cbxRandomizationType->setEnabled(false);

	// If there are no orders, make the select checkbox disabled
	if (s.orders().isEmpty())
		ui->rbSelect->setEnabled(false);

	updateStatusLabel();
}

void HStimulusOrderSelectionWidget::updateStatusLabel()
{
	QString s;
	QString sRandom("No randomization");

	if (ui->checkboxRandomize->isChecked())
	{
		sRandom = ui->cbxRandomizationType->currentText();
	}


	if (ui->rbDefault->isChecked())
	{
		s = QString("Default order selected, %1").arg(sRandom);
		ui->labelStatus->setStyleSheet("QLabel { background-color : lightgreen; }");
	}
	else if (ui->rbSelect->isChecked())
	{
		s = QString("Order \"%1\" selected, %2").arg(ui->cbxOrders->currentText()).arg(sRandom);
		ui->labelStatus->setStyleSheet("QLabel { background-color : lightgreen; }");
	}
	else
	{
		s = QString("No stimulus order selected!");
		ui->labelStatus->setStyleSheet("QLabel { background-color : lightpink; }");
	}
	ui->labelStatus->setText(s);
	emit orderChosen();
}
