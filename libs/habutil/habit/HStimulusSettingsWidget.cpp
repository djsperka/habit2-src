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
{
	components();
	QWidget *pwidget = getEditorWidget(sdi);
	setLayout(makeLayout(pwidget));

	// squish it from the top
	//setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

}

GUILib::HStimulusSettingsWidget::HStimulusSettingsWidget(const Habit::StimulusSettings& settings, const HPlayerPositionType& ppt, QWidget *parent)
: QWidget(parent)
, m_settings(settings)
{
	components();
	QWidget *pwidget = getEditorWidget(ppt);
	setLayout(makeLayout(pwidget));

	// squish it from the top
	//setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

}

GUILib::HStimulusSettingsWidget::~HStimulusSettingsWidget() {};

QLayout *GUILib::HStimulusSettingsWidget::makeLayout(QWidget *pwidget)
{
	// make a lineedit and label, stick it in an hbox for the stimulus name
	QLabel *pNameLabel = new QLabel("Stimulus name:");

	QHBoxLayout *hname = new QHBoxLayout();
	hname->addWidget(pNameLabel);
	hname->addWidget(m_pName);

	// now put the label and the stack side-by-side
	QHBoxLayout *h = new QHBoxLayout();
	h->addLayout(hname, 0);
	h->addWidget(pwidget, 1);
	return h;
}

QWidget *GUILib::HStimulusSettingsWidget::getEditorWidget(const Habit::StimulusDisplayInfo& sdi)
{
	// Now the vbox layout that holds the HStimulusInfoWidget(s) to be used in the dialog.
	QWidget *pwidget = new QWidget(this);
	QVBoxLayout *vbox = new QVBoxLayout();

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
	return pwidget;
}

QWidget *GUILib::HStimulusSettingsWidget::getEditorWidget(const HPlayerPositionType& ppt)
{
	QWidget *pwidget = new QWidget(this);
	QVBoxLayout *vbox = new QVBoxLayout();
	if (ppt == HPlayerPositionType::Sound)
	{
		vbox->addWidget(m_pSound);
	}
	else if (ppt == HPlayerPositionType::Left)
	{
		vbox->addWidget(m_pLeft);
	}
	else if (ppt == HPlayerPositionType::Right)
	{
		vbox->addWidget(m_pRight);
	}
	else if (ppt == HPlayerPositionType::Center)
	{
		vbox->addWidget(m_pCenter);
	}
	pwidget->setLayout(vbox);
	return pwidget;
}

void GUILib::HStimulusSettingsWidget::components()
{
	// create HStimulusInfoWidget for each possible stimulus, and lineedit for the name.
	m_pLeft = new HStimulusInfoWidget(m_settings.getLeftStimulusInfo(), QString("Left:"), NULL, true);
	m_pCenter = new HStimulusInfoWidget(m_settings.getCenterStimulusInfo(), QString("Center:"), NULL, true);
	m_pRight = new HStimulusInfoWidget(m_settings.getRightStimulusInfo(), QString("Right:"), NULL, true);
	m_pSound = new HStimulusInfoWidget(m_settings.getIndependentSoundInfo(), QString("Audio:"), NULL, false);
	m_pName = new QLineEdit(m_settings.getName(), this);
	m_pName->setValidator(new HNameValidator());
	connect(m_pLeft, SIGNAL(stimulusInfoChanged()), this, SIGNAL(stimulusSettingsChanged()));
	connect(m_pCenter, SIGNAL(stimulusInfoChanged()), this, SIGNAL(stimulusSettingsChanged()));
	connect(m_pRight, SIGNAL(stimulusInfoChanged()), this, SIGNAL(stimulusSettingsChanged()));
	connect(m_pSound, SIGNAL(stimulusInfoChanged()), this, SIGNAL(stimulusSettingsChanged()));
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

