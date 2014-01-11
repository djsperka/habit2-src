/*
 * HTestingLookDetector.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: Oakeslab
 */

#include "HTestingLookDetector.h"
#include <QTextStream>
#include <QStringList>
#include <QTimer>


HTestingLookDetector::HTestingLookDetector(QFile& inputFile, int minlooktime_ms, int minlookawaytime_ms, HEventLog& log, QWidget* pdialog, bool bUseLeft, bool bUseCenter, bool bUseRight)
: HLookDetector(minlooktime_ms, minlookawaytime_ms, log)
, m_pdialog(pdialog)
, m_bUseLeft(bUseLeft)
, m_bUseCenter(bUseCenter)
, m_bUseRight(bUseRight)
, m_attentionDelay(100)
, m_cumulativeTime(0)
{
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

void HTestingLookDetector::agLookEnabled(bool enabled)
{
	if (enabled)
		QTimer::singleShot(m_attentionDelay, this, SLOT(fireAttention()));
}

void HTestingLookDetector::lookEnabled(bool enabled)
{
	if (enabled && !m_lookTransitions.empty())
	{
		QPair<HLookTrans, int> pair = m_lookTransitions.takeFirst();
		addTrans(pair.first, m_cumulativeTime);
		qDebug() << "Add trans " << pair.first << " at t=" << m_cumulativeTime;
		m_cumulativeTime += pair.second;
		if (isLookEnabled())
			QTimer::singleShot(m_attentionDelay, this, SLOT(fireTransition()));
	}
}

void HTestingLookDetector::fireAttention()
{
	qDebug() << "Emit attention() signal...";
	emit attention();
}

void HTestingLookDetector::fireTransition()
{
	if (isLookEnabled() && !m_lookTransitions.empty())
	{
		QPair<HLookTrans, int> pair = m_lookTransitions.takeFirst();
		addTrans(pair.first, m_cumulativeTime);
		qDebug() << "Add trans " << pair.first << " at t=" << m_cumulativeTime;
		m_cumulativeTime += pair.second;
		if (isLookEnabled() && pair.second > 0)
			QTimer::singleShot(m_attentionDelay, this, SLOT(fireTransition()));
	}
	return;
}



bool HTestingLookDetector::processLine(const QString& line)
{
	bool bval = true;

	QStringList tokens = line.split(",");
	if (tokens.at(0) == "AttentionDelay")
	{
		m_attentionDelay = tokens.at(1).toInt();
	}
	else if (tokens.at(0) == HLookTrans::NoneLeft.name())
	{
		m_lookTransitions.append( QPair< HLookTrans, int>(HLookTrans::NoneLeft, tokens.at(1).toInt()));
	}
	else if (tokens.at(0) == HLookTrans::LeftNone.name())
	{
		m_lookTransitions.append( QPair< HLookTrans, int>(HLookTrans::LeftNone, tokens.at(1).toInt()));
	}
	else if (tokens.at(0) == HLookTrans::NoneCenter.name())
	{
		m_lookTransitions.append( QPair< HLookTrans, int>(HLookTrans::NoneCenter, tokens.at(1).toInt()));
	}
	else if (tokens.at(0) == HLookTrans::CenterNone.name())
	{
		m_lookTransitions.append( QPair< HLookTrans, int>(HLookTrans::CenterNone, tokens.at(1).toInt()));
	}
	else if (tokens.at(0) == HLookTrans::NoneRight.name())
	{
		m_lookTransitions.append( QPair< HLookTrans, int>(HLookTrans::NoneRight, tokens.at(1).toInt()));
	}
	else if (tokens.at(0) == HLookTrans::RightNone.name())
	{
		m_lookTransitions.append( QPair< HLookTrans, int>(HLookTrans::RightNone, tokens.at(1).toInt()));
	}
	else if (tokens.at(0) == HLookTrans::NoneNone.name())
	{
		m_lookTransitions.append( QPair< HLookTrans, int>(HLookTrans::NoneNone, tokens.at(1).toInt()));
	}
	else
	{
		qDebug() << "Error in input file: unknown look trans type: " << line;
		bval = false;
	}
	return bval;
}


HTestingLookDetector::~HTestingLookDetector()
{
	qDebug() << "HTestingLookDetector: ";
};


