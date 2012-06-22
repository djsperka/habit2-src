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
		case NoneEnd:
			out << "NoneEnd";
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

