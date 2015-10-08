/*
 * HTestingInputWrangler.h
 *
 *  Created on: Oct 7, 2015
 *      Author: Oakeslab
 */

#ifndef HTESTINGINPUTWRANGLER_H_
#define HTESTINGINPUTWRANGLER_H_

#include <QMap>
#include <QPair>
#include <QList>
#include <QTimer>
#include <QString>
#include "HEventLog.h"
#include "HTypes.h"
#include "HLookDetector.h"
#include "HExperiment.h"

class HTestingInputWrangler: public QObject
{
	Q_OBJECT

	typedef QMap< QPair<int,int>, HEventLog *> TrialEventLogMap;
	typedef QMap<HPhaseType, TrialEventLogMap> PhaseTrialEventLogMapMap;

	QTimer* m_ptimer;
	PhaseTrialEventLogMapMap m_map;
	TrialEventLogMap *m_pCurrentEventLogMap;
	HEventLog *m_pEventLog;
	//QList<HEvent*>::const_iterator m_inputIterator;
	//HEventLog::const_iterator m_inputIterator;
	HMutableEventLogIterator m_inputIterator;
	HLookDetector *m_pLD;
	int m_offsetTime;
	int m_lastCheckTime;
	bool m_bIsEnabled;

	bool processLine(const QString& line);

public:
	static HEventLog m_staticDummyEventLog;

	HTestingInputWrangler();
	virtual ~HTestingInputWrangler() {};
	void enable(HLookDetector *pLD, const HExperiment* pExpt);
	bool load(QFile& inputFile);
protected slots:
	void phaseStarted(QString);
	void trialStarted(int, int);
	void lookDetectorEnabled();
	void lookDetectorDisabled();
	void check();
};



#endif /* HTESTINGINPUTWRANGLER_H_ */
