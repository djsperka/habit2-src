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

struct HAbortTrialEvent : public QEvent
{
	HAbortTrialEvent() : QEvent(Type(AbortTrialType)) {};
	~HAbortTrialEvent() {};
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

#endif
