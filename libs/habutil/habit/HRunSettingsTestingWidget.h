/*
 * HRunSettingsTestingWidget.h
 *
 *  Created on: Apr 4, 2018
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HABIT_HRUNSETTINGSTESTINGWIDGET_H_
#define LIBS_HABUTIL_HABIT_HRUNSETTINGSTESTINGWIDGET_H_

#include <QWidget>

namespace Ui
{
	class HRunSettingsTestingForm;
}

namespace GUILib
{

	class HRunSettingsTestingWidget: public QWidget
	{
	public:
		HRunSettingsTestingWidget(QWidget *parent);
		virtual ~HRunSettingsTestingWidget();
		bool isDisplayStimInWindow();

	private:
		Ui::HRunSettingsTestingForm *ui;
	};

} /* namespace GUILib */

#endif /* LIBS_HABUTIL_HABIT_HRUNSETTINGSTESTINGWIDGET_H_ */

