/*
 * HPhaseSettingsTabWidget.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: dan
 */

#include <HPhaseSettingsTabWidget.h>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QFontMetrics>

namespace GUILib
{


// TODO: enabled checkbox is not connected to reality
HPhaseSettingsTabWidget::HPhaseSettingsTabWidget(const Habit::HPhaseSettings& phaseSettings, const QString& labelName, const Habit::StimulusDisplayInfo& stimulusDisplayInfo, QWidget *parent)
: QWidget(parent)
{
	QLabel *plabel = new QLabel("Phase name:");
	m_plineeditPhaseName = new QLineEdit;
	m_pcbEnabled = new QCheckBox("Enable this phase");
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addWidget(plabel);
	hbox->addWidget(m_plineeditPhaseName);
	hbox->addWidget(m_pcbEnabled);
	hbox->addStretch(1);

	m_pTabWidget = new QTabWidget(this);
	m_pHabituationWidget = new HHabituationSetupWidget(phaseSettings.habituationSettings());
	m_pTabWidget->addTab(m_pHabituationWidget, "Phase Settings");
	m_pPhaseSettingsWidget = new HPhaseSettingsWidget(phaseSettings);
	m_pTabWidget->addTab(m_pPhaseSettingsWidget, QString("Trial Settings"));
	m_pStimuliWidget = new HStimuliSettingsWidget(labelName, phaseSettings.stimuli(), stimulusDisplayInfo);
	m_pTabWidget->addTab(m_pStimuliWidget, QString("Stimuli"));

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addLayout(hbox);
	vbox->addWidget(m_pTabWidget);
	setLayout(vbox);

	connect(m_plineeditPhaseName, SIGNAL(textChanged(const QString&)), this, SLOT(phaseNameTextChanged(const QString&)));
	connect(m_plineeditPhaseName, SIGNAL(textChanged(const QString&)), this, SIGNAL(phaseNameChanged(const QString&)));
	connect(m_pcbEnabled, SIGNAL(clicked(bool)), m_pTabWidget, SLOT(setEnabled(bool)));
	connect(m_pcbEnabled, SIGNAL(clicked(bool)), this, SIGNAL(phaseEnabledClicked(bool)));

	// initialize
	m_plineeditPhaseName->setText(phaseSettings.getName());
	phaseNameTextChanged(phaseSettings.getName());
	m_pcbEnabled->setChecked(phaseSettings.getIsEnabled());
	m_pTabWidget->setEnabled(phaseSettings.getIsEnabled());	// calling setChecked does not lead to SIGNAL
}

HPhaseSettingsTabWidget::~HPhaseSettingsTabWidget()
{
	// TODO Auto-generated destructor stub
}

Habit::HPhaseSettings HPhaseSettingsTabWidget::getPhaseSettings()
{
	Habit::HPhaseSettings ps = m_pPhaseSettingsWidget->getHPhaseSettings();
	ps.setStimuli(m_pStimuliWidget->getStimuliSettings());
	return ps;
}

void HPhaseSettingsTabWidget::phaseNameTextChanged(const QString& text)
{
    QFontMetrics fm(font());
    int mWidth = fm.width("m");
    int pixelsWide = mWidth * (text.size() + 2);
    int pixelsHigh = fm.height();

    m_plineeditPhaseName->setFixedSize(pixelsWide, pixelsHigh);

    m_plineeditPhaseName->adjustSize();
}

} /* namespace Habit */
