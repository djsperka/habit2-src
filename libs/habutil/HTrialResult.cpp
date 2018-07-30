/*
 * HTrialResult.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: Oakeslab
 */

#include "HTrialResult.h"

// This should be kept in sync with the enum defined in HTrialResultsRow. Pain but I can't think of a better way now.
QStringList HTrialResult::headers =
		QStringList() 	<< "SubjectID" << "Phase" << "Order (randomization)" << "Trial" << "Repeat" << "EndType"
						<< "Habituated" << "StimID" << "StimName" << "StimLabel"
						<< "Left" << "Center" << "Right" << "ISS"
						<< "Trial Start" << "Trial End"
						<< "TotalLook" << "TotalLookAway" << "TotalLeft" << "TotalCenter" << "TotalRight" << "LookEnabled" << "LookDisabled" << "Looks";



HTrialResult::HTrialResult()
: m_key()
, m_order("")
, m_trialStartTime(0)
, m_trialEndTime(0)
, m_lookEnabledTime(0)
, m_lookDisabledTime(0)
, m_bHabituated(false)
, m_sSubjectID("")
, m_sOrder("")
, m_sEndType("")
, m_iStimID(0)
, m_sStimName("")
, m_sStimLabel("")
, m_sStimLeft("")
, m_sStimCenter("")
, m_sStimRight("")
, m_sStimISS("")
, m_looks()
, m_events()
{}

HTrialResult::HTrialResult(const HTrialResult& r)
: m_key(r.m_key)
, m_order(r.m_order)
, m_trialStartTime(r.m_trialStartTime)
, m_trialEndTime(r.m_trialEndTime)
, m_lookEnabledTime(r.m_lookEnabledTime)
, m_lookDisabledTime(r.m_lookDisabledTime)
, m_bHabituated(r.m_bHabituated)
, m_sSubjectID(r.m_sSubjectID)
, m_sOrder(r.m_sOrder)
, m_sEndType(r.m_sEndType)
, m_iStimID(r.m_iStimID)
, m_sStimName(r.m_sStimName)
, m_sStimLabel(r.m_sStimLabel)
, m_sStimLeft(r.m_sStimLeft)
, m_sStimCenter(r.m_sStimCenter)
, m_sStimRight(r.m_sStimRight)
, m_sStimISS(r.m_sStimISS)
, m_looks(r.m_looks)
, m_events(r.m_events)
{}


HTrialResult::HTrialResult(const HTrialKey& key)
: m_key(key)
, m_order("")
, m_trialStartTime(0)
, m_trialEndTime(0)
, m_lookEnabledTime(0)
, m_lookDisabledTime(0)
, m_bHabituated(false)
, m_sSubjectID("")
, m_sOrder("")
, m_sEndType("")
, m_iStimID(0)
, m_sStimName("")
, m_sStimLabel("")
, m_sStimLeft("")
, m_sStimCenter("")
, m_sStimRight("")
, m_sStimISS("")
, m_looks()
, m_events()
{}

HTrialResult::~HTrialResult() {};

HTrialResult& HTrialResult::operator=(const HTrialResult& rhs)
{
	if (this == &rhs) return *this;
	m_key = rhs.m_key;
	m_order = rhs.m_order;
	m_trialStartTime = rhs.m_trialStartTime;
	m_trialEndTime = rhs.m_trialEndTime;
	m_lookEnabledTime = rhs.m_lookEnabledTime;
	m_lookDisabledTime = rhs.m_lookDisabledTime;
	m_bHabituated = rhs.m_bHabituated;
	m_sSubjectID = rhs.m_sSubjectID;
	m_sOrder = rhs.m_sOrder;
	m_sEndType = rhs.m_sEndType;
	m_iStimID = rhs.m_iStimID;
	m_sStimName = rhs.m_sStimName;
	m_sStimLabel = rhs.m_sStimLabel;
	m_sStimLeft = rhs.m_sStimLeft;
	m_sStimCenter = rhs.m_sStimCenter;
	m_sStimRight = rhs.m_sStimRight;
	m_sStimISS = rhs.m_sStimISS;
	m_looks = rhs.m_looks;
	m_events = rhs.m_events;
	return *this;
}

const QList<HLook>& HTrialResult::looks() const
{
	return m_looks;
}

const QList<HEvent*>& HTrialResult::events() const
{
	return m_events;
}


QString HTrialResult::getHeader(int header) const
{
	return HTrialResult::headers.at(header);
}

QString HTrialResult::getString(int header) const
{
	QString s("???");
	switch (header)
	{
		case indSubjectId: s = m_sSubjectID; break;
		case indPhase: s = m_key.phase(); break;
		case indOrderName: s = m_sOrder; break;
		case indTrial: s = QString("%1").arg(m_key.trial()); break;
		case indRepeat: s = QString("%1").arg(m_key.repeat()); break;
		case indEndType: s = m_sEndType; break;
		case indHabituated: s = (m_bHabituated ? "Yes" : "No"); break;
		case indStimId:  s = QString("%1").arg(m_iStimID); break;
		case indStimName: s = m_sStimName; break;
		case indStimLabel: s = m_sStimLabel; break;
		case indStimLeft: s = m_sStimLeft; break;
		case indStimCenter: s = m_sStimCenter; break;
		case indStimRight: s = m_sStimRight; break;
		case indStimISS: s = m_sStimISS; break;
		case indTrialStartTime: s = QString("%1").arg(m_trialStartTime); break;
		case indTrialEndTime: s = QString("%1").arg(m_trialEndTime); break;
		case indTotalLook:
		{
			int total=0;
			HLook l;
			foreach (l, m_looks) {
				if (l.isComplete())
					total += l.lookMS();
			}
			s = QString("%1").arg(total);
			break;
		}
		case indTotalAway: s = "0"; break;
		case indTotalLookLeft:
		{
			int total=0;
			HLook l;
			foreach (l, m_looks) {
				if (l.isComplete() && l.direction() == HLookDirection::LookLeft)
					total += l.lookMS();
			}
			s = QString("%1").arg(total);
			break;
		}
		case indTotalLookCenter:
		{
			int total=0;
			HLook l;
			foreach (l, m_looks) {
				if (l.isComplete() && l.direction() == HLookDirection::LookCenter)
					total += l.lookMS();
			}
			s = QString("%1").arg(total);
			break;
		}
		case indTotalLookRight:
		{
			int total=0;
			HLook l;
			foreach (l, m_looks) {
				if (l.isComplete() && l.direction() == HLookDirection::LookRight)
					total += l.lookMS();
			}
			s = QString("%1").arg(total);
			break;
		}
		case indLookEnabledTime: s = QString("%1").arg(m_lookEnabledTime); break;
		case indLookDisabledTime: s = QString("%1").arg(m_lookDisabledTime); break;
		default: s = "BAD INDEX"; break;
	}
	return s;
}


void HTrialResult::setSubjectID(const QString& s)
{
	m_sSubjectID = s;
}

void HTrialResult::setOrder(const QString& s)
{
	m_sOrder = s;
}

void HTrialResult::setEndType(const QString& s)
{
	m_sEndType = s;
}

void HTrialResult::setStimId(int i)
{
	m_iStimID = i;
}

void HTrialResult::setStimName(const QString& s)
{
	m_sStimName = s;
}

void HTrialResult::setStimLabel(const QString& s)
{
	m_sStimLabel = s;
}

void HTrialResult::setHabituated(bool b)
{
	m_bHabituated = b;
}

void HTrialResult::setTrialStartTime(int t)
{
	m_trialStartTime = t;
}

void HTrialResult::setTrialEndTime(int t)
{
	m_trialEndTime = t;
}

void HTrialResult::setLookEnabledTime(int t)
{
	m_lookEnabledTime = t;
}

void HTrialResult::setLookDisabledTime(int t)
{
	m_lookDisabledTime = t;
}

void HTrialResult::setLooks(const QList<HLook>& looks)
{
	m_looks = looks;
}



void HTrialResult::appendLook(const HLook& look)
{
	m_looks.append(look);
}

void HTrialResult::appendEvent(HEvent *e)
{
	m_events.append(e);
}

void HTrialResult::setStimLeft(const QString& s)
{
	m_sStimLeft = s;
}

void HTrialResult::setStimCenter(const QString& s)
{
	m_sStimCenter = s;
}

void HTrialResult::setStimRight(const QString& s)
{
	m_sStimRight = s;
}

void HTrialResult::setStimISS(const QString& s)
{
	m_sStimISS = s;
}

void HTrialResult::setOrderName(const QString& s)
{
	m_sOrder = s;
}
