/*
 *  HEvents.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/7/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HEVENTS_H_
#define _HEVENTS_H_

#include <QEvent>
#include <QEventTransition>

class HAGTrialEvent : public QEvent
{
public:
	HAGTrialEvent() : QEvent(QEvent::Type(QEvent::User+2))
	{}
};

class HNoAGTrialEvent : public QEvent
{
public:
	HNoAGTrialEvent() : QEvent(QEvent::Type(QEvent::User+3))
	{}
};


class HAGTrialTransition : public QAbstractTransition
{
public:
	HAGTrialTransition() {}
	
protected:
	virtual bool eventTest(QEvent *e) 
	{
		return (e->type() == QEvent::User+2);
	}
	
	virtual void onTransition(QEvent* e) { Q_UNUSED(e); };
};

#endif