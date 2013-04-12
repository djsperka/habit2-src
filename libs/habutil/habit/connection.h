#ifndef HABIT_CONNECTION_H
#define HABIT_CONNECTION_H

#include <QtSql/QSqlDatabase>

namespace Habit {

/**
	Singleton class which provides connection with database
 */
class connection : public QSqlDatabase
{
public:
	static connection* get_instance();
	
	QSqlQuery exec ( const QString & query = QString() ) const;

private:
	static connection* m_instance;

private:
	connection();

public:
	~connection();
};

} // namespace Habit

#endif // HABIT_CONNECTION_H
