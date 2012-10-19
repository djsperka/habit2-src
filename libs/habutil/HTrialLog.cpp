/*
 *  HTrialLog.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/16/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HTrialLog.h"

int HTrialLog::totalLookingTime() const
{
	int total = 0;
	foreach(const HLook& look, *this)
	{
		total += look.lookMS();
	}
	return total;
}


QDataStream & operator << (QDataStream& stream, const HTrialLog &tlog)
{
	stream << tlog.getStatus() << tlog.getStimulusSettings() << static_cast <const QList<HLook>&>(tlog);
	return stream;
}

QDataStream & operator >> (QDataStream& stream, HTrialLog& tlog)
{
	HTrialLog::TrialStatus s;
	Habit::StimulusSettings settings;
	stream >> s >> settings >> static_cast<QList<HLook>&>(tlog);
	tlog.setStatus(s);
	tlog.setStimulusSettings(settings);
	return stream;
}



QDataStream & operator << (QDataStream& stream, const HTrialLog::TrialStatus& s)
{	
	stream << (qint32)s;
#if 0
	switch (s)
	{
		case HTrialLog::TrialRunning : 
			stream << "TrialRunning"; 
			qDebug() << "TrialRunning"; 
			break;
		case HTrialLog::TrialCompleted : 
			stream << "TrialCompleted"; 
			qDebug() << "TrialCompleted"; 
			break;
		case HTrialLog::TrialFailed : 
			stream << "TrialFailed";
			qDebug() << "TrialFailed";
			break;
		case HTrialLog::TrialAborted : 
			stream << "TrialAborted";
			qDebug() << "TrialAborted";
			break;
		case HTrialLog::UnknownTrialStatus : 
			stream << "UnknownTrialStatus";
			qDebug() << "UnknownTrialStatus";
			break;
		default:
			stream << "UnknownTrialStatus";
			qDebug() << "UnknownTrialStatus";
			break;
	}
#endif
	return stream;
}

QDataStream & operator >> (QDataStream& stream, HTrialLog::TrialStatus& s)
{
	stream >> (qint32 &)s;
#if 0
	QString str;
	qDebug() << "Operator>>(TrialStatus): start pos " << stream.device()->pos();
	stream >> str;
	qDebug() << "Read trial status str " << str << " status " << stream.status() << " pos " << stream.device()->pos();
	if (str == "TrialRunning")
	{
		s = HTrialLog::TrialRunning;
	}
	else if (str == "TrialCompleted")
	{
		s = HTrialLog::TrialCompleted;
	}
	else if (str == "TrialFailed")
	{
		s = HTrialLog::TrialFailed;
	}
	else if (str == "TrialAborted")
	{
		s = HTrialLog::TrialAborted;
	}
	else if (str == "UnknownTrialStatus")
	{
		s = HTrialLog::UnknownTrialStatus;
	}
	else 
	{
		s = HTrialLog::UnknownTrialStatus;
	}
#endif
	return stream;
}


bool operator==(const HTrialLog& lhs, const HTrialLog& rhs)
{
	return lhs.getStatus()==rhs.getStatus() && lhs.getStimulusSettings()==rhs.getStimulusSettings() && static_cast<const QList<HLook>&>(lhs) == static_cast<const QList<HLook>&>(rhs);
}

