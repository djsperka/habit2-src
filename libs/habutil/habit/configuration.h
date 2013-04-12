
#ifndef HABIT_CONFIGURATION_H
#define HABIT_CONFIGURATION_H

#include <QtCore/QString>

namespace Habit {

class configuration
{
public:
	static configuration* get_instance();

public:
	QString get_database_password() const {
		return m_database_password.isEmpty() ? "" : m_database_password;
	}

	QString get_database_name() const {
		return m_database_name.isEmpty() ? "habit.db3" : m_database_name;
	}

private:
	void load_configuration();

private:
	static configuration* m_instance;

private:
	QString m_database_password;
	QString m_database_name;

private:
	configuration();
};

} // namespace Habit

#endif // HABIT_CONFIGURATION_H
