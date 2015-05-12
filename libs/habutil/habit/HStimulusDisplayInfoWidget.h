/*
 * HStimulusDisplayInfoWidget.h
 *
 *  Created on: Oct 14, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSDISPLAYINFOWIDGET_H_
#define HSTIMULUSDISPLAYINFOWIDGET_H_

#include "stimulusdisplayinfo.h"
#include <QWidget>

namespace Ui
{
	class HStimulusDisplayInfoForm;
}


namespace GUILib
{
	class HStimulusDisplayInfoWidget : public QWidget
	{
		Q_OBJECT

	public:
		HStimulusDisplayInfoWidget(const Habit::StimulusDisplayInfo& s, QWidget *parent=0);
		virtual ~HStimulusDisplayInfoWidget();
		Habit::StimulusDisplayInfo getStimulusDisplayInfo();

	private:

		Ui::HStimulusDisplayInfoForm *ui;
		Habit::StimulusDisplayInfo m_info;

		void initialize();

	signals:
		void stimulusLayoutTypeChanged(int i);

	private slots:
		void onColorChooserClick();
		void listItemSelectionChanged();

	};
};






#endif /* HSTIMULUSDISPLAYINFOWIDGET_H_ */
