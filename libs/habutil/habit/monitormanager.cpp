#include "monitormanager.h"

#ifdef USING_QT5
#include <QApplication>
#include <QDesktopWidget>
#else
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#endif

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
