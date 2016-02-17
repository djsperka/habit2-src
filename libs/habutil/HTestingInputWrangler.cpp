/*
 * HTestingInputWrangler.cpp
 *
 *  Created on: Oct 7, 2015
 *      Author: Oakeslab
 */

#include <QListIterator>
#include "HTestingInputWrangler.h"
#include "HElapsedTimer.h"
HEventLog HTestingInputWrangler::m_staticDummyEventLog;



HTestingInputWrangler::HTestingInputWrangler()
: QObject()
, m_ptimer(NULL)
, m_pCurrentEventLogMap(NULL)
, m_pEventLog(&HTestingInputWrangler::m_staticDummyEventLog)
, m_inputIterator(HTestingInputWrangler::m_staticDummyEventLog)
, m_pLD(NULL)
, m_offsetTime(0)
, m_lastCheckTime(0)
, m_bIsEnabled(false)
{
	m_ptimer = new QTimer(this);
	m_ptimer->setInterval(0);
	QObject::connect(m_ptimer, SIGNAL(timeout()), this, SLOT(check()));
}

void HTestingInputWrangler::enable(HLookDetector *pLD, const HExperiment* pExpt)
{
	Q_ASSERT(pLD);
	m_pLD = pLD;
	connect(m_pLD, SIGNAL(lookDetectorEnabled()), this, SLOT(lookDetectorEnabled()));
	connect(m_pLD, SIGNAL(lookDetectorDisabled()), this, SLOT(lookDetectorDisabled()));
	connect(pExpt, SIGNAL(phaseStarted(QString)), this, SLOT(phaseStarted(QString)));
	connect(pExpt, SIGNAL(trialStarted(int, int)), this, SLOT(trialStarted(int, int)));
	connect(pExpt, SIGNAL(exited()), this, SLOT(experimentFinished()));
	m_bIsEnabled = true;
}


void HTestingInputWrangler::stop()
{
	if (m_ptimer && m_ptimer->isActive()) m_ptimer->stop();
}

bool HTestingInputWrangler::load(QFile& inputFile)
{
	bool b = false;


	/*
	 * Expecting input like this....
	 * phase trial repeat trans ms
	 * phase trial repeat trans ms
	 * ...
	 *
	 * trials numbered starting with 1
	 * repeat numbered starting with 0
	 * phase PreTest|Habituation|Test
	 * trans NoneLeft|LeftNone|NoneCenter|CenterNone|NoneRight|RightNone
	 */

	qDebug() << "HTestingInputWrangler: loading input file " << inputFile.fileName();
	int num = 0;
    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
    	b = true;
    	QTextStream in(&inputFile);
    	while (!in.atEnd())
    	{
    		QString line = in.readLine();
    		if (!processLine(line))
    		{
    			qDebug() << "Error in input file at line " << num << ": " << line;
    		}
    		else
    		{
        		num++;
    		}
    	}
    }
    if (b)
    {
    	qDebug() << "Loaded " << num << " events from " << inputFile.fileName();
    }

    dump();

    return b;
};

bool HTestingInputWrangler::processLine(const QString& line)
{
	// If line begins with # or is blank then skip it
	if (line.trimmed().isEmpty() || line.startsWith('#'))
	{
		// skip line
	}
	else if (line.startsWith('!'))
	{
		// TODO: Can have special handling for other input?
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
		if (tokens.count() != 5)
		{
			qDebug() << "HTestingInputWrangler::processLine - expecting 5 tokens, found " << tokens.count();
			return false;
		}

		// First token should be a phase name recognized by getPhaseType()
		const HPhaseType& phaseType = getPhaseType(tokens.at(0));
		if (phaseType == HPhaseType::UnknownPhase)
		{
			qDebug() << "HTestingInputWrangler::processLine - unrecognized phase type \"" << tokens.at(0) << "\"";
			return false;
		}

		// Get the TrialEventLogMap from the PhaseTrialEventLogMapMap;
		if (!m_map.contains(phaseType))
		{
			TrialEventLogMap telm;
			m_map.insert(phaseType, telm);
		}
		TrialEventLogMap* pTrialEventLogMap = &m_map[phaseType];

		bool bt, br, bm;
		int itrial, irepeat, ims;

		itrial = tokens.at(1).toInt(&bt);
		irepeat = tokens.at(2).toInt(&br);
		ims = tokens.at(4).toInt(&bm);

		if (!bt)
		{
			qDebug() << "HTestingInputWrangler::processLine - cannot parse trial number \"" << tokens.at(1) << "\"";
			return false;
		}

		if (!br)
		{
			qDebug() << "HTestingInputWrangler::processLine - cannot parse repeat number \"" << tokens.at(2) << "\"";
			return false;
		}

		if (!bm)
		{
			qDebug() << "HTestingInputWrangler::processLine - cannot parse ms value \"" << tokens.at(4) << "\"";
			return false;
		}

		// Now we can fetch the HEventLog for the trial/repeat given
		QPair<int,int> key(itrial, irepeat);
		if (!pTrialEventLogMap->contains(key))
		{
			pTrialEventLogMap->insert(key, new HEventLog());
		}
		HEventLog *pEventLog = pTrialEventLogMap->value(key);


		// no LookTransType - do this by hand
		if (tokens.at(3) == HLookTrans::NoneLeft.name())
		{
			pEventLog->append(new HLookTransEvent(HLookTrans::NoneLeft, ims));
		}
		else if (tokens.at(3) == HLookTrans::LeftNone.name())
		{
			pEventLog->append(new HLookTransEvent(HLookTrans::LeftNone, ims));
		}
		else if (tokens.at(3) == HLookTrans::NoneCenter.name())
		{
			pEventLog->append(new HLookTransEvent(HLookTrans::NoneCenter, ims));
		}
		else if (tokens.at(3) == HLookTrans::CenterNone.name())
		{
			pEventLog->append(new HLookTransEvent(HLookTrans::CenterNone, ims));
		}
		else if (tokens.at(3) == HLookTrans::NoneRight.name())
		{
			pEventLog->append(new HLookTransEvent(HLookTrans::NoneRight, ims));
		}
		else if (tokens.at(3) == HLookTrans::RightNone.name())
		{
			pEventLog->append(new HLookTransEvent(HLookTrans::RightNone, ims));
		}
		else if (tokens.at(3) == HLookTrans::NoneNone.name())
		{
			pEventLog->append(new HLookTransEvent(HLookTrans::NoneNone, tokens.at(1).toInt()));
		}
		else
		{
			qDebug() << "Unknown LookTransType: " << tokens.at(3);
			return false;
		}

		qDebug() << "There are " << pEventLog->count() << " events so far...";

	}
	return true;
}

void HTestingInputWrangler::phaseStarted(QString sPhase)
{
	Q_ASSERT(m_bIsEnabled);
	// when phase starts, get
	qDebug() << "HTestingInputWrangler::phaseStarted(\"" << sPhase << "\")";
	if (m_map.contains(getPhaseType(sPhase)))
	{
		m_pCurrentEventLogMap = &m_map[getPhaseType(sPhase)];
		qDebug() << "There are events for this phase.";
	}
	else
	{
		m_pCurrentEventLogMap = NULL;
		qWarning() << "No look transitions were found for phase " << sPhase;
	}
}

void HTestingInputWrangler::trialStarted(int itrial, int irepeat)
{
	Q_ASSERT(m_bIsEnabled);
	// initialize pointer to the event log found in m_pCurrentEventLogMap for this trial/repeat
	qDebug() << "HTestingInputWrangler::trialStarted(" << itrial << "/" << irepeat << ")";
	if (!m_pCurrentEventLogMap)
	{
		qWarning() << "HTestingInputWrangler::trialStarted(): No look transitions found for current phase";
		m_pEventLog = NULL;
	}
	else if (!m_pCurrentEventLogMap->contains(QPair<int, int>(itrial, irepeat)))
	{
		qWarning() << "No look transitions for trial/repeat " << itrial << "/" << irepeat;
		m_pEventLog = NULL;
	}
	else
	{
		qDebug() << "There are " << (m_pCurrentEventLogMap->value(QPair<int, int>(itrial, irepeat)))->count() << " events for this trial/repeat.";
		m_inputIterator = HMutableEventLogIterator(*m_pCurrentEventLogMap->value(QPair<int,int>(itrial, irepeat)));
	}

}

void HTestingInputWrangler::lookDetectorEnabled()
{
	Q_ASSERT(m_bIsEnabled);
	m_offsetTime = HElapsedTimer::elapsed();
	m_ptimer->start();
	qDebug() << "HTestingInputWrangler::lookDetectorEnabled() - timer started.";
}

void HTestingInputWrangler::lookDetectorDisabled()
{
	Q_ASSERT(m_bIsEnabled);
	m_ptimer->stop();
	qDebug() << "HTestingInputWrangler::lookDetectorEnabled() - timer stopped.";
}

void HTestingInputWrangler::check()
{
	Q_ASSERT(m_bIsEnabled);
	int t = HElapsedTimer::elapsed();
	int tRelative = t - m_offsetTime;	// compare to tEvent-tEventOffsetTime
	if (t == m_lastCheckTime) return;

	if (tRelative % 100 == 0)
	{
		if (m_inputIterator.hasNext())
		{
			qDebug() << "check() " << tRelative << ": next " << m_inputIterator.peekNext()->timestamp();
		}
		else
		{
			qDebug() << "check() " << tRelative << ": NO NEXT";
		}
	}

	while (	m_inputIterator.hasNext() &&
			m_inputIterator.peekNext()->timestamp() <= tRelative)
	{
		if (m_inputIterator.peekNext()->type() == HEventType::HEventLookTrans)
		{
			HLookTransEvent* elt = static_cast<HLookTransEvent*>(m_inputIterator.peekNext());
			if (m_pLD)
				m_pLD->addTrans(elt->transtype(), t);
		}
		else
		{
			qDebug() << "HTestingLookDetector::check() @ " << t << ": skip event " << m_inputIterator.peekNext()->eventCSV();
		}
		m_inputIterator.next();
	}
	m_lastCheckTime = t;
};


void HTestingInputWrangler::dump()
{
	QList< HPhaseType > keys = m_map.keys();
	qDebug() << "HTestingInputWrangler events loaded: ";
	QListIterator< HPhaseType > itPhases(keys);
	while (itPhases.hasNext())
	{
		HPhaseType type = itPhases.next();
		qDebug() << type.name() << endl;

		// Get the map for this phase.
		TrialEventLogMap *pEventLogMap = &m_map[type];
		QList< QPair<int, int> > trkeys = pEventLogMap->keys();
		QListIterator<QPair<int, int> >itTrialRepeat(trkeys);
		while (itTrialRepeat.hasNext())
		{
			QPair<int, int> tr = itTrialRepeat.next();
			qDebug() << "Trial/repeat " << tr.first << "/" << tr.second;

		}

	}
	return;
};