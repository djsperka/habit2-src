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

#include <QObject>
#include <QList>
#include <QTimer>
#include <QTextStream>
#include "HLook.h"
#include "HEventLog.h"


class HLooker : public QObject
{
	Q_OBJECT

public:
	HLooker(int minlooktime_ms, int minlookawaytime_ms, HEventLog& log);
	~HLooker() {};
	void addTrans(LookTransType type, int tMS);	
	
private:
	int m_minLookTimeMS;
	int m_minLookAwayTimeMS;
	HEventLog& m_log;
	bool m_bLive;
	int m_indexAt;
	bool m_bLookStarted;
	LookDirection m_direction;
	bool m_bLookAwayStarted;

	QTimer *m_ptimer;
	int m_lookStartIndex;
	int m_lookStartTimeMS;
	int m_lookAwayStartIndex;
	int m_lookAwayStartTimeMS;
	QList< QPair<LookTransType, int> > m_transitions;
	QList< HLook > m_looks;
	
	LookDirection directionTo(LookTransType type);
	bool isTransToLook(LookTransType type);
	void update();
	
private slots:
	void timeout();

signals:
	void look(HLook l);
};
	

	

#endif
