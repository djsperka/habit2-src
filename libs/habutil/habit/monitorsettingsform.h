#ifndef MONITORSETTINGSFORM_H
#define MONITORSETTINGSFORM_H

#include "monitorsettings.h"

#include <QWidget>

namespace Ui {class MonitorSettingsFormClass;};

namespace System
{
    class MonitorManager;
}

namespace GUILib
{
    class MonitorSettingsForm;
}

class GUILib::MonitorSettingsForm : public QWidget
{
    Q_OBJECT

public:
    MonitorSettingsForm(const Habit::MonitorSettings& monitorSettings, QWidget *w = 0);
    ~MonitorSettingsForm();

    void setMonitorSettings(const Habit::MonitorSettings& monitorSettings);
    Habit::MonitorSettings getMonitorSettings() const;

    void updateMonitorSettings();

public:
	Habit::MonitorSettings getConfigurationObject();
	void setConfigurationObject(const Habit::MonitorSettings&);
	void initialize();

public slots:
    void identifyMonitors(bool msgBox = true);

private:
    Ui::MonitorSettingsFormClass *ui;
    System::MonitorManager *monitorManager_;
    Habit::MonitorSettings monitorSettings_;
};

#endif // MONITORSETTINGSFORM_H
