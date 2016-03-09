/*
 *  HLook.cpp
 *  libhabit
 *
 *  Created by Oakes Lab on 6/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include "HLook.h"

static const int f_iVersion2(-2123);

bool isLookToAnyStimulus(const HLook& look)
{
	return (look.direction() == HLookDirection::LookLeft ||
			look.direction() == HLookDirection::LookCenter ||
			look.direction() == HLookDirection::LookRight);
}


QTextStream& operator<<(QTextStream& out, const HLookTrans& lt)
{
	if (lt == HLookTrans::NoneLeft)
	{
		out << "NoneLeft";
	}
	else if (lt == HLookTrans::LeftNone)
	{
		out << "LeftNone";
	}
	else if (lt == HLookTrans::NoneCenter)
	{
		out << "NoneCenter";
	}
	else if (lt == HLookTrans::CenterNone)
	{
		out << "CenterNone";
	}
	else if (lt == HLookTrans::NoneRight)
	{
		out << "NoneRight";
	}
	else if (lt == HLookTrans::RightNone)
	{
		out << "RightNone";
	}
	else if (lt == HLookTrans::NoneNone)
	{
		out << "NoneNone";
	}
	else
	{
		out << "Unknown";
	}
	return out;
}

QTextStream& operator<<(QTextStream& out, const HLookDirection& direction)
{
	if (direction == HLookDirection::LookAway)
	{
		out << "LookAway";
	}
	else if (direction == HLookDirection::LookLeft)
	{
		out << "LookLeft";
	}
	else if (direction == HLookDirection::LookRight)
	{
		out << "LookRight";
	}
	else if (direction == HLookDirection::LookCenter)
	{
		out << "LookCenter";
	}
	else
	{
		out << "UnknownLookDirection";
	}
	return out;
};


QTextStream& operator<<(QTextStream& out, const HLook& l)
{
	out << l.direction() << ":" << l.startMS() << "-" << l.endMS() << "(" << l.lookMS() << ")";
	return out;
};


QDataStream& operator<<(QDataStream& out, const HLook& l)
{
	out << f_iVersion2 << l.direction().number() << l.startMS() << l.endMS() << l.lookMS() << l.sublooks();
	return out;
};

QDataStream& operator>>(QDataStream& in, HLook& l)
{
	int d, t0, t1, t2;
	QList<HLook> sublooks;

	in >> d;
	if (d == f_iVersion2)
	{
		in >> d >> t0 >> t1 >> t2 >> sublooks;
		l.setDirection(getLookDirection(d));
		l.setStartMS(t0);
		l.setEndMS(t1);
		l.setLookMS(t2);
		l.setSublooks(sublooks);
	}
	else
	{
		in >> t0 >> t1 >> t2;
		l.setDirection(getLookDirection(d));
		l.setStartMS(t0);
		l.setEndMS(t1);
		l.setLookMS(t2);
		// no sublooks for old version
	}
	return in;
}

QDebug operator<<(QDebug dbg, const HLookTrans& lt)
{
	if (lt == HLookTrans::NoneLeft)
	{
		dbg.nospace() << "NoneLeft";
	}
	else if (lt == HLookTrans::LeftNone)
	{
		dbg.nospace() << "LeftNone";
	}
	else if (lt == HLookTrans::NoneCenter)
	{
		dbg.nospace() << "NoneCenter";
	}
	else if (lt == HLookTrans::CenterNone)
	{
		dbg.nospace() << "CenterNone";
	}
	else if (lt == HLookTrans::NoneRight)
	{
		dbg.nospace() << "NoneRight";
	}
	else if (lt == HLookTrans::RightNone)
	{
		dbg.nospace() << "RightNone";
	}
	else if (lt == HLookTrans::NoneNone)
	{
		dbg.nospace() << "NoneNone";
	}
	else
	{
		dbg.nospace() << "Unknown";
	}
	return dbg.space();
};

QDebug operator<<(QDebug dbg, const HLookDirection& direction)
{
	if (direction == HLookDirection::LookAway)
	{
		dbg.nospace() << "LookAway";
	}
	else if (direction == HLookDirection::LookLeft)
	{
		dbg.nospace() << "LookLeft";
	}
	else if (direction == HLookDirection::LookRight)
	{
		dbg.nospace() << "LookRight";
	}
	else if (direction == HLookDirection::LookCenter)
	{
		dbg.nospace() << "LookCenter";
	}
	else
	{
		dbg.nospace() << "UnknownLookDirection";
	}
	return dbg.space();
}

QDebug operator<<(QDebug dbg, const HLook& l)
{
	dbg.nospace() << l.direction() << ":" << l.startMS() << "-" << l.endMS() << "=" << l.lookMS() << endl << l.sublooks();

	return dbg.space();
}

QDebug operator<<(QDebug dbg, const QList<HLook>& looklist)
{
	dbg.nospace() << "Contains: " << looklist.size() << endl;
	QListIterator<HLook> it(looklist);
	int i=1;
	while (it.hasNext())
	{
		dbg << i++ << ": " << it.next() << endl;
	}
	return dbg.nospace();
}


bool operator==(const HLook& lhs, const HLook& rhs)
{
	return lhs.direction() == rhs.direction() && lhs.startMS() == rhs.startMS() && lhs.endMS() == rhs.endMS() && lhs.lookMS()==rhs.lookMS() && lhs.sublooks() == rhs.sublooks();
}
	

