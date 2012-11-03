/*
 *  HEvents.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/31/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


/*
 *  habit2.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HEvents.h"

QTextStream& habit2::operator<<(QTextStream& out, const habit2::HEventType& type)
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

QTextStream& habit2::operator<<(QTextStream& out, const habit2::HTrialEndType& etype)
{
	switch (etype)
	{
		case kTrialEndSuccess:
			out << "Success";
			break;
		case kTrialEndFail:
			out << "Fail";
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
			


QTextStream& habit2::operator<<(QTextStream& out, const habit2::HEvent& e)
{
	out << qSetFieldWidth(7) << right << e.timestamp() << " " << qSetFieldWidth(20) << left << e.type() << " " << qSetFieldWidth(0) << e.eventInfo() << endl;
	return out;
};

QString habit2::HPhaseStartEvent::eventInfo() const 
{
	return m_phase;
};

QString habit2::HPhaseEndEvent::eventInfo() const 
{
	return QString("Phase ended");
};

QString habit2::HTrialStartEvent::eventInfo() const 
{
	return QString("Trial %1 repeat %2").arg(trialnumber()).arg(repeatnumber());
};

QString habit2::HTrialEndEvent::eventInfo() const
{
	QString s;
	switch (m_endtype)
	{
		case kTrialEndSuccess:
			s = "Success";
			break;
		case kTrialEndFail:
			s = "Fail";
			break;
		case kTrialEndAbort:
			s = "Abort";
			break;
		default:
			s = "Unknown reason";
			break;
	}
	return s;
};

QString habit2::HStimRequestEvent::eventInfo() const
{
	return QString("Request stim index %1").arg(m_stimindex);
};

QString habit2::HStimStartEvent::eventInfo() const
{
	return QString("Stim index %1 started on player %2").arg(m_stimindex).arg(m_playerid);
};

QString habit2::HStimulusSettingsEvent::eventInfo() const
{
	return QString("Stim index: %1  Name: %2").arg(m_stimindex, 2).arg(m_settings.getName());
};

QString habit2::HStimulusInfoEvent::eventInfo() const
{
	return QString("Stim index: %1  File: %2 Loop: %3 Volume: %4").arg(m_stimindex).arg(m_info.getFileName()).arg(m_info.isLoopPlayBack()).arg(m_info.getAudioBalance());
};

QString habit2::HAttentionEvent::eventInfo() const
{
	return QString("");
};

QString habit2::HLookEvent::eventInfo() const
{
	QString s;
	QTextStream tmp(&s);
	tmp << m_look;
	return s;
};