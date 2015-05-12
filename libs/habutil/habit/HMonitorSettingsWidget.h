/*
 * HMonitorSettingsWidget.h
 *
 *  Created on: Oct 9, 2014
 *      Author: Oakeslab
 */

#ifndef HMONITORSETTINGSWIDGET_H_
#define HMONITORSETTINGSWIDGET_H_

#include "monitorsettings.h"
#include <QWidget>

namespace Ui
{
	class HMonitorSettingsForm;
}


namespace GUILib
{
	class HMonitorSettingsWidget : public QWidget
	{
	public:
		HMonitorSettingsWidget(const Habit::MonitorSettings& s, QWidget *parent=0);
		virtual ~HMonitorSettingsWidget();

	private:

		Ui::HMonitorSettingsForm *ui;
		Habit::MonitorSettings m_settings;

	};
};

#endif /* HMONITORSETTINGSWIDGET_H_ */
