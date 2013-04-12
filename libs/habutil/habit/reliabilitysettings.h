#ifndef HABIT_RELIABILITY_SETTINGS_H
#define HABIT_RELIABILITY_SETTINGS_H

#include <QtCore/QString>
#include <QtCore/QDate>
#include <QtCore/QDataStream>

namespace Habit {

class ReliabilitySettings
{
public:
	ReliabilitySettings(void);
	~ReliabilitySettings(void);

	QString getObserver() { return _observer; }
	void	setObserver(const QString& value) { _observer = value; }
	QDate	getDate() { return _date; }
	void	setDate(QDate value) { _date = value; }
	QString	getComment() { return _comment; }
	void	setComment(const QString& value) { _comment = value; }

private:
	QString _observer;
	QDate	_date;
	QString	_comment;
};

QDataStream & operator << (QDataStream& stream, ReliabilitySettings settings);
QDataStream & operator >> (QDataStream& stream, ReliabilitySettings& settings);

}

#endif