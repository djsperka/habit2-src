/*
 * HStimulusListWidget.cpp
 *
 *  Created on: Oct 9, 2014
 *      Author: Oakeslab
 */


#include "HStimulusSettingsWidget.h"
#include "HStimulusOrder.h"
#include "HNameValidator.h"
#include "HTypes.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegExpValidator>

GUILib::HStimulusSettingsWidget::HStimulusSettingsWidget(const Habit::StimulusSettings& settings, const Habit::StimulusDisplayInfo& sdi, QWidget *parent)
: QWidget(parent)
, m_settings(settings)
, m_sdi(sdi)
{
	// create HStimulusInfoWidget for each possible stimulus.
	m_pLeft = new HStimulusInfoWidget(m_settings.getLeftStimulusInfo(), QString("L:"), NULL, true);
	m_pCenter = new HStimulusInfoWidget(m_settings.getCenterStimulusInfo(), QString("C:"), NULL, true);
	m_pRight = new HStimulusInfoWidget(m_settings.getRightStimulusInfo(), QString("R:"), NULL, true);
	m_pSound = new HStimulusInfoWidget(m_settings.getIndependentSoundInfo(), QString("S:"), NULL, false);

	// label
	QLabel *pNameLabel = new QLabel("Stimulus name:");
	m_pName = new QLineEdit(m_settings.getName(), this);
	m_pName->setValidator(new HNameValidator());

	QWidget *pwidget = new QWidget(this);
	QVBoxLayout *vbox = new QVBoxLayout();

	// stack for single stim or l/r stim.
	m_pStack = new QStackedWidget(this);

	if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		vbox->addWidget(m_pCenter);
		if (sdi.getUseISS()) vbox->addWidget(m_pSound);
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		vbox->addWidget(m_pLeft);
		vbox->addWidget(m_pRight);
		if (sdi.getUseISS()) vbox->addWidget(m_pSound);
	}
	else
	{
		vbox->addWidget(m_pCenter);
		if (sdi.getUseISS()) vbox->addWidget(m_pSound);
		qWarning() << "layout type not set in ctor GUILib::HStimulusSettingsWidget::HStimulusSettingsWidget(const Habit::StimulusSettings& settings, const Habit::StimulusDisplayInfo& sdi, QWidget *parent)";
	}

	pwidget->setLayout(vbox);

	// name and label
	QHBoxLayout *hname = new QHBoxLayout();
	hname->addWidget(pNameLabel);
	hname->addWidget(m_pName);

	// TODO: m_pName needs a validator

	// now put the label and the stack side-by-side
	QHBoxLayout *h = new QHBoxLayout();
	h->addLayout(hname, 0);
	h->addWidget(pwidget, 1);
	setLayout(h);

	// squish it from the top
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

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

#if 0
void GUILib::HStimulusSettingsWidget::setStimulusLayoutType(const HStimulusLayoutType& type)
{
	// make sure sound widget is not in either page in the stack
//	m_pStack->widget(m_stackidSingle)->layout()->removeWidget(m_pSound);
//	m_pStack->widget(m_stackidLR)->layout()->removeWidget(m_pSound);
	m_pwSingle->layout()->removeWidget(m_pSound);
	m_pwLR->layout()->removeWidget(m_pSound);
	if (type == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		if (m_sdi.getUseISS()) m_pwSingle->layout()->addWidget(m_pSound);
		m_pStack->setCurrentIndex((m_stackidSingle));
	}
	else if (type == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		if (m_sdi.getUseISS()) m_pwLR->layout()->addWidget(m_pSound);
		m_pStack->setCurrentIndex((m_stackidLR));
	}
	else
	{
		//default is single stim
		if (m_sdi.getUseISS()) m_pwSingle->layout()->addWidget(m_pSound);
		m_pStack->setCurrentIndex((m_stackidSingle));
	}
}
#endif

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

