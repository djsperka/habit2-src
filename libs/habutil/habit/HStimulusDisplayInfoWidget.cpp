/*
 * HStimulusDisplayInfoWidget.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: Oakeslab
 */

#include "HStimulusDisplayInfoWidget.h"
#include "ui_HStimulusDisplayInfoForm.h"
#include <QColorDialog>
#include <QtDebug>

using namespace GUILib;

HStimulusDisplayInfoWidget::HStimulusDisplayInfoWidget(const Habit::StimulusDisplayInfo& info, QWidget *parent)
: QWidget(parent)
, ui(new Ui::HStimulusDisplayInfoForm)
, m_info(info)
{
	ui->setupUi(this);
	connect(ui->pbBackgroundColor, SIGNAL(clicked()), this, SLOT(onColorChooserClick()));
	connect(ui->rbSingle, SIGNAL(toggled(bool)), this, SLOT(rbSingleToggled(bool)));
	connect(ui->rbLeftRight, SIGNAL(toggled(bool)), this, SLOT(rbLeftRightToggled(bool)));
	connect(ui->rbTriple, SIGNAL(toggled(bool)), this, SLOT(rbTripleToggled(bool)));
	//	connect(ui->listStimulusLayout, SIGNAL(itemSelectionChanged()), this, SLOT(listItemSelectionChanged()));
	connect(ui->rbFullScreen, SIGNAL(toggled(bool)), ui->cbMaintainAspectRatio, SLOT(setEnabled(bool)));
	initialize();
}

HStimulusDisplayInfoWidget::~HStimulusDisplayInfoWidget()
{
	delete ui;
}

void HStimulusDisplayInfoWidget::rbSingleToggled(bool b)
{
	if (b)
	{
		emit stimulusLayoutTypeChanged(HStimulusLayoutType::HStimulusLayoutSingle.number());
	}
}

void HStimulusDisplayInfoWidget::rbLeftRightToggled(bool b)
{
	if (b)
	{
		emit stimulusLayoutTypeChanged(HStimulusLayoutType::HStimulusLayoutLeftRight.number());
	}
}

void HStimulusDisplayInfoWidget::rbTripleToggled(bool b)
{
	if (b)
	{
		emit stimulusLayoutTypeChanged(HStimulusLayoutType::HStimulusLayoutTriple.number());
	}
}

//void HStimulusDisplayInfoWidget::listItemSelectionChanged()
//{
//	QList<QListWidgetItem *> selectedItems = ui->listStimulusLayout->selectedItems();
//	if (selectedItems.size() == 1)
//	{
//		// have to find which item is selected! This isn't straighforward with a QListWidget...
//		int i = ui->listStimulusLayout->row(selectedItems.at(0));
//		if (i >= 0)
//		{
//			emit stimulusLayoutTypeChanged(i);
//		}
//		else
//			qWarning() << "HStimulusDisplayInfoWidget::listItemSelectionChanged() : selected item not found in list widget!";
//	}
//	return;
//}


void HStimulusDisplayInfoWidget::onColorChooserClick() {
	QColorDialog dlg;
	dlg.setCurrentColor(ui->pbBackgroundColor->palette().color(QPalette::Button));
	dlg.exec();
	QColor c = dlg.currentColor();
	QString st = QString("background-color: rgb(%1, %2, %3)").arg(c.red()).arg(c.green()).arg(c.blue());
	ui->pbBackgroundColor->setStyleSheet(st);
}

void HStimulusDisplayInfoWidget::initialize()
{
	QPalette p = ui->pbBackgroundColor->palette();
	p.setColor(QPalette::Button, m_info.getBackGroundColor());
	ui->pbBackgroundColor->setPalette(p);
	QColor c = ui->pbBackgroundColor->palette().color(QPalette::Button);
	QString st = QString("background-color: rgb(%1, %2, %3)").arg(c.red()).arg(c.green()).arg(c.blue());
	ui->pbBackgroundColor->setStyleSheet(st);

	ui->cbxUseISS->setChecked(m_info.getUseISS());
	if (m_info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
		ui->rbSingle->setChecked(true);
	else if (m_info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
		ui->rbLeftRight->setChecked(true);
	else
		ui->rbTriple->setChecked(true);

	ui->cbMaintainAspectRatio->setChecked(m_info.isOriginalAspectRatioMaintained());
	if (m_info.getDisplayType() == HDisplayType::HDisplayTypeFullScreen)
	{
		ui->rbFullScreen->setChecked(true);
		ui->cbMaintainAspectRatio->setEnabled(true);
	}
	else
	{
		ui->rbOriginalSize->setChecked(true);
		ui->cbMaintainAspectRatio->setEnabled(false);
	}
}

Habit::StimulusDisplayInfo HStimulusDisplayInfoWidget::getStimulusDisplayInfo()
{
	Habit::StimulusDisplayInfo info;
	info.setBackGroundColor(ui->pbBackgroundColor->palette().color(QPalette::Button));
	if (ui->rbSingle->isChecked())
		info.setStimulusLayoutType(HStimulusLayoutType::HStimulusLayoutSingle);
	else if (ui->rbLeftRight->isChecked())
		info.setStimulusLayoutType(HStimulusLayoutType::HStimulusLayoutLeftRight);
	else
		info.setStimulusLayoutType(HStimulusLayoutType::HStimulusLayoutTriple);

	info.setUseISS(ui->cbxUseISS->isChecked());
	info.setMaintainOriginalAspectRatio(ui->cbMaintainAspectRatio->isChecked());
	if (ui->rbFullScreen->isChecked())
	{
		info.setDisplayType(HDisplayType::HDisplayTypeFullScreen);
	}
	else if (ui->rbOriginalSize->isChecked())
	{
		info.setDisplayType(HDisplayType::HDisplayTypeOriginalSize);
	}
	else
	{
		info.setDisplayType(HDisplayType::HDisplayTypeUnknown);
	}
	info.setId(m_info.getId());
	return info;
}

