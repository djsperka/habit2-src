/*
 * HHabituationSetupWidget.h
 *
 *  Created on: Feb 6, 2015
 *      Author: Oakeslab
 */

#ifndef HHABITUATIONSETUPWIDGET_H_
#define HHABITUATIONSETUPWIDGET_H_

#include "habituationsettings.h"
#include <QWidget>

namespace Ui
{
	class HHabituationSetupForm;
}


namespace GUILib
{
	class HHabituationSetupWidget : public QWidget
	{
		Q_OBJECT

	public:
		HHabituationSetupWidget(const Habit::HabituationSettings& settings, QWidget* w = 0);
		HHabituationSetupWidget(QWidget* w = 0);
		~HHabituationSetupWidget();

		Habit::HabituationSettings getHabituationSettings();
		void setHabituationSettings(const Habit::HabituationSettings&);

	private:
		void initializeComponents();
		void initialize();
		void connections();

		Ui::HHabituationSetupForm *ui;
		Habit::HabituationSettings m_settings;

	};
};

#endif /* HHABITUATIONSETUPWIDGET_H_ */
