/*
 *  HLooker.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/13/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _HLOOKER_H_
#define _HLOOKER_H_

#include <QStateMachine>
#include <QList>
#include <QTimer>
#include <QTextStream>
#include <QAbstractTransition>
#include "HLook.h"
#include "HEventLog.h"
#include "HState.h"
#include "HQEvents.h"


class HLooker : public QStateMachine
{
	Q_OBJECT

public:
	HLooker(int minlooktime_ms, int minlookawaytime_ms, int maxlookawaytime_ms, int maxaccumlooktime_ms, HEventLog& log, bool bLive = true);
	~HLooker() {};
	void addTrans(const HLookTrans& type, int tMS);
	
	// Assume that current Look/away status remains until tMS.
	// If m_bLookStarted==true, check if there is enough looking time to
	// constitute a look. If so, generate a look object, add to our list of
	// looks, and emit a look() signal.
	// Intended to be called when either
	// 1) min look away time exceeded, and we were in a look state. Leave HLooker
	//    in a look-away state, current to tMS.
	// or
	// 2) abort look monitoring. Trial is being ended for some reason, flush out
	//    any look or look-away if their respective min times have been exceeded.
	bool flush(int tMS);

	// Stop processing at the time tMS.
	void stopLooker(int tMS);

public slots:

	void minLookAwayTimeout();
	void maxAccumulatedLookTimeReached();
	void minLookTimeReached();

	// slots called when the individual states are entered. Handle things with these
	// slots rather than the states' onEntry slots, as it makes it simpler to have
	// access to the vars which track look/away status and times.
	void onInitialStateEntered();
	void onLookingStateEntered();
	void onLookingStateExited();
	void onLookingAwayStateEntered();
	void onLookingAwayStateExited();

	// slot called when state machine is stopped (i.e. when stop() is called).
	// Should always call the overloaded stop(int tMS) so a proper stop time is
	// used to flush

private:
	int m_minLookTimeMS;		// look must be at least this long
	int m_minLookAwayTimeMS;	// when look away has been at least this long, current look is ended.
	int m_maxLookAwayTimeMS;	// when >0, if looking away for this long emit maxLookAwayTime()
	int m_maxAccumulatedLookTimeMS; // when >0, when accumulated look time reaches this emit maxAccumulatedLookTime()
	HEventLog& m_log;
	bool m_bLive;				// if true, running with live observer. if false, pre-recorded transitions used.
	bool m_bLookPending;		// if true a look has been started but not yet ended.
	const HLookDirection* m_pdirectionLookPending;	// when m_bLookStarted==true this is the direction
	int m_iLookPendingStartIndex;	// only valid if (m_bLookPending); in that case m_transitions[m_iLookPendingStartIndex]
									// is the initial transition in this looking period.
	bool m_bLookAwayStarted;	// if true, look-away has been started. m_bLookStarted may also be true, indicating
								// that the look may end, depending on how long this lasts

	// These are various tallies and counters that I'd like to do away with.
	//int m_lookStartTimeMS;		// initial start time for a look
	//int m_lookLastStartTimeMS;	// start time for most recent looking period (if short look-aways)
	//int m_cumulativeLookTimeMS;	// cumulative total look time for current look
	//int m_lookAwayStartTimeMS;	// initial start time for look-away
	//int m_cumulativeLookAwayTimeMS;	// cumulative total look time for a look-away.

	// Timers used
	QTimer *m_ptimerLookAwayTimeout;
	QTimer *m_ptimerMaxLookAway;
	QTimer *m_ptimerMaxAccumulatedLook;
	QTimer *m_ptimerMinLookTime;

	// List of all transitions. Starts from scratch each time initial state entered.
	QList< QPair<const HLookTrans*, int> > m_transitions;
	QList< HLook > m_looks;
	
	bool analyzeLooking(int m_iLookPendingStartIndex, int& lookStartTimeMS, int& lastLookStartTimeMS, int& cumulativeLookTimeMS, int& lastLookAwayStartTimeMS);
	int getSumOfCompleteLooks();

signals:
	void look(HLook l);
	void lookStarted();
	//void lookAway(HLook l);
	void lookAwayStarted();
	void maxLookAwayTime();
	void maxAccumulatedLookTime();
};
	
class HLookerState: public QState
{
	Q_OBJECT

public:
	HLookerState(HLooker& looker, QState* parent = NULL): QState(parent), m_looker(looker) {};
	virtual ~HLookerState() {};
	HLooker& looker() { return m_looker; };
private:
	HLooker& m_looker;
};

class HLookerInitialState: public HLookerState
{
public:
	HLookerInitialState(HLooker& looker, QState* parent = NULL): HLookerState(looker, parent) {};
	virtual ~HLookerInitialState() {};
	virtual void onEntry(QEvent* event);
};

class HLookingState: public HLookerState
{
public:
	HLookingState(HLooker& looker, QState* parent = NULL): HLookerState(looker, parent) {};
	virtual ~HLookingState() {};
	virtual void onEntry(QEvent* event);
};
	
class HLookingAwayState: public HLookerState
{
public:
	HLookingAwayState(HLooker& looker, QState* parent = NULL): HLookerState(looker, parent) {};
	virtual ~HLookingAwayState() {};
	virtual void onEntry(QEvent* event);
};

class HTransToLookTransition: public QAbstractTransition
{
public:
	HTransToLookTransition() {};
	~HTransToLookTransition() {};
protected:
	virtual bool eventTest(QEvent *event)
	{
		return (event->type() == QEvent::Type(HTransToLookQEvent::TransToLookType));
	};
	virtual void onTransition(QEvent* event)
	{
		Q_UNUSED(event);
	};
};

class HTransToLookAwayTransition: public QAbstractTransition
{
public:
	HTransToLookAwayTransition() {};
	~HTransToLookAwayTransition() {};
protected:
	virtual bool eventTest(QEvent *event)
	{
		return (event->type() == QEvent::Type(HTransToLookAwayQEvent::TransToLookAwayType));
	};
	virtual void onTransition(QEvent* event)
	{
		Q_UNUSED(event);
	};
};

#endif
