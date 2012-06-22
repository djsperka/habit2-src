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
	HLook(LookDirection direc, int startMS, int endMS): m_direction(direc), m_startMS(startMS), m_endMS(endMS) {};
	~HLook() {};
	LookDirection direction() const { return m_direction; };
	int startMS() const { return m_startMS; };
	int endMS() const { return m_endMS; };
private:
	LookDirection m_direction;
	int m_startMS;
	int m_endMS;
};

QTextStream& operator<<(QTextStream& out, const LookTransType& type);
QTextStream& operator<<(QTextStream& out, const LookDirection& direction);
QTextStream& operator<<(QTextStream& out, const HLook& l);


#endif