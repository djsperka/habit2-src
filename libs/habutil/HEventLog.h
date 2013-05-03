/*
 *  HEventLog.h
 *  habutil
 *
 *  Created by Oakes Lab on 10/31/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HEVENTLOG_H_
#define _HEVENTLOG_H_

#include "HEvents.h"
#include "HPhaseLog.h"
#include <QList>
#include <QDataStream>

class HEventLog: public QList<HEvent*>
{
public:		
	HEventLog()
	: QList<HEvent*>()
	{};

	HEventLog(const HEventLog& l)
	: QList<HEvent*>(l)
	{};

	virtual ~HEventLog();	// must destroy all events

	HPhaseLog getPhaseLog(QString s = QString(""));
	
	bool saveToCSV(QString& filename);
};

QDataStream& operator<< (QDataStream& stream, HEventLog log);
QDataStream& operator>> (QDataStream& stream, HEventLog& log);

#endif
