/*
 * HPhaseSettingsTabWidget.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: dan
 */

#include <HPhaseSettingsTabWidget.h>
#include <HNameValidator.h>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QFontMetrics>

namespace GUILib
{

HPhaseSettingsTabWidget::HPhaseSettingsTabWidget(const Habit::HPhaseSettings& phaseSettings, const QString& labelName, const Habit::StimulusDisplayInfo& stimulusDisplayInfo, bool bReadOnly, QWidget *parent)
: QWidget(parent)
{
	QLabel *plabel = new QLabel("Phase name:");
	m_plineeditPhaseName = new QLineEdit;
	m_plineeditPhaseName->setValidator(new HNameValidator());
	m_pcbEnabled = new QCheckBox("Enable this phase");
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addWidget(plabel);
	hbox->addWidget(m_plineeditPhaseName);
	hbox->addWidget(m_pcbEnabled);
	hbox->addStretch(1);

	m_pTabWidget = new QTabWidget(this);
	//m_pHabituationWidget = new HHabituationSetupWidget(phaseSettings.habituationSettings());
	m_pHabituationSettingsWidget = new HHabituationSettingsWidget(phaseSettings.habituationSettings());
	m_pHabituationSettingsWidget->setDisabled(bReadOnly);
	m_pTabWidget->addTab(m_pHabituationSettingsWidget, "Phase Settings");
	m_pPhaseSettingsWidget = new HPhaseSettingsWidget(phaseSettings);
	m_pPhaseSettingsWidget->setDisabled(bReadOnly);
	m_pTabWidget->addTab(m_pPhaseSettingsWidget, QString("Trial Settings"));
	m_pStimuliWidget = new HStimuliSettingsWidget(labelName, phaseSettings.stimuli(), phaseSettings.getSeqno(), stimulusDisplayInfo);
	m_pStimuliWidget->setDisabled(bReadOnly);
	m_pTabWidget->addTab(m_pStimuliWidget, QString("Stimuli"));

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addLayout(hbox);
	vbox->addWidget(m_pTabWidget);
	setLayout(vbox);

	connect(m_plineeditPhaseName, SIGNAL(textChanged(const QString&)), this, SLOT(phaseNameTextChanged(const QString&)));
	connect(m_plineeditPhaseName, SIGNAL(textChanged(const QString&)), this, SIGNAL(phaseNameChanged(const QString&)));
	connect(m_pcbEnabled, SIGNAL(clicked(bool)), m_pTabWidget, SLOT(setEnabled(bool)));
	connect(m_pcbEnabled, SIGNAL(clicked(bool)), this, SIGNAL(phaseEnabledClicked(bool)));
	// This is not connected any more. FIXME
	//connect(this, SIGNAL(stimulusLayoutTypeChanged(int)), m_pStimuliWidget, SLOT(stimulusLayoutTypeChanged(int)));

	// initialize
	m_plineeditPhaseName->setText(phaseSettings.getName());
	phaseNameTextChanged(phaseSettings.getName());
	m_pcbEnabled->setChecked(phaseSettings.getIsEnabled());
	m_pTabWidget->setEnabled(phaseSettings.getIsEnabled());	// calling setChecked does not lead to SIGNAL

	m_plineeditPhaseName->setDisabled(bReadOnly);
	m_pcbEnabled->setDisabled(bReadOnly);
}

HPhaseSettingsTabWidget::~HPhaseSettingsTabWidget()
{
}

Habit::HPhaseSettings HPhaseSettingsTabWidget::getPhaseSettings()
{
	Habit::HPhaseSettings ps = m_pPhaseSettingsWidget->getHPhaseSettings();
	ps.setStimuli(m_pStimuliWidget->getStimuliSettings());
	ps.setHabituationSettings(m_pHabituationSettingsWidget->getHabituationSettings());

	// get name and 'phase enabled' settings. Seqno should be the same
	// as it was -- the PhaseSettingsWidget will set it so.
	ps.setName(m_plineeditPhaseName->text().trimmed());
	ps.setIsEnabled(m_pcbEnabled->isChecked());
	return ps;
}

void HPhaseSettingsTabWidget::phaseNameTextChanged(const QString& text)
{
    QFontMetrics fm(font());
    //int mWidth = fm.width("m");
    int pixelsWide = fm.horizontalAdvance(text);
    int pixelsHigh = fm.height();

    m_plineeditPhaseName->setFixedSize(pixelsWide, pixelsHigh);

    m_plineeditPhaseName->adjustSize();
}

void HPhaseSettingsTabWidget::stimulusDisplayInfoChanged(const Habit::StimulusDisplayInfo& info)
{
	qDebug() << "HPhaseSettingsTabWidget::stimulusDisplayInfoChanged:";
	qDebug() << info;
}

} /* namespace Habit */
