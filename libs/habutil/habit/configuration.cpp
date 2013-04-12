#include "configuration.h"
#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>

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

void configuration::load_configuration() {
	m_database_name = QDesktopServices::storageLocation(QDesktopServices::DataLocation) ;
#ifdef Q_OS_MAC
	m_database_name += "/habit.db3";
#else
	m_database_name += "\\habit.db3";
#endif
	m_database_password = "";
}

} //namespace habit