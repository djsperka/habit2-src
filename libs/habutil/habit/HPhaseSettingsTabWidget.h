/*
 * HPhaseSettingsTabWidget.h
 *
 *  Created on: Jun 8, 2016
 *      Author: dan
 */

#ifndef HPHASESETTINGSTABWIDGET_H_
#define HPHASESETTINGSTABWIDGET_H_

#include <QTabWidget>
#include "HPhaseSettingsWidget.h"
#include "HStimuliSettingsWidget.h"

namespace GUILib {

class HPhaseSettingsTabWidget: public QTabWidget
{
	Q_OBJECT

private:
	HPhaseSettingsWidget* m_pPhaseSettingsWidget;
	HStimuliSettingsWidget* m_pStimuliWidget;

public:
	HPhaseSettingsTabWidget(const Habit::HPhaseSettings& phaseSettings, const QString& labelName, const Habit::StimulusDisplayInfo& stimulusDisplayInfo, QWidget *parent=0);
	virtual ~HPhaseSettingsTabWidget();
	Habit::HPhaseSettings getPhaseSettings();
};

} /* namespace Habit */

#endif /* HPHASESETTINGSTABWIDGET_H_ */
