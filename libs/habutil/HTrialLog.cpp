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
	return stream;
}

QDataStream & operator >> (QDataStream& stream, HTrialLog::TrialStatus& s)
{
	stream >> (qint32 &)s;
	return stream;
}


bool operator==(const HTrialLog& lhs, const HTrialLog& rhs)
{
	return lhs.getStatus()==rhs.getStatus() && lhs.getStimulusSettings()==rhs.getStimulusSettings() && static_cast<const QList<HLook>&>(lhs) == static_cast<const QList<HLook>&>(rhs);
}

