#ifndef CONTROLBAROPTIONSFORM_H
#define CONTROLBAROPTIONSFORM_H

#include "controlbaroptions.h"

#ifndef USING_QT5
#include <QtGui/QWidget>
#else
#include <QtWidgets>
#endif


namespace Ui {class ControlBarOptionsFormClass;};

namespace GUILib 
{
    class ControlBarOptionsForm;
}

class GUILib::ControlBarOptionsForm : public QWidget
{
    Q_OBJECT

public:
    ControlBarOptionsForm(const Habit::ControlBarOptions& controlBarOptions, QWidget *w = 0);
    ~ControlBarOptionsForm();

public:
	 Habit::ControlBarOptions getConfigurationObject();
	 void setConfigurationObject(const Habit::ControlBarOptions&);
	 void initialize();

private:
    Ui::ControlBarOptionsFormClass *ui;
    Habit::ControlBarOptions controlBarOptions_;
};

#endif // CONTROLBAROPTIONSFORM_H
