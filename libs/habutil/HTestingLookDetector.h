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
	HTestingLookDetector(QFile& inputFile, int minlooktime_ms, int minlookawaytime_ms, int maxlookawaytime_ms, int maxaccumlooktime_ms, HEventLog& log, QWidget* pdialog, bool bUseLeft, bool bUseCenter, bool bUseRight);
	virtual ~HTestingLookDetector();
protected:
	virtual void agLookEnabled(bool enabled);
	virtual void lookEnabled(bool enabled);
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

	/// timer for checking trans list.
	QTimer* m_ptimer;

	/// Transitions list
	QList< QPair< const HLookTrans*, int> > m_lookTransitions;
};

#endif /* HTESTINGLOOKDETECTOR_H_ */