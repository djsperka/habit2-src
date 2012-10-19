/*
 *  HLooker.cpp
 *  hstate
 *
 *  Created by Oakes Lab on 6/13/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HLooker.h"
#include "HElapsedTimer.h"
#include <QtDebug>

HLooker::HLooker(int minlooktime_ms, int minlookawaytime_ms) : 
m_bLive(true), m_indexAt(0), m_minLookTimeMS(minlooktime_ms), m_minLookAwayTimeMS(minlookawaytime_ms), m_bLookStarted(false), m_direction(NoLook), m_bLookAwayStarted(false)
{
	m_ptimer = new QTimer();
	m_ptimer->setInterval(minlookawaytime_ms);
	m_ptimer->setSingleShot(true);
	QObject::connect(m_ptimer, SIGNAL(timeout()), this, SLOT(timeout()));
};


void HLooker::addTrans(LookTransType type, int tMS)
{
	m_transitions.append(qMakePair(type, tMS));
	update();
	return;
}

LookDirection HLooker::directionTo(LookTransType type)
{
	LookDirection ld = NoLook;
	switch (type) {
		case NoneLeft:
			ld = LookLeft;
			break;
		case NoneRight:
			ld = LookRight;
			break;
		case NoneCenter:
			ld = LookCenter;
			break;
		case LeftNone:
		case CenterNone:
		case RightNone:
		case NoneNone:
		case UnknownLookTrans:
			break;
	}
	return ld;
}


bool HLooker::isTransToLook(LookTransType type)
{
	LookDirection d = directionTo(type);
	return (d==LookLeft || d==LookCenter || d==LookRight);
}



void HLooker::update()
{
	int tMS;
	LookTransType trans;
	
	while (m_indexAt < m_transitions.count())
	{
		trans = m_transitions[m_indexAt].first;
		tMS = m_transitions[m_indexAt].second;
		
		qDebug() << "update(): index, trans, t = " << m_indexAt << ", " << trans << ", " << tMS;
		
		if (!m_bLookStarted)
		{
			if (isTransToLook(trans))
			{
				m_bLookStarted = true;
				m_lookStartTimeMS = tMS;
				m_lookStartIndex = m_indexAt;
				m_direction = directionTo(trans);
				
				qDebug() << "update(): Look started " << m_direction;
			}
			m_indexAt++;
		}
		else 
		{
			if (!m_bLookAwayStarted)
			{
				if (!isTransToLook(trans))
				{
					m_bLookAwayStarted = true;
					m_lookAwayStartTimeMS = tMS;
					m_lookAwayStartIndex = m_indexAt;
					qDebug() << "update(): Look away started, pending look " << m_direction;
				}
				
				// TODO: may need to assert that this trans is to look-away. Not sure
				// if we can have anything but that without an error. 
				
				m_indexAt++;
			}
			else 
			{
				if ((tMS - m_lookAwayStartTimeMS) >= m_minLookAwayTimeMS)
				{
					qDebug() << "update(): min look away time exceeded";
					if ((m_lookAwayStartTimeMS - m_lookStartTimeMS) >= m_minLookTimeMS)
					{
						// new look
						HLook l(m_direction, m_lookStartTimeMS, m_lookAwayStartTimeMS);
						m_looks.append(l);
						m_bLookStarted = false;
						m_bLookAwayStarted = false;
						m_indexAt = m_lookAwayStartIndex + 1;
						qDebug() << "update(): look complete " << m_direction;
						emit look(l);						
					}
					else 
					{
						// last look wasn't a full look. Start over. 
						m_bLookStarted = false;
						m_bLookAwayStarted = false;
						m_indexAt = m_lookAwayStartIndex + 1;
						
						qDebug() << "update(): not a full look.";
					}
				}
				else 
				{
					qDebug() << "update(): min look away time not exceeded " << tMS << "-" << m_lookAwayStartTimeMS << "=" << tMS - m_lookAwayStartTimeMS;
					// not a full look-away. If its a look-back to same direction as original look, 
					// then erase the look-away and resume the look. Otherwise, start a new look.
					if (directionTo(trans) == m_direction)
					{
						m_bLookAwayStarted = false;
						m_indexAt++;
						
						qDebug() << "update(): look-back";
					}
					else 
					{
						m_bLookAwayStarted = false;
						m_bLookStarted = true;
						m_lookStartTimeMS = tMS;
						m_direction = directionTo(trans);
						m_indexAt++;
						
						qDebug() << "update(): new look started " << m_direction;
					}
				}
			}
		}
	}
	
	/* 
	 * If a look away has started then start the timer
	 */
	
	if (m_bLookAwayStarted && !m_ptimer->isActive())
	{
		int t = m_minLookAwayTimeMS - (HElapsedTimer::elapsed() - m_lookAwayStartTimeMS);
		m_ptimer->start(t);
	}
	else if (!m_bLookAwayStarted && m_ptimer->isActive())
	{
		m_ptimer->stop();
	}
	return;
}

void HLooker::timeout()
{
	if (m_bLookAwayStarted)
	{
		addTrans(NoneNone, HElapsedTimer::elapsed());
	}
	return;
}

