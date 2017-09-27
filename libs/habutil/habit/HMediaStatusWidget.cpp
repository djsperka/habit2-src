/*
 * HMediaStatusWidget.cpp
 *
 *  Created on: Dec 29, 2014
 *      Author: Oakeslab
 */

#include "HMediaStatusWidget.h"
#include "ui_HMediaStatusForm.h"
#include <QtDebug>

using namespace GUILib;

HMediaStatusWidget::HMediaStatusWidget(const HStimulusLayoutType& layout, bool bDisplayStimulusInfo, QWidget* parent)
: QWidget(parent)
, m_pMediaManager(0)
, m_layout(layout)
, m_bDisplayStimulusInfo(bDisplayStimulusInfo)
, ui(new Ui::HMediaStatusForm)
{
	ui->setupUi(this);
	initialize();
//	connections();
}

HMediaStatusWidget::~HMediaStatusWidget()
{
	delete ui;
}

void HMediaStatusWidget::connections()
{
	connect(m_pMediaManager, SIGNAL(agStarted(int)), this, SLOT(onAGStarted(int)));
	connect(m_pMediaManager, SIGNAL(stimStarted(int)), this, SLOT(onStimStarted(int)));
	connect(m_pMediaManager, SIGNAL(cleared()), this, SLOT(onCleared()));
}

void HMediaStatusWidget::setMediaManager(const HGMM* pmm)
{
	m_pMediaManager = pmm;
	connections();
}

void HMediaStatusWidget::initialize()
{
	int i;

	// add pages to stacked widget
	QWidget *pWidget0 = new QWidget();
	QPalette p = palette();
	p.setColor(backgroundRole(), Qt::gray);
	pWidget0->setPalette(p);
	pWidget0->setAutoFillBackground(true);
	i=ui->statusStack->addWidget(pWidget0);

	QLabel *pWidget1 = new QLabel();
	QPixmap pixmap1;
	pixmap1.load(":/resources/attention.png");
	pWidget1->setPixmap(pixmap1);
	i=ui->statusStack->addWidget(pWidget1);

	QLabel *pWidget2 = new QLabel();
	QPixmap pixmap2;
	pixmap2.load(":/resources/stim-now-playing.png");
	pWidget2->setPixmap(pixmap2);
	i = ui->statusStack->addWidget(pWidget2);

	setStatusBackground();
}

void HMediaStatusWidget::setStatusBackground()
{
	if (m_layout == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		ui->label1->setText(QString("C: idle"));
		ui->label2->setText(QString("S: idle"));
		ui->label3->setHidden(true);
	}
	else if (m_layout == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		ui->label1->setText(QString("L: idle"));
		ui->label2->setText(QString("R: idle"));
		ui->label2->setText(QString("S: idle"));
	}
	ui->statusStack->setCurrentIndex(0);
}

void HMediaStatusWidget::setStatusAG(const Habit::StimulusSettings& settings)
{
	if (m_layout == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		if (m_bDisplayStimulusInfo)
		{
			ui->label1->setText(QString("C: (AG) %1").arg(settings.getCenterStimulusInfo().label()));
			ui->label2->setText(QString("S: (AG) %1").arg(settings.getIndependentSoundInfo().label()));
			ui->label3->setHidden(true);
		}
		else
		{
			ui->label1->setText(QString("C: (AG) Attention Getter"));
			ui->label2->setText(QString("S: (AG) Attention Getter"));
			ui->label3->setHidden(true);
		}
	}
	else if (m_layout == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		if (m_bDisplayStimulusInfo)
		{
			ui->label1->setText(QString("L: (AG) %1").arg(settings.getLeftStimulusInfo().label()));
			ui->label2->setText(QString("R: (AG) %1").arg(settings.getRightStimulusInfo().label()));
			ui->label3->setText(QString("S: (AG) %1").arg(settings.getIndependentSoundInfo().label()));
		}
		else
		{
			ui->label1->setText(QString("L: (AG) Attention Getter"));
			ui->label2->setText(QString("R: (AG) Attention Getter"));
			ui->label3->setText(QString("S: (AG) Attention Getter"));
		}
	}
	// display the resource icon for attention in the contained widget
	ui->statusStack->setCurrentIndex(1);
}

void HMediaStatusWidget::setStatusStim(const Habit::StimulusSettings& settings)
{
	if (m_layout == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		if (m_bDisplayStimulusInfo)
		{
			ui->label1->setText(QString("C: %1").arg(settings.getCenterStimulusInfo().label()));
			ui->label2->setText(QString("S: %1").arg(settings.getIndependentSoundInfo().label()));
			ui->label3->setHidden(true);
		}
		else
		{
			ui->label1->setText(QString("C: Stimulus info not displayed"));
			ui->label2->setText(QString("S: Stimulus info not displayed"));
			ui->label3->setHidden(true);
		}
	}
	else if (m_layout == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		if (m_bDisplayStimulusInfo)
		{
			ui->label1->setText(QString("L: %1").arg(settings.getLeftStimulusInfo().label()));
			ui->label2->setText(QString("R: %1").arg(settings.getRightStimulusInfo().label()));
			ui->label3->setText(QString("S: %1").arg(settings.getIndependentSoundInfo().label()));
		}
		else
		{
			ui->label1->setText(QString("L: Stimulus info not displayed"));
			ui->label2->setText(QString("R: Stimulus info not displayed"));
			ui->label3->setText(QString("S: Stimulus info not displayed"));
		}
	}

	// display the resource icon for stim in the contained widget
	ui->statusStack->setCurrentIndex(2);
}


void HMediaStatusWidget::onAGStarted(int key)
{
	Habit::StimulusSettings settings;
	if (m_pMediaManager) settings = m_pMediaManager->getStimulusSettings(key);
	setStatusAG(settings);
}

void HMediaStatusWidget::onStimStarted(int key)
{
	Habit::StimulusSettings settings;
	if (m_pMediaManager) settings = m_pMediaManager->getStimulusSettings(key);
	setStatusStim(settings);
}

void HMediaStatusWidget::onCleared()
{
	setStatusBackground();
}
