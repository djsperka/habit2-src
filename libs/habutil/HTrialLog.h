/*
 *  HTrialLog.h
 *  habutil
 *
 *  Created by Oakes Lab on 10/16/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HTRIALLOG_H_
#define _HTRIALLOG_H_

#include "HLook.h"
#include "stimulussettings.h"
#include <QList>
#include <QDataStream>


class HTrialLog : public QList<const HLook *>
{
public:
	HTrialLog()
	: QList<const HLook *>()
	, m_status(TrialRunning)
	{};
	
	HTrialLog(const HTrialLog& l)
	: QList<const HLook *>(l)
	{
		setStatus(l.getStatus());
	};
	virtual ~HTrialLog() {};

	enum TrialStatus {
		TrialCompleted, TrialRunning, TrialFailed, TrialAborted, UnknownTrialStatus
	};

	bool isCompleted() const { return m_status==TrialCompleted; };
	int totalLookingTime() const;
	void setCompleted() { m_status = TrialCompleted; };
	void setFailed() { m_status = TrialFailed; };
	void setAborted() { m_status = TrialAborted; };
	void setRunning() { m_status = TrialRunning; };
	TrialStatus getStatus() const { return m_status; };
	void setStatus(TrialStatus status) { m_status = status; };
private:
	TrialStatus m_status;
};

Q_DECLARE_METATYPE(HTrialLog);

QDataStream & operator << (QDataStream& stream, const HTrialLog &tlog);
QDataStream & operator >> (QDataStream& stream, HTrialLog& tlog);
QDataStream & operator << (QDataStream& stream, const HTrialLog::TrialStatus& s);
QDataStream & operator >> (QDataStream& stream, HTrialLog::TrialStatus& s);
bool operator==(const HTrialLog& lhs, const HTrialLog& rhs);


#endif

