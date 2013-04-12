#include "reliabilitysettings.h"

namespace Habit {

ReliabilitySettings::ReliabilitySettings(void)
{
}

ReliabilitySettings::~ReliabilitySettings(void)
{
}

QDataStream & operator<<( QDataStream& stream, ReliabilitySettings settings )
{
	stream << settings.getDate() << settings.getObserver() << settings.getComment();
	return stream;
}

QDataStream & operator>>( QDataStream& stream, ReliabilitySettings& settings )
{
	QDate date;
	stream >> date;
	settings.setDate(date);
	QString observer;
	stream >> observer;
	settings.setObserver(observer);
	QString comment;
	stream >> comment;
	settings.setComment(comment);
	return stream;
}

}