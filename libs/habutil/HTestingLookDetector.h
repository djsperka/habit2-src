/*
 * HTestingLookDetector.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Oakeslab
 */

#ifndef HTESTINGLOOKDETECTOR_H_
#define HTESTINGLOOKDETECTOR_H_

#include "HLookDetector.h"
#include <QString>
#include <QFile>
#include <QTimer>
#include "HEventLog.h"

class HTestingLookDetector: public HLookDetector
{
	Q_OBJECT

public:
	HTestingLookDetector(QFile& inputFile, HEventLog& log, bool bInclusiveLookTime, QWidget* pdialog, bool bUseLeft, bool bUseCenter, bool bUseRight);
	virtual ~HTestingLookDetector();
protected:
	virtual void agLookEnabled(bool enabled);
	virtual void lookEnabled(bool enabled);
	virtual bool load(QFile& inputFile);	// will be overloaded
private:
	// Process a line read from the input file.
	bool processLine(const QString& line);
private slots:
	void check();
private:
	QWidget* m_pdialog;
	bool m_bUseLeft;
	bool m_bUseCenter;
	bool m_bUseRight;
	int m_lastCheckTime;
	int m_offsetTime;		// The elapsed timer used during testing may have elapsed more time than the
							// actual experiment due to pauses, e.g. This time is subtracted from the
							// value of the elapsed timer to get the time in the same time frame as the
							// input data events
	int m_eventOffsetTime;
	bool m_bAG;
	bool m_bLook;

	/// timer for checking trans list.
	QTimer* m_ptimer;

	/// Transitions list
//	QList< QPair< const HLookTrans*, int> > m_lookTransitions;

	// input is taken from an event log. This is NOT the same event log
	// that is used by HLooker for output.
	HEventLog m_input;
	HMutableEventLogIterator m_inputIterator;
};

#endif /* HTESTINGLOOKDETECTOR_H_ */
