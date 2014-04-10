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
#include <QtDebug>

HTestingLookDetector::HTestingLookDetector(QFile& inputFile, int minlooktime_ms, int minlookawaytime_ms, int maxlookawaytime_ms, int maxaccumlooktime_ms, HEventLog& log, QWidget* pdialog, bool bUseLeft, bool bUseCenter, bool bUseRight)
: HLookDetector(minlooktime_ms, minlookawaytime_ms, maxlookawaytime_ms, maxaccumlooktime_ms, log)
, m_pdialog(pdialog)
, m_bUseLeft(bUseLeft)
, m_bUseCenter(bUseCenter)
, m_bUseRight(bUseRight)
, m_lastCheckTime(-1)
, m_offsetTime(0)
, m_eventOffsetTime(0)
, m_bAG(false)
, m_bLook(false)
, m_input()
, m_inputIterator(m_input)
{
	m_ptimer = new QTimer();
	m_ptimer->setInterval(0);
	QObject::connect(m_ptimer, SIGNAL(timeout()), this, SLOT(check()));
	load(inputFile);
    m_inputIterator.toFront();
}

bool HTestingLookDetector::load(QFile& inputFile)
{
	bool b = false;
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
    return b;
};

void HTestingLookDetector::check()
{
	int t = HElapsedTimer::elapsed();
	int tRelative = t - m_offsetTime;	// compare to tEvent-tEventOffsetTime
	if (t == m_lastCheckTime) return;
	while (	m_inputIterator.hasNext() &&
			m_inputIterator.peekNext()->type() != HEventType::HEventLookDisabled &&
			m_inputIterator.peekNext()->timestamp()-m_eventOffsetTime <= tRelative)
	{
		// Events of interest depend on whether we are in AGLook or Look mode.
		if (m_bAG)
		{
			if (m_inputIterator.peekNext()->type() == HEventType::HEventAttention)
			{
				emit attention();
			}
			else
			{
				qDebug() << "HTestingLookDetector::check(AG): skip event " << m_inputIterator.peekNext()->eventCSV();
			}
		}
		if (m_bLook)
		{
			if (m_inputIterator.peekNext()->type() == HEventType::HEventLookTrans)
			{
				HLookTransEvent* elt = static_cast<HLookTransEvent*>(m_inputIterator.peekNext());
				addTrans(elt->transtype(), t);
			}
			else
			{
				qDebug() << "HTestingLookDetector::check(Look): skip event " << m_inputIterator.peekNext()->eventCSV();
			}
		}
		m_inputIterator.next();
	}
	m_lastCheckTime = t;
};

void HTestingLookDetector::agLookEnabled(bool enabled)
{
	if (enabled)
	{
		m_bAG = true;
		m_bLook = false;
		m_ptimer->start();
		if (m_inputIterator.skipToEventType(HEventType::HEventAGLookEnabled))
		{
			// The time on the AGLookEnabled event is the event offset
			m_eventOffsetTime = m_inputIterator.peekNext()->timestamp();
			qDebug() << "HTestingLookDetector::agLookEnabled(true): AGLookEnabledEvent @ " << m_eventOffsetTime;
		}
		else
		{
			qCritical() << "HTestingLookDetector::agLookEnabled(true): Cannot skip to AGLookEnabled event.";
		}
	}
	else
	{
		m_ptimer->stop();
		m_bAG = false;
		m_bLook = false;
	}
}

void HTestingLookDetector::lookEnabled(bool enabled)
{
	if (enabled)
	{
		m_bAG = false;
		m_bLook = true;
		m_ptimer->start();
		if (m_inputIterator.skipToEventType(HEventType::HEventLookEnabled))
		{
			// The time on the LookEnabled event is the event offset
			m_eventOffsetTime = m_inputIterator.peekNext()->timestamp();
			qDebug() << "HTestingLookDetector::lookEnabled(true): LookEnabledEvent @ " << m_eventOffsetTime;
		}
		else
		{
			qCritical() << "HTestingLookDetector::lookEnabled(true): Cannot skip to LookEnabled event.";
		}
	}
	else
	{
		m_bAG = false;
		m_bLook = false;
		m_ptimer->stop();
	}
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
			m_input.append(new HLookTransEvent(HLookTrans::NoneLeft, tokens.at(1).toInt()));
//			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::NoneLeft, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::LeftNone.name())
		{
			m_input.append(new HLookTransEvent(HLookTrans::LeftNone, tokens.at(1).toInt()));
//			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::LeftNone, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::NoneCenter.name())
		{
			m_input.append(new HLookTransEvent(HLookTrans::NoneCenter, tokens.at(1).toInt()));
//			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::NoneCenter, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::CenterNone.name())
		{
			m_input.append(new HLookTransEvent(HLookTrans::CenterNone, tokens.at(1).toInt()));
//			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::CenterNone, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::NoneRight.name())
		{
			m_input.append(new HLookTransEvent(HLookTrans::NoneRight, tokens.at(1).toInt()));
//			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::NoneRight, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::RightNone.name())
		{
			m_input.append(new HLookTransEvent(HLookTrans::RightNone, tokens.at(1).toInt()));
//			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::RightNone, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HLookTrans::NoneNone.name())
		{
			m_input.append(new HLookTransEvent(HLookTrans::NoneNone, tokens.at(1).toInt()));
//			m_lookTransitions.append( QPair< const HLookTrans*, int>(&HLookTrans::NoneNone, tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HEventType::HEventLookEnabled.name())
		{
			m_input.append(new HLookEnabledEvent(tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HEventType::HEventAGLookEnabled.name())
		{
			m_input.append(new HAGLookEnabledEvent(tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HEventType::HEventLookDisabled.name())
		{
			m_input.append(new HLookDisabledEvent(tokens.at(1).toInt()));
		}
		else if (tokens.at(0) == HEventType::HEventAttention.name())
		{
			m_input.append(new HAttentionEvent(tokens.at(1).toInt()));
		}
		else
		{
			qDebug() << "Skipping line: " << line;
		}
	}
	return bval;
}


HTestingLookDetector::~HTestingLookDetector()
{
};


