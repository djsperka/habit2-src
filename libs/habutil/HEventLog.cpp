/*
 *  HEventLog.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/31/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HEventLog.h"
#include <QFile>
#include <QTextStream>

HEventLog::~HEventLog()
{
//	while (!isEmpty())
//		delete takeFirst();
};

HPhaseLog HEventLog::getPhaseLog(QString phase)
{
	bool bHavePhaseStart=false;
	HPhaseLog phaselog;
	QListIterator<HEvent*> events(*this);
	// Move iterator to phase start event
	if (phase == "")
	{
		events.toBack();
		while (events.hasPrevious())
		{
			HEvent* e = events.previous();
			if (e->type() == HEventType::HEventPhaseStart)
			{
				bHavePhaseStart = true;
				events.next();			// leave iterator after the phase start event
				break;
			}
		}
	}
	else 
	{
		events.toFront();
		while (events.hasNext())
		{
			HEvent* e = events.next();
			if (e->type() == HEventType::HEventPhaseStart)
			{
				HPhaseStartEvent* pse = static_cast<HPhaseStartEvent*>(e);
				if (pse->phase() == phase)
				{
					bHavePhaseStart = true;
					break;
				}
			}
		}
	}
	
	// Now if we found start of phase, get the look times
	if (bHavePhaseStart)
	{
		bool bInterTrial = true;
		bool bPhaseEnd = false;
		int iTotal = 0;
		HEvent* pevent;
		while (events.hasNext() && !bPhaseEnd)
		{
			pevent = events.next();
			if (bInterTrial)
			{
				// if we are between trials, looking for 
				// TrialStart event
				if (pevent->type() == HEventType::HEventTrialStart)
				{
					bInterTrial = false;
					iTotal = 0;
				}
				else if (pevent->type() == HEventType::HEventPhaseEnd)
				{
					bPhaseEnd = true;
				}
			}
			else 
			{
				// if we are not between trials, then we are in one.
				// We are looking for look events and TrialEnd events. 
				// We select only the "successful" trial end types for tallying
				// looks.
				if (pevent->type() == HEventType::HEventTrialEnd)
				{
					HTrialEndEvent* pte = static_cast<HTrialEndEvent*>(pevent);
					if (	pte->endtype() == HTrialEndType::HTrialEndGotLook ||
							pte->endtype() == HTrialEndType::HTrialEndMaxStimulusTime ||
							pte->endtype() == HTrialEndType::HTrialEndMaxAccumulatedLookTime ||
							pte->endtype() == HTrialEndType::HTrialEndMaxLookAwayTime ||
							pte->endtype() == HTrialEndType::HTrialEndStimAbort)
					{
						phaselog.append(iTotal);
					}
					iTotal = 0;
					bInterTrial = true;
				}
				else if (pevent->type() == HEventType::HEventLook)
				{
					HLookEvent* plook = static_cast<HLookEvent*>(pevent);
					if (plook->look().direction() != HLookDirection::LookAway)
					{
						iTotal += plook->look().lookMS();
					}
				}
			}
		}
	}
	return phaselog;
}


bool HEventLog::saveToCSV(QString& filename) const
{
	bool b = false;
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream out(&file);
		QListIterator<HEvent*> events(*this);
		events.toFront();
		while (events.hasNext())
		{
			HEvent* e = events.next();
			out << e->eventCSV() << endl;
		}
		file.close();
		b = true;
	}					
	return b;
}

bool HMutableEventLogIterator::skipToEventType(const HEventType& type)
{
	bool b = false;
	while (!b && this->hasNext())
	{
		HEvent* e = this->peekNext();
		if (e->type() == type)
		{
			b = true;	// found what we were looking for
		}
		else
		{
			this->next();
		}
	}
	return b;
}

QDataStream& operator<<(QDataStream& stream, HEventLog log)
{
	stream << log.size();
	QListIterator<HEvent *> it(log);
	while (it.hasNext())
	{
		HEvent* event = it.next();
		stream << *event;
	}
	return stream;
}

QDataStream& operator>>(QDataStream& stream, HEventLog& log)
{
	// TODO: Should operator>> first clear() the log?
	int n, i;
	stream >> n;
	for (i=0; i<n; i++)
	{
		HEvent* event = HEvent::getEvent(stream);
		log.append(event);
	}
	return stream;
}

bool operator==(const HEventLog& log0, const HEventLog& log1)
{
	bool b = false;
	if (log0.size() == log1.size())
	{
		QListIterator<HEvent *> it0(log0);
		QListIterator<HEvent *> it1(log1);
		b = true;
		while (it0.hasNext() && it1.hasNext())
		{
			HEvent* e0 = it0.next();
			HEvent* e1 = it1.next();
			if (e0->eventCSV() != e1->eventCSV())
			{
				b = false;
				break;
			}
		}
	}
	return b;
}
