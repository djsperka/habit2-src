/*
 * HHabituationTypeWidget.h
 *
 *  Created on: Jul 15, 2016
 *      Author: dan
 */

#ifndef HHABITUATIONTYPEWIDGET_H_
#define HHABITUATIONTYPEWIDGET_H_

#include "ui_HHabituationCriteriaWidget.h"
#include "HTypes.h"
#include "criterionsettings.h"

namespace Ui
{
	class HHabituationCriteriaWidget;
}


namespace GUILib
{
	class HHabituationCriteriaWidget : public QStackedWidget
	{
		Q_OBJECT

	public:
		HHabituationCriteriaWidget(int ntrials = 3, const Habit::CriterionSettings& cs = Habit::CriterionSettings(), int totalLookingTime = 0, QWidget* w = 0);
		~HHabituationCriteriaWidget() {};
		int getNTrials();
		int getTotalLookLength();
		const Habit::CriterionSettings getCriterionSettings();
	private:
		Ui::HHabituationCriteriaWidget *ui;
	};
};



#endif /* HHABITUATIONTYPEWIDGET_H_ */
