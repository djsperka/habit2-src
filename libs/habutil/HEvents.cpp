/*
 *  HEvents.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/31/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include "HEvents.h"

QTextStream& operator<<(QTextStream& out, const HEventType& type)
{
	switch (type) {
		case kPhaseStart:
			out << "PhaseStart";
			break;
		case kPhaseEnd:
			out << "PhaseEnd";
			break;
		case kTrialStart:
			out << "TrialStart";
			break;
		case kTrialEnd:
			out << "TrialEnd";
			break;
		case kStimRequest:
			out << "StimRequest";
			break;
		case kStimStart:
			out << "StimStart";
			break;
		case kStimulusSettings:
			out << "StimulusSettings";
			break;
		case kStimulusInfo:
			out << "StimulusInfo";
			break;
		case kAttention:
			out << "Attention";
			break;
		case kLook:
			out << "Look";
			break;
		case kUndefined:
		default:
			out << "Undefined";
			break;
	}
	return out;
};

QTextStream& operator<<(QTextStream& out, const HTrialEndType& etype)
{
	switch (etype)
	{
		case kTrialEndGotLook:
			out << "GotLook";
			break;
		case kTrialEndFixedTimeout:
			out << "FixedTimeout";
			break;
		case kTrialEndNoLookTimeout:
			out << "NoLookTimeout";
			break;
		case kTrialEndAbort:
			out << "Abort";
			break;
		default:
			out << "Unknown reason";
			break;
	}
	return out;
};

QTextStream& operator<<(QTextStream& out, const HEvent& e)
{
	out << qSetFieldWidth(7) << right << e.timestamp() << " " << qSetFieldWidth(20) << left << e.type() << " " << qSetFieldWidth(0) << e.eventInfo() << endl;
	return out;
};

QString HPhaseStartEvent::eventInfo() const 
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

QString HTrialEndEvent::eventInfo() const
{
	QString s;
	switch (m_endtype)
	{
		case kTrialEndGotLook:
			s = "GotLook";
			break;
		case kTrialEndFixedTimeout:
			s = "FixedTimeout";
			break;
		case kTrialEndNoLookTimeout:
			s = "NoLookTimeout";
			break;
		case kTrialEndAbort:
			s = "TrialAborted";
			break;
		default:
			s = "Unknown reason";
			break;
	}
	return s;
};


QString HStimRequestEvent::eventInfo() const
{
	return QString("Request stim index %1").arg(m_stimindex);
};

QString HStimStartEvent::eventInfo() const
{
	return QString("Stim started on player %1").arg(m_playerid);
};

QString HAGStartEvent::eventInfo() const
{
	return QString("AG started on player %1").arg(m_playerid);
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

QString HLookTransEvent::eventInfo() const
{
	QString s;
	QTextStream tmp(&s);
	tmp << transtype();
	return s;
};
