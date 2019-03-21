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

void HMediaStatusWidget::setStatusLabels(const Habit::StimulusSettings& settings, const QString& stimOrAG)
{
	if (m_layout == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		if (m_bDisplayStimulusInfo)
		{
			ui->label1->setText(QString("Center: (%1) %2").arg(stimOrAG).arg(settings.getCenterStimulusInfo().label()));
			ui->label2->setText(QString("Audio: (%1) %2").arg(stimOrAG).arg(settings.getIndependentSoundInfo().label()));
			ui->label3->setHidden(true);
			ui->label4->setHidden(true);
		}
		else
		{
			ui->label1->setText(QString("Center: (%1) Not displayed").arg(stimOrAG));
			ui->label2->setText(QString("Audio: (%1) Not displayed").arg(stimOrAG));
			ui->label3->setHidden(true);
			ui->label4->setHidden(true);
		}
	}
	else if (m_layout == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		if (m_bDisplayStimulusInfo)
		{
			ui->label1->setText(QString("Left: (%1) %2").arg(stimOrAG).arg(settings.getLeftStimulusInfo().label()));
			ui->label2->setText(QString("Right: (%1) %2").arg(stimOrAG).arg(settings.getRightStimulusInfo().label()));
			ui->label3->setText(QString("Audio: (%1) %2").arg(stimOrAG).arg(settings.getIndependentSoundInfo().label()));
			ui->label4->setHidden(true);
		}
		else
		{
			ui->label1->setText(QString("Left: (%1) Not displayed").arg(stimOrAG));
			ui->label2->setText(QString("Right: (%1) Not displayed").arg(stimOrAG));
			ui->label3->setText(QString("Audio: (%1) Not displayed").arg(stimOrAG));
			ui->label4->setHidden(true);
		}
	}
	else if (m_layout == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		if (m_bDisplayStimulusInfo)
		{
			ui->label1->setText(QString("Left: (%1) %2").arg(stimOrAG).arg(settings.getLeftStimulusInfo().label()));
			ui->label2->setText(QString("Center: (%1) %2").arg(stimOrAG).arg(settings.getCenterStimulusInfo().label()));
			ui->label3->setText(QString("Right: (%1) %2").arg(stimOrAG).arg(settings.getRightStimulusInfo().label()));
			ui->label4->setText(QString("Audio: (%1) %2").arg(stimOrAG).arg(settings.getIndependentSoundInfo().label()));
		}
		else
		{
			ui->label1->setText(QString("Left: (%1) Not displayed").arg(stimOrAG));
			ui->label2->setText(QString("Center: (%1) Not displayed").arg(stimOrAG));
			ui->label3->setText(QString("Right: (%1) Not displayed").arg(stimOrAG));
			ui->label4->setText(QString("Audio: (%1) Not displayed").arg(stimOrAG));
		}
	}
}



void HMediaStatusWidget::setStatusBackground()
{
	if (m_layout == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		ui->label1->setText(QString("Center: idle"));
		ui->label2->setText(QString("Audio: idle"));
		ui->label3->setHidden(true);
		ui->label4->setHidden(true);
	}
	else if (m_layout == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		ui->label1->setText(QString("Left: idle"));
		ui->label2->setText(QString("Right: idle"));
		ui->label3->setText(QString("Audio: idle"));
		ui->label4->setHidden(true);
	}
	else if (m_layout == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		ui->label1->setText(QString("Left: idle"));
		ui->label2->setText(QString("Center: idle"));
		ui->label3->setText(QString("Right: idle"));
		ui->label4->setText(QString("Audio: idle"));
	}
	ui->statusStack->setCurrentIndex(0);
}



void HMediaStatusWidget::setStatusAG(const Habit::StimulusSettings& settings)
{
	// update labels
	setStatusLabels(settings, QString("AG"));

	// display the resource icon for attention in the contained widget
	ui->statusStack->setCurrentIndex(1);
}

void HMediaStatusWidget::setStatusStim(const Habit::StimulusSettings& settings)
{
	// update labels
	setStatusLabels(settings, QString("Stim"));

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

