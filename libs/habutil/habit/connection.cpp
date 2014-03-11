#include "connection.h"
#include "configuration.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtCore/QDebug>

namespace Habit {

connection* connection::m_instance = 0;

connection* connection::get_instance() {
	Q_ASSERT(false);		// should not call this function!
	if(0 == m_instance) {
		m_instance = new connection();
	}
	return m_instance;
}

connection::connection()
	: QSqlDatabase("QSQLITE")
{
	configuration* config = configuration::get_instance();
	Q_ASSERT(0 != config);
	setPassword(config->get_database_password());
	setDatabaseName(config->get_database_name());
	qDebug() << "database name " << config->get_database_name();
	if(open()) {
		qDebug() << "Opened";
	} else {
		qDebug() << "Not Opened";
	}
}

connection::~connection() {
}

QSqlQuery connection::exec ( const QString & query ) const
{
	QSqlQuery qq = QSqlDatabase::exec(query);
	//qDebug() << "Q: " << qq.lastQuery();
	return qq;
}
	
	
	
} // namespace Habit
