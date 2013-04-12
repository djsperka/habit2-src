#include "monitormanager.h"
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

using namespace System;

MonitorManager::MonitorManager()
{

}

unsigned int MonitorManager::monitorsCount() const
{
     return QApplication::desktop()->screenCount();
}

MonitorManager::~MonitorManager()
{

}
