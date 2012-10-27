/*
 *  HPhaseLog.h
 *  habutil
 *
 *  Created by Oakes Lab on 10/22/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HPHASELOG_H_
#define _HPHASELOG_H_

#include "HTrialLog.h"
#include <QObject>
#include <QList>
#include <QDataStream>


class HPhaseLog: public QObject
{
	Q_OBJECT
	
public:
	HPhaseLog() : QObject(), m_triallogs() {};
	virtual ~HPhaseLog() {};

	int nCompleted() const;
	int totalLookingTime() const;

	friend QDataStream & operator << (QDataStream& stream, const HPhaseLog &plog);
	friend QDataStream & operator >> (QDataStream& stream, HPhaseLog& tlog);
	friend bool operator==(const HPhaseLog& lhs, const HPhaseLog& rhs);
	
protected slots:
	void trialStarted(Habit::StimulusSettings& settings);
	void trialCompleted();
	void trialAborted();
	void trialFailed();
	void gotLook(HLook l);
protected:
	QList<HTrialLog> m_triallogs;
};

QDataStream & operator << (QDataStream& stream, const HPhaseLog &plog);
QDataStream & operator >> (QDataStream& stream, HPhaseLog& tlog);
bool operator==(const HPhaseLog& lhs, const HPhaseLog& rhs);

#endif