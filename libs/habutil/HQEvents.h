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


struct HNoLookQEvent : public QEvent
{
	HNoLookQEvent() : QEvent(Type(NoLookType)) {};
	~HNoLookQEvent() {};
	enum { NoLookType = QEvent::User + 6 };
};

struct HMaxLookAwayTimeQEvent : public QEvent
{
	HMaxLookAwayTimeQEvent() : QEvent(Type(MaxLookAwayTimeType)) {};
	~HMaxLookAwayTimeQEvent() {};
	enum { MaxLookAwayTimeType = QEvent::User + 4 };
};

struct HGotLookQEvent : public QEvent
{
	HGotLookQEvent() : QEvent(Type(GotLookType)) {};
	~HGotLookQEvent() {};
	enum { GotLookType = QEvent::User + 5 };
};

struct HMaxAccumulatedLookTimeQEvent : public QEvent
{
	HMaxAccumulatedLookTimeQEvent() : QEvent(Type(MaxAccumLookTimeType)) {};
	~HMaxAccumulatedLookTimeQEvent() {};
	enum { MaxAccumLookTimeType = QEvent::User + 7 };
};

struct HPhaseAccumulatedLookTimeQEvent : public QEvent
{
	HPhaseAccumulatedLookTimeQEvent() : QEvent(Type(PhaseAccumulatedLookTimeType)) {};
	~HPhaseAccumulatedLookTimeQEvent() {};
	enum { PhaseAccumulatedLookTimeType = QEvent::User + 8 };
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
