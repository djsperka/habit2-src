/*
 * HHabituationTypeWidget.h
 *
 *  Created on: Jul 15, 2016
 *      Author: dan
 */

#ifndef HHABITUATIONTYPEWIDGET_H_
#define HHABITUATIONTYPEWIDGET_H_

#include "ui_HHabituationTypeWidget.h"
#include "HTypes.h"

namespace Ui
{
	class HHabituationTypeWidget;
}


namespace GUILib
{
	class HHabituationTypeWidget : public QWidget
	{
		Q_OBJECT

	signals:
	    void currentIndexChanged(int);

	public:
		HHabituationTypeWidget(const HHabituationType& type = HHabituationType::HHabituationTypeFixedN, QWidget* w = 0);
		~HHabituationTypeWidget() {};
		const HHabituationType& getHabituationType();

	private:
		Ui::HHabituationTypeWidget *ui;
	};
};



#endif /* HHABITUATIONTYPEWIDGET_H_ */
