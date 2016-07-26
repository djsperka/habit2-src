/*
 * HHabituationSettingsWidget.h
 *
 *  Created on: Jul 26, 2016
 *      Author: dan
 */

#ifndef HHABITUATIONSETTINGSWIDGET_H_
#define HHABITUATIONSETTINGSWIDGET_H_

#include <QWidget>
#include "habituationsettings.h"
#include "HHabituationTypeWidget.h"
#include "HHabituationCriteriaWidget.h"

namespace GUILib {

class HHabituationSettingsWidget: public QWidget
{
	Q_OBJECT

public:
	HHabituationSettingsWidget(const Habit::HabituationSettings& habituationSettings, QWidget *parent=NULL);
	virtual ~HHabituationSettingsWidget();
	Habit::HabituationSettings getHabituationSettings();

private:
	Habit::HabituationSettings m_settings;
	HHabituationTypeWidget *m_pTypeWidget;
	HHabituationCriteriaWidget *m_pCriteriaWidget;
};

} /* namespace GUILib */

#endif /* HHABITUATIONSETTINGSWIDGET_H_ */
