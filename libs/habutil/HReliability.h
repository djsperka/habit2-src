/*
 * HReliability.h
 *
 *  Created on: Jun 17, 2013
 *      Author: Oakeslab
 */

#ifndef HRELIABILITY_H_
#define HRELIABILITY_H_

#include "HLogState.h"
#include "HQEvents.h"

#include <QAbstractTransition>

class HLookDetector;
class HResults;

class HReliability: public HLogState
{
	Q_OBJECT

public:
	HReliability(HEventLog& log, HLookDetector& ld, HResults& r, QState* parent = 0);
	~HReliability() {};

	HLookDetector& getLookDetector() { return m_ld; };

protected:
	virtual void onEntry(QEvent* e);
	virtual void onExit(QEvent* e);

private:
	HEventLog& m_log;
	HLookDetector& m_ld;
	HResults& m_results;
};


class HReliabilityTransition: public QAbstractTransition
{
	Q_OBJECT

public:
	HReliabilityTransition(HEventLog& log, HLookDetector& ld) : m_log(log), m_ld(ld) {};
	~HReliabilityTransition() {};
protected:
	bool eventTest(QEvent* e);
	void onTransition(QEvent* e) { Q_UNUSED(e); };
private:
	HEventLog& m_log;
	HLookDetector& m_ld;
};

class HReliabilityEndTransition: public QAbstractTransition
{
	Q_OBJECT

public:
	HReliabilityEndTransition(HEventLog& log) : m_log(log) {};
	~HReliabilityEndTransition() {};
protected:
	bool eventTest(QEvent* e);
	void onTransition(QEvent* e) { Q_UNUSED(e); };
private:
	HEventLog& m_log;
};



#endif /* HRELIABILITY_H_ */
