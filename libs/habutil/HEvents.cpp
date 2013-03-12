/*
 *  HEvents.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/31/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include "HEvents.h"


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

QString HTrialStartEvent::eventInfo() const 
{
	return QString("Trial %1 repeat %2").arg(trialnumber()).arg(repeatnumber());
};

QString HTrialStartEvent::eventCSVAdditional() const
{
	return QString("%1,%2").arg(trialnumber()).arg(repeatnumber());
};

QString HTrialEndEvent::eventInfo() const
{
	return endtype().name();
};

QString HTrialEndEvent::eventCSVAdditional() const
{
	return endtype().name();
};

QString HStimRequestEvent::eventInfo() const
{
	return QString("Request stim index %1").arg(m_stimindex);
};

QString HStimRequestEvent::eventCSVAdditional() const
{
	return QString("%1").arg(m_stimindex);
};

QString HStimStartEvent::eventInfo() const
{
	return QString("Stim started on player %1").arg(m_playerid);
};

QString HStimStartEvent::eventCSVAdditional() const
{
	return QString("%1").arg(m_playerid);
};

QString HAGStartEvent::eventInfo() const
{
	return QString("AG started on player %1").arg(m_playerid);
};

QString HAGStartEvent::eventCSVAdditional() const
{
	return QString("%1").arg(m_playerid);
};

QString HStimulusSettingsEvent::eventInfo() const
{
	return QString("Stim index: %1  Name: %2").arg(m_stimindex, 2).arg(m_settings.getName());
};

QString HStimulusInfoEvent::eventInfo() const
{
	return QString("Stim index: %1  File: %2 Loop: %3 Volume: %4").arg(m_stimindex).arg(m_info.getFileName()).arg(m_info.isLoopPlayBack()).arg(m_info.getAudioBalance());
};

QString HAttentionEvent::eventInfo() const
{
	return QString("");
};

QString HLookEvent::eventInfo() const
{
	QString s;
	QTextStream tmp(&s);
	tmp << m_look;
	return s;
};

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

QString HHabituationSuccessEvent::eventInfo() const
{
	return QString("HabituationSuccess");
};

QString HHabituationFailureEvent::eventInfo() const
{
	return QString("HabituationFailure");
};


