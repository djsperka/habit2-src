/*
 * HPhaseSettingsTabWidget.h
 *
 *  Created on: Jun 8, 2016
 *      Author: dan
 */

#ifndef HPHASESETTINGSTABWIDGET_H_
#define HPHASESETTINGSTABWIDGET_H_

#include <QTabWidget>
#include <QLineEdit>
#include <QCheckBox>
#include "HPhaseSettingsWidget.h"
#include "HStimuliSettingsWidget.h"
#include "HHabituationSettingsWidget.h"

namespace GUILib {

class HPhaseSettingsTabWidget: public QWidget
{
	Q_OBJECT

private:
	QLineEdit *m_plineeditPhaseName;
	QCheckBox *m_pcbEnabled;
	QTabWidget *m_pTabWidget;
	HPhaseSettingsWidget* m_pPhaseSettingsWidget;
	HStimuliSettingsWidget* m_pStimuliWidget;
	//HHabituationSetupWidget* m_pHabituationWidget;
	HHabituationSettingsWidget *m_pHabituationSettingsWidget;

public:
	HPhaseSettingsTabWidget(const Habit::HPhaseSettings& phaseSettings, const QString& labelName, const Habit::StimulusDisplayInfo& stimulusDisplayInfo, QWidget *parent=0);
	virtual ~HPhaseSettingsTabWidget();
	Habit::HPhaseSettings getPhaseSettings();

signals:
	void phaseNameChanged(const QString&);
	void phaseEnabledClicked(bool);

private slots:
	void phaseNameTextChanged(const QString& s);
};

} /* namespace Habit */

#endif /* HPHASESETTINGSTABWIDGET_H_ */
