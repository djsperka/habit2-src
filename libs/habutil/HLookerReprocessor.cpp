/*
 * HLookerReprocessor.cpp
 *
 *  Created on: Mar 9, 2016
 *      Author: Oakeslab
 */

#include <HLookerReprocessor.h>
#include <QtDebug>
#include <QCoreApplication>
#include <QEventLoop>
using namespace Habit;

HLookerReprocessor::HLookerReprocessor(const HLookSettings& lookSettings, const QList<HEvent*>& events, QObject *parent)
: QObject(parent)
, m_list()
, m_log()
{
	bool bHaveLookEnabled = false;

	m_pLooker = new HLooker(m_log, false);

	// set looker settings.
	// Setting max accum look time and max look away time to 0 means that they're ignored by HLooker.
	m_pLooker->setIsInclusiveLookTime(lookSettings.getInclusiveLookTime());
	m_pLooker->setMinLookTime(lookSettings.getMinLookTime());
	m_pLooker->setMinLookAwayTime(lookSettings.getMinLookAwayTime());
	m_pLooker->setMaxAccumulatedLookTime(0);
	m_pLooker->setMaxLookAwayTime(0);

	// set up connections to look(HLook) and lookAborted(HLook) signals
	connect(m_pLooker, SIGNAL(look(HLook)), this, SLOT(gotLook(HLook)));
	connect(m_pLooker, SIGNAL(lookAborted(HLook)), this, SLOT(gotLookAborted(HLook)));
	connect(m_pLooker, SIGNAL(lookingDirection(QString)), this, SLOT(gotLookingDirection(QString)));

	// start state machine
	// Note that we are running inside of a SLOT, and so the Qt event loop is effectively stopped.
	// We intersperse calls to process events below to address this.

	m_pLooker->start();
	QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

	// Now feed all HTransEvents through the looker.
	HEvent* e;
	foreach( e, events) {
		if (e->type() == HEventType::HEventLookTrans)
		{
			HLookTransEvent* plt = static_cast<HLookTransEvent*>(e);
			//qDebug() << "Adding look trans " << plt->transtype() << " at " << plt->timestamp();
			qDebug() << "HEvent: " << e->eventCSV();
			m_pLooker->addTrans(plt->transtype(), plt->timestamp());
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
		}
		else if (e->type() == HEventType::HEventLookEnabled)
		{
			qDebug() << "HEvent: " << e->eventCSV();
			bHaveLookEnabled = true;
			// nothing done with these events currently
			//HLookEnabledEvent* ple = static_cast<HLookEnabledEvent*>(e);
		}
		else if (e->type() == HEventType::HEventLookDisabled)
		{
			qDebug() << "HEvent: " << e->eventCSV();
			if (bHaveLookEnabled)
			{
				qDebug() << "Stopping reprocessor at " << e->timestamp();
				m_pLooker->stopLooker(e->timestamp());
				QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
			}
			else
			{
				qDebug() << "Received HLookDisabled event before HLookEnabled event. Ignoring...";
			}
		}
	}
	QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

}

HLookerReprocessor::~HLookerReprocessor() {
	delete m_pLooker;
}

void HLookerReprocessor::gotLook(HLook l)
{
	qDebug() << "gotLook():";
	qDebug() << l;
	m_list.append(l);
}

void HLookerReprocessor::gotLookAborted(HLook l)
{
	qDebug() << "gotLookAborted():";
	qDebug() << l;
	m_list.append(l);
}

void HLookerReprocessor::gotLookingDirection(QString s)
{
	//qDebug() << "gotLookingDirection " << s;
}

