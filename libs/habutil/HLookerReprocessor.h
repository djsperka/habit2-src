/*
 * HLookerReprocessor.h
 *
 *  Created on: Mar 9, 2016
 *      Author: Oakeslab
 */

#ifndef HLOOKERREPROCESSOR_H_
#define HLOOKERREPROCESSOR_H_

#include <HLooker.h>
#include "HEvents.h"
#include <QList>
#include <HEventLog.h>
#include <HLookSettings.h>

class HLookerReprocessor: public QObject
{
	Q_OBJECT

private:
	HLooker* m_pLooker;
	QList<HLook> m_list;
	HEventLog m_log;
public:
	HLookerReprocessor(const Habit::HLookSettings& lookSettings, const QList<HEvent*>& events, QObject *parent=NULL);
	virtual ~HLookerReprocessor();
	QList<HLook>& looks() { return m_list; };
public slots:
	void gotLook(HLook);
	void gotLookAborted(HLook);
	void gotLookingDirection(QString s);

};

#endif /* HLOOKERREPROCESSOR_H_ */
