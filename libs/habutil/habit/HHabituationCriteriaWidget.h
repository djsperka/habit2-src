/*
 * HHabituationTypeWidget.h
 *
 *  Created on: Jul 15, 2016
 *      Author: dan
 */

#ifndef HHABITUATIONCRITERIAWIDGET_H_
#define HHABITUATIONCRITERIAWIDGET_H_

#include "HTypes.h"
#include "criterionsettings.h"
#include <QStackedWidget>

namespace Ui
{
	class HHabituationCriteriaForm;
};


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
		Ui::HHabituationCriteriaForm *ui;

	};
};



#endif /* HHABITUATIONTYPEWIDGET_H_ */
