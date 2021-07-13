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
	HLooker(HEventLog& log, bool bLive = true);
	HLooker(const HLooker& looker);
	~HLooker() {};

	// by default the looker is live (i.e. it will use timers).
	// You can only turn it off when looker state machine is not running.
	bool isLive() const { return m_bIsLive; };

	// Add a transition. A new event is posted to the state machine.
	void addTrans(const HLookTrans& type, int tMS);

	// Change parameters of the look detector. These must be set before starting the state machine!
	bool setIsInclusiveLookTime(bool b);
	bool setMinLookTime(int t);
	bool setMinLookAwayTime(int t);
	bool setMaxAccumulatedLookTime(int tMaxAccum);
	bool setMaxLookAwayTime(int tMaxLookAway);

	bool setPhaseAccumulatedLookTime(int tPhaseAccum);
	
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
	//bool flush(int tMS);

	// Stop processing at the time tMS.
	void stopLooker(int tMS);

public slots:

	void minLookAwayTimeout();
	void maxAccumulatedLookTimeReached();
	void minLookTimeReached();
	void maxLookAwayTimeout();
	void phaseAccumulatedLookTimeReached();

	// slots called when the individual states are entered. Handle things with these
	// slots rather than the states' onEntry slots, as it makes it simpler to have
	// access to the vars which track look/away status and times.
	void onInitialStateEntered();
	void onLookingStateEntered();
	void onLookingStateExited();
	void onLookingAwayStateEntered();
	void onLookingAwayStateExited();
	void onStarted();
	void onRunningChanged(bool);

	// slot called when state machine is stopped (i.e. when stop() is called).
	// Should always call the overloaded stop(int tMS) so a proper stop time is
	// used to flush
protected:
	HEventLog& log() { return m_log; };

private:
	bool m_bIsLive;				// if true, then timers will be used, as if in real experiment.
	int m_minLookTimeMS;		// look must be at least this long
	int m_minLookAwayTimeMS;	// when look away has been at least this long, current look is ended.
	int m_maxLookAwayTimeMS;	// when >0, if looking away for this long emit maxLookAwayTime()
	int m_maxAccumulatedLookTimeMS; // when >0, when accumulated look time reaches this emit maxAccumulatedLookTime()
	int m_phaseAccumulatedLookTimeMS; // when >0, when accumulated look time reaches this emit phaseAccumulatedLookTime(). Phase should manipulate this between trials.
	HEventLog& m_log;
	bool m_bInclusiveLookTime;	// if true (default), short look-aways are included in total look time.
	bool m_bLookStarted;		// if true a look has been started but not yet ended.
	const HLookDirection* m_pdirectionLookStarted;	// when m_bLookStarted==true this is the direction
	int m_iLookStartedIndex;	// only valid if (m_bLookStarted); in that case m_transitions[m_iLookStartedIndex]
									// is the initial transition in this looking period.
	bool m_bLookAwayStarted;	// if true, look-away has been started. m_bLookStarted may also be true, indicating
								// that the look may end, depending on how long this lasts


	// Timers used
	QTimer *m_ptimerMinLookTime;		// m_minLookTimeMS
	QTimer *m_ptimerMinLookAwayTime;	// m_minLookAwayTimeMS
	QTimer *m_ptimerMaxLookAway;		// m_maxLookAwayTimeMS
	QTimer *m_ptimerMaxAccumulatedLook;	// m_maxAccumulatedLookTimeMS
	QTimer *m_ptimerPhaseAccumulatedLook; // m_phaseAccumulatedLookTimeMS

	// List of all transitions. Starts from scratch each time initial state entered.
	QList< QPair<const HLookTrans*, int> > m_transitions;
	QList< HLook > m_looks;
	
	/// Starting with the transition at m_transitions[index], determine the status of "looking" as of the end of m_transitions.
	// The analysis only sums the time spent in looking states and makes no attempt to account for the min look-away time (which
	// can and a look) or the actual direction looking is in. It merely sums up the time spent looking at any target (see
	// HLookTrans::isTransToLook()).
	// sets the value of the last 4 args:
	// lookStartTimeMS: the time of the first transition to a look direction
	// lastLookStartTimeMS: the time of the most recent transition to a look direction
	// cumulativeLookTimeMS: the total time in a looking state (excludes time looking away)
	// lastLookAwayStartTimeMS: the time of the most recent transition to a look-away direction.
	bool analyzeLooking(int m_iLookPendingStartIndex, int& lookStartTimeMS, int& lastLookStartTimeMS, int& cumulativeLookTimeMS, int& lastLookAwayStartTimeMS, HLookList& sublist);
	int getSumOfCompleteLooks();

signals:
	void look(HLook l);
	void lookStarted();	// indicates that looking at a target has begun
	void lookPending();	// looking at a target for at least the min look time
	void lookAborted(HLook l);	// a look that started has ended before becoming a complete look
	void lookingDirection(QString);
	//void lookAwayStarted();
	void maxLookAwayTime();
	void maxAccumulatedLookTime();
	void phaseAccumulatedLookTime();	// if/when the looking time in m_phaseAccumulatedLookTimeMS is hit
};
	
class HLookerState: public QState
{
//	Q_OBJECT

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
	//virtual void onEntry(QEvent* event);
};

class HLookingState: public HLookerState
{
public:
	HLookingState(HLooker& looker, QState* parent = NULL): HLookerState(looker, parent) {};
	virtual ~HLookingState() {};
	//virtual void onEntry(QEvent* event);
};
	
class HLookingAwayState: public HLookerState
{
public:
	HLookingAwayState(HLooker& looker, QState* parent = NULL): HLookerState(looker, parent) {};
	virtual ~HLookingAwayState() {};
	//virtual void onEntry(QEvent* event);
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
