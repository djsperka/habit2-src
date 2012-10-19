/*
 *  HLook.cpp
 *  libhabit
 *
 *  Created by Oakes Lab on 6/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include "HLook.h"

QTextStream& operator<<(QTextStream& out, const LookTransType& lt)
{
	switch (lt) {
		case NoneLeft:
			out << "NoneLeft";
			break;
		case LeftNone:
			out << "LeftNone";
			break;
		case NoneCenter:
			out << "NoneCenter";
			break;
		case CenterNone:
			out << "CenterNone";
			break;
		case NoneRight:
			out << "NoneRight";
			break;
		case RightNone:
			out << "RightNone";
			break;
		case NoneNone:
			out << "NoneNone";
			break;
		default:
			out << "Unknown";
			break;
	}
	return out;
}

QTextStream& operator<<(QTextStream& out, const LookDirection& direction)
{
	switch (direction) {
		case NoLook:
			out << "NoLook";
			break;
		case LookLeft:
			out << "LookLeft";
			break;
		case LookRight:
			out << "LookRight";
			break;
		case LookCenter:
			out << "LookCenter";
			break;
		default:
			out << "UnknownLookDirection";
			break;
	}
	return out;
};


QTextStream& operator<<(QTextStream& out, const HLook& l)
{
	out << l.direction() << ":" << l.startMS() << "-" << l.endMS();
	return out;
};




QDataStream& operator<<(QDataStream& out, const LookTransType& lt)
{
	out << (qint32)lt;
	return out;
}

QDataStream& operator<<(QDataStream& out, const LookDirection& direction)
{
	out << (qint32)direction;
	return out;	
}

QDataStream& operator<<(QDataStream& out, const HLook& l)
{
	out << l.m_direction << l.m_startMS << l.m_endMS;
	return out;
};


QDataStream& operator>>(QDataStream& in, LookTransType& type)
{
	in >> (qint32 &)type;
	return in;
}


QDataStream& operator>>(QDataStream& in, LookDirection& direction)
{
	in >> (qint32 &)direction;
	return in;
}

QDataStream& operator>>(QDataStream& in, HLook& l)
{
	in >> l.m_direction >> l.m_startMS >> l.m_endMS;
	return in;
}




QDebug operator<<(QDebug dbg, const LookTransType& lt)
{
	switch (lt) {
		case NoneLeft:
			dbg.nospace() << "NoneLeft";
			break;
		case LeftNone:
			dbg.nospace() << "LeftNone";
			break;
		case NoneCenter:
			dbg.nospace() << "NoneCenter";
			break;
		case CenterNone:
			dbg.nospace() << "CenterNone";
			break;
		case NoneRight:
			dbg.nospace() << "NoneRight";
			break;
		case RightNone:
			dbg.nospace() << "RightNone";
			break;
		case NoneNone:
			dbg.nospace() << "NoneNone";
			break;
		default:
			dbg.nospace() << "Unknown";
			break;
	}
	return dbg.space();
};

QDebug operator<<(QDebug dbg, const LookDirection& direction)
{
	switch (direction) {
		case NoLook:
			dbg.nospace() << "NoLook";
			break;
		case LookLeft:
			dbg.nospace() << "LookLeft";
			break;
		case LookRight:
			dbg.nospace() << "LookRight";
			break;
		case LookCenter:
			dbg.nospace() << "LookCenter";
			break;
		default:
			dbg.nospace() << "UnknownLookDirection";
			break;
	}	
	return dbg.space();
}

QDebug operator<<(QDebug dbg, const HLook& l)
{
	dbg.nospace() << l.direction() << ":" << l.startMS() << "-" << l.endMS();
	return dbg.space();
}

bool operator==(const HLook& lhs, const HLook& rhs)
{
	return lhs.m_direction == rhs.m_direction && lhs.m_startMS == rhs.m_startMS && lhs.m_endMS == rhs.m_endMS;
}
	

