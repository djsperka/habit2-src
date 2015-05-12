/*
 * HMonitorSettingsWidget.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: Oakeslab
 */

#include "HMonitorSettingsWidget.h"
#include "ui_HMonitorSettingsForm.h"

using namespace GUILib;

HMonitorSettingsWidget::HMonitorSettingsWidget(const Habit::MonitorSettings& s, QWidget *parent)
: QWidget(parent)
, ui(new Ui::HMonitorSettingsForm)
, m_settings(s)
{
	ui->setupUi(this);
}

HMonitorSettingsWidget::~HMonitorSettingsWidget()
{
	delete ui;
}


