/*
 * HStimulusListWidget.cpp
 *
 *  Created on: Oct 9, 2014
 *      Author: Oakeslab
 */


#include "HStimulusSettingsWidget.h"
#include "HStimulusOrder.h"
#include "HTypes.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegExpValidator>

GUILib::HStimulusSettingsWidget::HStimulusSettingsWidget(const Habit::StimulusSettings& settings, const HStimulusLayoutType& stimLayout, QWidget *parent)
: QWidget(parent)
, m_settings(settings)
, m_pStimulusLayout(&stimLayout)
{
	// create HStimulusInfoWidget for each possible stimulus.
	m_pLeft = new HStimulusInfoWidget(m_settings.getLeftStimulusInfo(), QString("L:"), this);
	m_pCenter = new HStimulusInfoWidget(m_settings.getCenterStimulusInfo(), QString("C:"), this);
	m_pRight = new HStimulusInfoWidget(m_settings.getRightStimulusInfo(), QString("R:"), this);
	m_pSound = new HStimulusInfoWidget(m_settings.getIndependentSoundInfo(), QString("S:"), this);

	// label
	QLabel *pNameLabel = new QLabel("Stimulus name:");
	m_pName = new QLineEdit(m_settings.getName(), this);
	m_pName->setValidator(new QRegExpValidator(Habit::HStimulusOrder::getStimlusNameLabelRE()));

	// stack
	m_pStack = new QStackedWidget(this);


	// Construct the pages in the stack. Sound not added until initialize() is called.
	// first page of stack is for single stim
	QWidget *pwSingle = new QWidget();
	QVBoxLayout *vSingle = new QVBoxLayout();
	vSingle->addWidget(m_pCenter);
	//vSingle->addWidget(m_pISSSingle);

	//vSingle->addStretch(1);
	pwSingle->setLayout(vSingle);
	m_stackidSingle = m_pStack->addWidget(pwSingle);

	// second page is for left/right and iss
	QWidget *pwLR = new QWidget();
	QVBoxLayout *vLR = new QVBoxLayout();
	vLR->addWidget(m_pLeft);
	vLR->addWidget(m_pRight);
	//vLR->addWidget(m_pISSLR);
	//vLR->addStretch(1);
	pwLR->setLayout(vLR);
	m_stackidLR = m_pStack->addWidget(pwLR);

	// name and label
	QHBoxLayout *hname = new QHBoxLayout();
	hname->addWidget(pNameLabel);
	hname->addWidget(m_pName);

	// TODO: m_pName needs a validator

	// now put the label and the stack side-by-side
	QHBoxLayout *h = new QHBoxLayout();
	h->addLayout(hname, 0);
	h->addWidget(m_pStack, 1);

	// Now put all the stuff into a vbox
	QVBoxLayout *v = new QVBoxLayout();
	v->addLayout(h);
//	v->addStretch(1);
	setLayout(v);

	// squish it from the top
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

	setStimulusLayoutType(*m_pStimulusLayout);

	connections();
}

GUILib::HStimulusSettingsWidget::~HStimulusSettingsWidget() {};


void GUILib::HStimulusSettingsWidget::connections()
{
	connect(m_pLeft, SIGNAL(stimulusInfoChanged()), this, SIGNAL(stimulusSettingsChanged()));
	connect(m_pCenter, SIGNAL(stimulusInfoChanged()), this, SIGNAL(stimulusSettingsChanged()));
	connect(m_pRight, SIGNAL(stimulusInfoChanged()), this, SIGNAL(stimulusSettingsChanged()));
	connect(m_pSound, SIGNAL(stimulusInfoChanged()), this, SIGNAL(stimulusSettingsChanged()));
};


void GUILib::HStimulusSettingsWidget::setStimulusLayoutType(const HStimulusLayoutType& type)
{
	m_pStimulusLayout = &type;

	// make sure sound widget is not in either page in the stack
	m_pStack->widget(m_stackidSingle)->layout()->removeWidget(m_pSound);
	m_pStack->widget(m_stackidLR)->layout()->removeWidget(m_pSound);
	if (*m_pStimulusLayout == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		m_pStack->widget(m_stackidSingle)->layout()->addWidget(m_pSound);
		m_pStack->setCurrentIndex((m_stackidSingle));
	}
	else if (*m_pStimulusLayout == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		m_pStack->widget(m_stackidLR)->layout()->addWidget(m_pSound);
		m_pStack->setCurrentIndex((m_stackidLR));
	}
	else
	{
		//default is single stim
		m_pStack->widget(m_stackidSingle)->layout()->addWidget(m_pSound);
		m_pStack->setCurrentIndex((m_stackidSingle));
	}
}

void GUILib::HStimulusSettingsWidget::initialize()
{
	m_pCenter->setStimulusInfo(m_settings.getCenterStimulusInfo());
	m_pLeft->setStimulusInfo(m_settings.getLeftStimulusInfo());
	m_pRight->setStimulusInfo(m_settings.getRightStimulusInfo());
	m_pSound->setStimulusInfo(m_settings.getIndependentSoundInfo());
	m_pName->setText(m_settings.getName());
}


Habit::StimulusSettings GUILib::HStimulusSettingsWidget::getStimulusSettings()
{
	Habit::StimulusSettings settings(m_settings);
	settings.setLeftStimulusInfo(m_pLeft->getStimulusInfo());
	settings.setCenterStimulusInfo(m_pCenter->getStimulusInfo());
	settings.setRightStimulusInfo(m_pRight->getStimulusInfo());
	settings.setIndependentSoundInfo(m_pSound->getStimulusInfo());
	settings.setName(m_pName->text().trimmed());
	return settings;
}
 QString GUILib::HStimulusSettingsWidget::getName()
 {
	 return m_pName->text().trimmed();
 }

