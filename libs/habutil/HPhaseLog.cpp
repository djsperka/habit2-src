/*
 *  HPhaseLog.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/22/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhaseLog.h"

int HPhaseLog::nCompleted() const
{
	int n=0;
	for (int i=0; i<m_triallogs.size(); i++)
		if (m_triallogs.at(i).isCompleted()) n++;
	return n;
};

int HPhaseLog::totalLookingTime() const
{
	int total=0;
	for (int i=0; i<m_triallogs.size(); i++)
		if (m_triallogs.at(i).isCompleted()) total += m_triallogs.at(i).totalLookingTime();
	return total;
};

void HPhaseLog::trialStarted(Habit::StimulusSettings& settings)
{
	HTrialLog t(settings);
	t.setRunning();
	m_triallogs.append(t);
};

void HPhaseLog::trialCompleted()
{
	if (!m_triallogs.isEmpty())
		m_triallogs.back().setCompleted();
};

void HPhaseLog::trialAborted()
{
	if (!m_triallogs.isEmpty())
		m_triallogs.back().setAborted();
};

void HPhaseLog::trialFailed()
{
	if (!m_triallogs.isEmpty())
		m_triallogs.back().setFailed();
};

void HPhaseLog::gotLook(HLook l)
{
	if (!m_triallogs.isEmpty())
	{
		m_triallogs.back().append(l);
	}
	else 
	{
		qWarning("HPhaseLog: cannot add look - no trial running.");
	}
};

QDataStream& operator<<(QDataStream& stream, const HPhaseLog &plog)
{
	stream << plog.m_triallogs;
	return stream;	
}

QDataStream& operator>>(QDataStream& stream, HPhaseLog& plog)
{
	stream >> plog.m_triallogs;
	return stream;
}

bool operator==(const HPhaseLog& lhs, const HPhaseLog& rhs)
{
	return lhs.m_triallogs == rhs.m_triallogs;
};


