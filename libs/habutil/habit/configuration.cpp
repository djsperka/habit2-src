#include "configuration.h"
#include <QApplication>

#ifndef USING_QT5
#include <QDesktopServices>
#else
#include <QStandardPaths>
#endif

namespace Habit {
		
configuration* configuration::m_instance = 0;

configuration* configuration::get_instance() {
	if(0 == m_instance) {
		m_instance = new configuration();
	}
	return m_instance;
}

configuration::configuration() 
	: m_database_password("")
	, m_database_name("")
{
	load_configuration();
}

void configuration::load_configuration()
{

#ifndef USING_QT5
	m_database_name = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else
	m_database_name = QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0];
#endif

#ifdef Q_OS_MAC
	m_database_name += "/habit.db3";
#else
	m_database_name += "\\habit.db3";
#endif
	m_database_password = "";
}

} //namespace habit
