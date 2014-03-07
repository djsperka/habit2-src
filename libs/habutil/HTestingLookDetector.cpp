/*
 * HTestingLookDetector.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: Oakeslab
 */

#include "HTestingLookDetector.h"
#include "HElapsedTimer.h"
#include <QTextStream>
#include <QStringList>
#include <QTimer>


HTestingLookDetector::HTestingLookDetector(QFile& inputFile, int minlooktime_ms, int minlookawaytime_ms, int maxlookawaytime_ms, int maxaccumlooktime_ms, HEventLog& log, QWidget* pdialog, bool bUseLeft, bool bUseCenter, bool bUseRight)
: HLookDetector(minlooktime_ms, minlookawaytime_ms, maxlookawaytime_ms, maxaccumlooktime_ms, log)
, m_pdialog(pdialog)
, m_bUseLeft(bUseLeft)
, m_bUseCenter(bUseCenter)
, m_bUseRight(bUseRight)
, m_lastCheckTime(-1)
{
	m_ptimer = new QTimer();
	m_ptimer->setInterval(0);
	QObject::connect(m_ptimer, SIGNAL(timeout()), this, SLOT(check()));

	qDebug() << "HTestingLookDetector: load signals from input file.";

	int num = 0;
    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
    	QTextStream in(&inputFile);
    	while (!in.atEnd())
    	{
    		QString line = in.readLine();
    		num++;
    		if (!processLine(line))
    		{
    			qDebug() << "Error in input file at line " << num << ": " << line;
    		}
    	}
    }
};

void HTestingLookDetector::check()
{
	int t = HElapsedTimer::elapsed();
	if (t == m_lastCheckTime) return;

	while (m_lookTransitions.size() > 0 && m_lookTransitions[0].second <= t)
	{
		QPair<const HLookTrans*, int> pair = m_lookTransitions.takeFirst();
		addTrans(*pair.first, pair.second);
		qDebug() << "HTestingLookDetector::check() addTrans " << *pair.first << " @ " << pair.second;
	}
	m_lastCheckTime = t;
};

void HTestingLookDetector::agLookEnabled(bool enabled)
{
	Q_UNUSED(enabled);
}

void HTestingLookDetector::lookEnabled(bool enabled)
{
	if (enabled)
		m_ptimer->start();
	else
		m_ptimer->stop();
}

bool HTestingLookDetector::processLine(const QString& line)
{
	bool bval = true;

	// If line begins with # or is blank then skip it
	if (line.trimmed().isEmpty() || line.startsWith('#'))
	{
		// skip line
	}
	else if (line.startsWith('!'))
	{
#if 0
		QString s = line.remove(0,1);
		QTextStream in(s);
		int i0, i1, i2, i3;
		in >> i0 >> i1 >> i2 >> i3;
		qDebug() << "Read parameters from input file line: " << i0 << " " i1 << " " << i2 << " " << i3;
#endif
	}
	else
	{
		QStringList tokens = line.split(",");
		if (tokens.at(0) == HLookTrans::NoneLeft.name())
		{
			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::NoneLeft, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::LeftNone.name())
		{
			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::LeftNone, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::NoneCenter.name())
		{
			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::NoneCenter, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::CenterNone.name())
		{
			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::CenterNone, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::NoneRight.name())
		{
			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::NoneRight, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::RightNone.name())
		{
			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::RightNone, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::NoneNone.name())
		{
			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::NoneNone, tokens.at(1).toInt()));
		}
		else
		{
			qDebug() << "Error in input file: unknown look trans type: " << line;
			bval = false;
		}
	}
	return bval;
}


HTestingLookDetector::~HTestingLookDetector()
{
};


