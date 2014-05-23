/*
 *  HQEvents.h
 *  habutil
 *
 *  Created by Oakes Lab on 2/1/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HQEVENTS_H_
#define _HQEVENTS_H_


#include <QEvent>
class HEvent;
class HLookTrans;


struct HAllTrialsDoneEvent : public QEvent
{
	HAllTrialsDoneEvent() : QEvent(Type(AllTrialsDoneType)) {};
	~HAllTrialsDoneEvent() {};
	enum { AllTrialsDoneType = QEvent::User + 1 };
};

struct HNewTrialEvent : public QEvent
{
	HNewTrialEvent() : QEvent(Type(NewTrialType)) {};
	~HNewTrialEvent() {};
	enum { NewTrialType = QEvent::User + 2 };
};

struct HAbortTrialQEvent : public QEvent
{
	HAbortTrialQEvent() : QEvent(Type(AbortTrialType)) {};
	~HAbortTrialQEvent() {};
	enum { AbortTrialType = QEvent::User + 3 };
};

class HReliabilityQEvent : public QEvent
{
public:
	HReliabilityQEvent(HEvent* phevent) : QEvent(Type(ReliabilityType)), m_phevent(phevent) {};
	~HReliabilityQEvent() {};
	enum { ReliabilityType = QEvent::User + 4 };
	HEvent* hevent() { return m_phevent; };
private:
	HEvent* m_phevent;
};

struct HReliabilityEndQEvent : public QEvent
{
	HReliabilityEndQEvent() : QEvent(Type(ReliabilityEndType)) {};
	~HReliabilityEndQEvent() {};
	enum { ReliabilityEndType = QEvent::User + 5 };
};

struct HNoLookQEvent : public QEvent
{
	HNoLookQEvent() : QEvent(Type(NoLookType)) {};
	~HNoLookQEvent() {};
	enum { NoLookType = QEvent::User + 6 };
};


class HTransToLookQEvent: public QEvent
{
public:
	HTransToLookQEvent(const HLookTrans& lt, int tMS): QEvent(Type(TransToLookType)), m_lt(lt), m_tMS(tMS) {};
	~HTransToLookQEvent() {};
	enum { TransToLookType = QEvent::User + 10 };
	const HLookTrans& trans() { return m_lt; };
	int tTrans() { return m_tMS; };
private:
	const HLookTrans& m_lt;
	int m_tMS;
};

class HTransToLookAwayQEvent: public QEvent
{
public:
	HTransToLookAwayQEvent(const HLookTrans& lt, int tMS): QEvent(Type(TransToLookAwayType)), m_lt(lt), m_tMS(tMS) {};
	~HTransToLookAwayQEvent() {};
	enum { TransToLookAwayType = QEvent::User + 11 };
	const HLookTrans& trans() { return m_lt; };
	int tTrans() { return m_tMS; };
private:
	const HLookTrans& m_lt;
	int m_tMS;
};

#endif
