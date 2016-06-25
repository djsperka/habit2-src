/*
 * HPhaseSettingsTabWidget.cpp
 *
 *  Created on: Jun 8, 2016
 *      Author: dan
 */

#include <HPhaseSettingsTabWidget.h>

namespace GUILib
{

HPhaseSettingsTabWidget::HPhaseSettingsTabWidget(const Habit::HPhaseSettings& phaseSettings, const QString& labelName, const Habit::StimulusDisplayInfo& stimulusDisplayInfo, QWidget *parent)
: QTabWidget(parent)
{
	m_pPhaseSettingsWidget = new HPhaseSettingsWidget(phaseSettings, this);
	addTab(m_pPhaseSettingsWidget, QString("Phase/Trial Settings"));
	m_pStimuliWidget = new HStimuliSettingsWidget(labelName, phaseSettings.stimuli(), stimulusDisplayInfo, this);
	addTab(m_pStimuliWidget, QString("Stimuli"));
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


} /* namespace Habit */
