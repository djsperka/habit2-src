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
#include <QDebug>

enum LookTransType { UnknownLookTrans=-1, NoneLeft, LeftNone, NoneCenter, CenterNone, NoneRight, RightNone, NoneNone};
enum LookDirection { UnknownLookDirection=-1, NoLook, LookLeft, LookRight, LookCenter};

class HLook
{
public:
	HLook(LookDirection direc, int startMS, int endMS): m_direction(direc), m_startMS(startMS), m_endMS(endMS) {};
	HLook() : m_direction(UnknownLookDirection), m_startMS(0), m_endMS(0) {};
	HLook(const HLook& h) 
	{
		m_direction = h.direction();
		m_startMS = h.startMS();
		m_endMS = h.endMS();
	};
	~HLook() {};
	void setDirection(LookDirection d) { m_direction = d; };
	void setStartMS(int ms) { m_startMS = ms; };
	void setEndMS(int ms) { m_endMS = ms; };
	LookDirection direction() const { return m_direction; };
	int startMS() const { return m_startMS; };
	int endMS() const { return m_endMS; };
	int lookMS() const { return m_endMS - m_startMS; };

	// TODO These do not need to be friends. Use member functions!
	friend QDataStream& operator<<(QDataStream& out, const HLook& l);
	friend QDataStream& operator>>(QDataStream& out, HLook& l);
	friend bool operator==(const HLook& lhs, const HLook& rhs);
private:
	LookDirection m_direction;
	int m_startMS;
	int m_endMS;
};

Q_DECLARE_METATYPE(HLook);

QTextStream& operator<<(QTextStream& out, const LookTransType& type);
QTextStream& operator<<(QTextStream& out, const LookDirection& direction);
QTextStream& operator<<(QTextStream& out, const HLook& l);

QDataStream& operator<<(QDataStream& out, const LookTransType& type);
QDataStream& operator<<(QDataStream& out, const LookDirection& direction);
QDataStream& operator<<(QDataStream& out, const HLook& l);

QDataStream& operator>>(QDataStream& out, LookTransType& type);
QDataStream& operator>>(QDataStream& out, LookDirection& direction);
QDataStream& operator>>(QDataStream& out, HLook& l);

bool operator==(const HLook& lhs, const HLook& rhs);

QDebug operator<<(QDebug dbg, const LookTransType& type);
QDebug operator<<(QDebug dbg, const LookDirection& direction);
QDebug operator<<(QDebug dbg, const HLook& l);

#endif
