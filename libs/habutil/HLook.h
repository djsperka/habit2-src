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

#include "HTypes.h"
#include <QTime>
#include <QTextStream>
#include <QDebug>
#include <QList>

//enum LookTransType { UnknownLookTrans=-1, NoneLeft, LeftNone, NoneCenter, CenterNone, NoneRight, RightNone, NoneNone};
//enum LookDirection { UnknownLookDirection=-1, NoLook, LookLeft, LookRight, LookCenter};

class HLook
{
public:
	HLook(const HLookDirection& direc, int startMS, int endMS): m_pdirection(&direc), m_startMS(startMS), m_endMS(endMS) { m_lookMS = endMS-startMS; };
	HLook(const HLookDirection& direc, int startMS, int endMS, int lookMS): m_pdirection(&direc), m_startMS(startMS), m_endMS(endMS), m_lookMS(lookMS) {};
	HLook() : m_pdirection(&HLookDirection::UnknownLookDirection), m_startMS(0), m_endMS(0), m_lookMS(0) {};
	HLook(const HLook& h) 
	{
		m_pdirection = &h.direction();
		m_startMS = h.startMS();
		m_endMS = h.endMS();
		m_lookMS = h.lookMS();
		m_sublooks = h.sublooks();
	};
	~HLook() {};
	void setDirection(const HLookDirection& d) { m_pdirection = &d; };
	void setStartMS(int ms) { m_startMS = ms; };
	void setEndMS(int ms) { m_endMS = ms; };
	void setLookMS(int ms) { m_lookMS = ms; };
	void setSublooks(const QList<HLook>& sublooks) { m_sublooks = sublooks; };
	const HLookDirection& direction() const { return *m_pdirection; };
	int startMS() const { return m_startMS; };
	int endMS() const { return m_endMS; };
	int lookMS() const { return m_lookMS; };
	const QList<HLook>& sublooks() const { return m_sublooks; };

	// TODO These do not need to be friends. Use member functions!
	//friend QDataStream& operator<<(QDataStream& out, const HLook& l);
	//friend QDataStream& operator>>(QDataStream& out, HLook& l);
	//friend bool operator==(const HLook& lhs, const HLook& rhs);
private:
	const HLookDirection* m_pdirection;
	int m_startMS;
	int m_endMS;
	int m_lookMS;
	QList<HLook> m_sublooks;
};

typedef QList<HLook> HLookList;


Q_DECLARE_METATYPE(HLook);

bool isLookToAnyStimulus(const HLook& look);

QTextStream& operator<<(QTextStream& out, const HLookTrans& type);
QTextStream& operator<<(QTextStream& out, const HLookDirection& direction);
QTextStream& operator<<(QTextStream& out, const HLook& l);

//QDataStream& operator<<(QDataStream& out, const LookTransType& type);
//QDataStream& operator<<(QDataStream& out, const LookDirection& direction);
QDataStream& operator<<(QDataStream& out, const HLook& l);

//QDataStream& operator>>(QDataStream& out, LookTransType& type);
//QDataStream& operator>>(QDataStream& out, LookDirection& direction);
QDataStream& operator>>(QDataStream& out, HLook& l);

bool operator==(const HLook& lhs, const HLook& rhs);

QDebug operator<<(QDebug dbg, const HLookTrans& type);
QDebug operator<<(QDebug dbg, const HLookDirection& direction);
QDebug operator<<(QDebug dbg, const HLook& l);
QDebug operator<<(QDebug dbg, const QList<HLook>& looklist);
#endif
