/*
 *  HLook.h
 *  libhabit
 *
 *  Created by Oakes Lab on 6/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _HLOOK_H_
#define _HLOOK_H_

#include <QTime>
#include <QTextStream>

enum LookTransType { NoneLeft=0, LeftNone, NoneCenter, CenterNone, NoneRight, RightNone, NoneEnd };
enum LookDirection { NoLook=0, LookLeft, LookRight, LookCenter };

class HLook
{
public:
	HLook(LookDirection direc, QTime& start, QTime& end): m_direction(direc), m_starttime(start), m_endtime(end) {};
	~HLook() {};
	LookDirection direction() const { return m_direction; };
	const QTime& starttime() const { return m_starttime; };
	const QTime& endtime() const { return m_endtime; };
private:
	LookDirection m_direction;
	QTime m_starttime;
	QTime m_endtime;
};

QTextStream& operator<<(QTextStream& out, const LookTransType& type);
QTextStream& operator<<(QTextStream& out, const LookDirection& direction);
QTextStream& operator<<(QTextStream& out, const HLook& l);


#endif