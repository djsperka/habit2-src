/*
 *  HEvents.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/31/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include "HEvents.h"
#include <QtDebug>

/* 
 * Constants for event types and trial end types 
 */
 
const HEventType HEventType::HEventPhaseStart(0, "PhaseStart");
const HEventType HEventType::HEventPhaseEnd(1, "PhaseEnd");
const HEventType HEventType::HEventTrialStart(2, "TrialStart");
const HEventType HEventType::HEventTrialEnd(3, "TrialEnd");
const HEventType HEventType::HEventAGRequest(4, "AGRequest");
const HEventType HEventType::HEventAGStart(5, "AGStart");
const HEventType HEventType::HEventAGEnd(6, "AGEnd");
const HEventType HEventType::HEventStimRequest(7, "StimRequest");
const HEventType HEventType::HEventStimStart(8, "StimStart");
const HEventType HEventType::HEventStimEnd(9, "StimEnd");
const HEventType HEventType::HEventStimulusSettings(10, "StimulusSettings");
const HEventType HEventType::HEventStimulusInfo(11, "StimulusInfo");
const HEventType HEventType::HEventAttention(12, "Attention");
const HEventType HEventType::HEventLook(13, "Look");
const HEventType HEventType::HEventLookTrans(14, "LookTrans");
const HEventType HEventType::HHabituationSuccess(15, "HabituationSuccess");
const HEventType HEventType::HHabituationFailure(16, "HabituationFailure");
const HEventType HEventType::HEventUndefined(-1, "Undefined");

// Note undefined event not in search array.
const HEventType* HEventType::A[17] =
{
	&HEventType::HEventPhaseStart,
	&HEventType::HEventPhaseEnd,
	&HEventType::HEventTrialStart,
	&HEventType::HEventTrialEnd,
	&HEventType::HEventAGRequest,
	&HEventType::HEventAGStart,
	&HEventType::HEventAGEnd,
	&HEventType::HEventStimRequest,
	&HEventType::HEventStimStart,
	&HEventType::HEventStimEnd,
	&HEventType::HEventStimulusSettings,
	&HEventType::HEventStimulusInfo,
	&HEventType::HEventAttention,
	&HEventType::HEventLook,
	&HEventType::HEventLookTrans,
	&HEventType::HHabituationSuccess,
	&HEventType::HHabituationFailure
};
	
bool operator==(const HEventType& lhs, const HEventType& rhs)
{
	return lhs.number() == rhs.number();
};

const HEventType& getHEventType(int number_value)
{
	const HEventType* t = &HEventType::HEventUndefined;
	for (unsigned int i=0; i<sizeof(HEventType::A)/sizeof(HEventType*) && t==&HEventType::HEventUndefined; i++)
		if (HEventType::A[i]->number() == number_value) t = HEventType::A[i];
	return *t;
};


// These are supplied when creating HTrialEndEvent

const HTrialEndType HTrialEndType::HTrialEndGotLook(0, "GotLook");
const HTrialEndType HTrialEndType::HTrialEndFixedTimeout(1, "FixedTimeout");
const HTrialEndType HTrialEndType::HTrialEndNoLookTimeout(2, "NoLookTimeout");
const HTrialEndType HTrialEndType::HTrialEndAbort(3, "Abort");
const HTrialEndType HTrialEndType::HTrialEndUndefined(-1, "Unknown");
	
const HTrialEndType* HTrialEndType::A[4] = 
{
	&HTrialEndType::HTrialEndGotLook, 
	&HTrialEndType::HTrialEndFixedTimeout,
	&HTrialEndType::HTrialEndNoLookTimeout, 
	&HTrialEndType::HTrialEndAbort
};
	
bool operator==(const HTrialEndType& lhs, const HTrialEndType& rhs)
{
	return lhs.number() == rhs.number();
};
	

const HTrialEndType& getHTrialEndType(int number_value)
{
	const HTrialEndType* t = &HTrialEndType::HTrialEndUndefined;
	for (unsigned int i=0; i<sizeof(HTrialEndType::A)/sizeof(HTrialEndType*) && t==&HTrialEndType::HTrialEndUndefined; i++)
		if (HTrialEndType::A[i]->number() == number_value) t = HTrialEndType::A[i];
	return *t;
};







// Stream operators for supporting types

QTextStream& operator<<(QTextStream& out, const HEventType& type)
{
	out << type.name();
	return out;
};

QTextStream& operator<<(QTextStream& out, const HTrialEndType& etype)
{
	out << etype.name();
	return out;
};

QTextStream& operator<<(QTextStream& out, const HEvent& e)
{
	out << qSetFieldWidth(7) << right << e.timestamp() << " " << qSetFieldWidth(20) << left << e.type() << " " << qSetFieldWidth(0) << e.eventInfo() << endl;
	return out;
};







// Pretty-print text output: eventInfo()
// CSV output: 

QString HEvent::eventCSV() const
{
	QString s = QString("%1,%2,%3").arg(this->type().name()).arg(this->type().number()).arg(this->timestamp());
	QString sa = this->eventCSVAdditional();
	if (sa.length() > 0)
	{
		s.append(",").append(sa);
	}
	return s;
}

QString HEvent::eventCSVAdditional() const		// default behavior
{
	return QString("");
}

QDataStream& operator<<(QDataStream& out, const HEvent& e)
{
	out << e.type().number() << e.timestamp();
	e.putAdditional(out);
	return out;
}

QDataStream& HEvent::putAdditional(QDataStream& stream) const
{
	return stream;
}

bool operator==(const HEvent& lhs, const HEvent& rhs)
{
#if 0
	qDebug() << "lhs " << lhs.eventCSV();
	qDebug() << "rhs " << rhs.eventCSV();
	qDebug() << "test " << (lhs.eventCSV() == rhs.eventCSV());
#endif
	return (lhs.eventCSV() == rhs.eventCSV());
}

HEvent* HEvent::getEvent(QDataStream& stream)
{
	HEvent* pevent = NULL;
	int n, ts;
	stream >> n >> ts;
	const HEventType& etype = getHEventType(n);
	if (etype == HEventType::HEventPhaseStart)
	{
		pevent = HPhaseStartEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventPhaseEnd)
	{
		pevent = HPhaseEndEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventTrialStart)
	{
		pevent = HTrialStartEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventTrialEnd)
	{
		pevent = HTrialEndEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventAGRequest)
	{
		pevent = HAGRequestEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventAGStart)
	{
		pevent = HAGStartEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventStimRequest)
	{
		pevent = HStimRequestEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventStimStart)
	{
		pevent = HStimStartEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventStimulusSettings)
	{
		pevent = HStimulusSettingsEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventAttention)
	{
		pevent = HAttentionEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventLook)
	{
		pevent = HLookEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HEventLookTrans)
	{
		pevent = HLookTransEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HHabituationSuccess)
	{
		pevent = HHabituationSuccessEvent::getEvent(stream, ts);
	}
	else if (etype == HEventType::HHabituationFailure)
	{
		pevent = HHabituationFailureEvent::getEvent(stream, ts);
	}
	else
	{
		// this is an error.

	}
	return pevent;
}

QDataStream& HPhaseStartEvent::putAdditional(QDataStream& stream) const
{
	stream << phasetype().number();
	return stream;
}

HPhaseStartEvent* HPhaseStartEvent::getEvent(QDataStream& stream, int timestamp)
{
	int n;
	stream >> n;
	const HPhaseType& phasetype = getPhaseType(n);
	return new HPhaseStartEvent(phasetype, timestamp);
}




QString HPhaseStartEvent::eventInfo() const 
{
	return phase();
};

QString HPhaseStartEvent::eventCSVAdditional() const
{
	return phase();
};

QString HPhaseEndEvent::eventInfo() const 
{
	return QString("Phase ended");
};

HPhaseEndEvent* HPhaseEndEvent::getEvent(QDataStream& stream, int timestamp)
{
	Q_UNUSED(stream);
	return new HPhaseEndEvent(timestamp);
}


QString HTrialStartEvent::eventInfo() const 
{
	return QString("Trial %1 repeat %2").arg(trialnumber()).arg(repeatnumber());
};

QString HTrialStartEvent::eventCSVAdditional() const
{
	return QString("%1,%2").arg(trialnumber()).arg(repeatnumber());
};

QDataStream& HTrialStartEvent::putAdditional(QDataStream& stream) const
{
	stream << trialnumber() << repeatnumber();
	return stream;
}

HTrialStartEvent* HTrialStartEvent::getEvent(QDataStream& stream, int timestamp)
{
	int trialnumber, repeatnumber;
	stream >> trialnumber >> repeatnumber;
	return new HTrialStartEvent(trialnumber, repeatnumber, timestamp);
}

QString HTrialEndEvent::eventInfo() const
{
	return endtype().name();
};

QString HTrialEndEvent::eventCSVAdditional() const
{
	return endtype().name();
};

QDataStream& HTrialEndEvent::putAdditional(QDataStream& stream) const
{
	stream << endtype().number();
	return stream;
}

HTrialEndEvent* HTrialEndEvent::getEvent(QDataStream& stream, int timestamp)
{
	int n;
	stream >> n;
	const HTrialEndType& endType = getHTrialEndType(n);
	return new HTrialEndEvent(endType, timestamp);
}


QString HStimRequestEvent::eventInfo() const
{
	return QString("Request stim index %1").arg(m_stimindex);
};

QString HStimRequestEvent::eventCSVAdditional() const
{
	return QString("%1").arg(m_stimindex);
};

QDataStream& HStimRequestEvent::putAdditional(QDataStream& stream) const
{
	stream << stimindex();
	return stream;
}

HStimRequestEvent* HStimRequestEvent::getEvent(QDataStream& stream, int timestamp)
{
	int stimid;
	stream >> stimid;
	return new HStimRequestEvent(stimid, timestamp);
}


QString HStimStartEvent::eventInfo() const
{
	return QString("Stim started on player %1").arg(m_playerid);
};

QString HStimStartEvent::eventCSVAdditional() const
{
	return QString("%1").arg(m_playerid);
};

QDataStream& HStimStartEvent::putAdditional(QDataStream& stream) const
{
	stream << playerid();
	return stream;
}

HStimStartEvent* HStimStartEvent::getEvent(QDataStream& stream, int timestamp)
{
	int stimid;
	stream >> stimid;
	return new HStimStartEvent(stimid, timestamp);
}


QString HAGRequestEvent::eventInfo() const
{
	return QString("AG Request");
}

HAGRequestEvent* HAGRequestEvent::getEvent(QDataStream& stream, int timestamp)
{
	Q_UNUSED(stream);
	return new HAGRequestEvent(timestamp);
}

QString HAGStartEvent::eventInfo() const
{
	return QString("AG started on player %1").arg(m_playerid);
};

QString HAGStartEvent::eventCSVAdditional() const
{
	return QString("%1").arg(m_playerid);
};

QDataStream& HAGStartEvent::putAdditional(QDataStream& stream) const
{
	stream << playerid();
	return stream;
}

HAGStartEvent* HAGStartEvent::getEvent(QDataStream& stream, int timestamp)
{
	int id;
	stream >> id;
	return new HAGStartEvent(id, timestamp);
}


QString HStimulusSettingsEvent::eventInfo() const
{
	return QString("Stim index: %1  Name: %2").arg(m_stimindex, 2).arg(m_settings.getName());
};

QString HStimulusSettingsEvent::eventCSVAdditional() const
{
	const Habit::StimulusInfo& left = settings().getLeftStimulusInfo();
	const Habit::StimulusInfo& center = settings().getCenterStimulusInfo();
	const Habit::StimulusInfo& right = settings().getRightStimulusInfo();
	const Habit::StimulusInfo& iss = settings().getIndependentSoundInfo();

	return QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23").
			arg(stimindex()).arg(settings().getId()).arg(settings().getName()).arg(settings().isLeftEnabled()).
			arg(settings().isCenterEnabled()).arg(settings().isRightEnabled()).arg(settings().isIndependentSoundEnabled()).
			arg(left.getName()).arg(left.getFileName()).arg(left.isLoopPlayBack()).arg(left.getAudioBalance()).
			arg(center.getName()).arg(center.getFileName()).arg(center.isLoopPlayBack()).arg(center.getAudioBalance()).
			arg(right.getName()).arg(right.getFileName()).arg(right.isLoopPlayBack()).arg(right.getAudioBalance()).
			arg(iss.getName()).arg(iss.getFileName()).arg(iss.isLoopPlayBack()).arg(iss.getAudioBalance());
};

QDataStream& HStimulusSettingsEvent::putAdditional(QDataStream& stream) const
{
	stream << stimindex() << settings();
	return stream;
}

HStimulusSettingsEvent* HStimulusSettingsEvent::getEvent(QDataStream& stream, int timestamp)
{
	Habit::StimulusSettings settings;
	int stimid;
	stream >> stimid >> settings;
	return new HStimulusSettingsEvent(settings, stimid, timestamp);
}

QString HStimulusInfoEvent::eventInfo() const
{
	return QString("Stim index: %1  File: %2 Loop: %3 Volume: %4").arg(m_stimindex).arg(m_info.getFileName()).arg(m_info.isLoopPlayBack()).arg(m_info.getAudioBalance());
};

QString HAttentionEvent::eventInfo() const
{
	return QString("");
};

HAttentionEvent* HAttentionEvent::getEvent(QDataStream& stream, int timestamp)
{
	Q_UNUSED(stream);
	return new HAttentionEvent(timestamp);
}

QString HLookEvent::eventInfo() const
{
	QString s;
	QTextStream tmp(&s);
	tmp << m_look;
	return s;
};


QDataStream& HLookEvent::putAdditional(QDataStream& stream) const
{
	stream << look();
	return stream;
}

HLookEvent* HLookEvent::getEvent(QDataStream& stream, int timestamp)
{
	HLook look;
	stream >> look;
	return new HLookEvent(look, timestamp);
}

QString HLookEvent::eventCSVAdditional() const
{
	QString s;
	QTextStream tmp(&s);
	tmp << m_look.direction() << "," << m_look.startMS() << "," << m_look.endMS() << "," << m_look.lookMS();
	return s;
}

QString HLookTransEvent::eventInfo() const
{
	QString s;
	QTextStream tmp(&s);
	tmp << transtype();
	return s;
};

QString HLookTransEvent::eventCSVAdditional() const
{
	QString s;
	QTextStream tmp(&s);
	tmp << transtype();
	return s;
};

QDataStream& HLookTransEvent::putAdditional(QDataStream& stream) const
{
	stream << transtype();
	return stream;
}

HLookTransEvent* HLookTransEvent::getEvent(QDataStream& stream, int timestamp)
{
	int type;
	stream >> type;
	return new HLookTransEvent((LookTransType)type, timestamp);
}

QString HHabituationSuccessEvent::eventInfo() const
{
	return QString("HabituationSuccess");
};

HHabituationSuccessEvent* HHabituationSuccessEvent::getEvent(QDataStream& stream, int timestamp)
{
	Q_UNUSED(stream);
	return new HHabituationSuccessEvent(timestamp);
}

QString HHabituationFailureEvent::eventInfo() const
{
	return QString("HabituationFailure");
};

HHabituationFailureEvent* HHabituationFailureEvent::getEvent(QDataStream& stream, int timestamp)
{
	Q_UNUSED(stream);
	return new HHabituationFailureEvent(timestamp);
}

