/*
 * Preroller.cpp
 *
 *  Created on: Oct 31, 2018
 *      Author: dan
 */

#include "Preroller.h"
#include "HGMM.h"


QDebug operator<<(QDebug dbg, const _PTS& pts)
{
	dbg.nospace() << "PTS: " << pts.name << ":" << pts.trial << "(" << pts.stimid << ")";
	return dbg.nospace();
}


Preroller::Preroller(int nInPhase)
: m_nInPhase(nInPhase)
, m_ptsNowPlaying()
{
	// TODO Auto-generated constructor stub

}

Preroller::~Preroller() {
	// TODO Auto-generated destructor stub
}

void Preroller::updatePrerollList(const QString& phaseName, int trial)
{
	int index;
	qDebug() << "update preroll list:";
	if (hasPhase(phaseName, &index))
	{
		const _phsthab& ph = phase(index);

		// if the trial number is negative, it means preroll the start
		// of this phase. This would be called when a habituation trial starts, and
		// the NEXT phase should have its first couple of stimuli prerolled.
		//
		// Otherwise, the args indicate that the stim for the given phase/trial has started.
		// We take the following steps:
		// - remove phase/trial from prerolled list
		// - add N trials to pending preroll list. These are the next N trials in this phase,
		//   and may have to include some trials from the next phase to round out N
		// - check cleanup list - if any of the stimuli are also in the pending list,
		//   then rewind and move to prerolled list. The FIRST entry found should be moved
		//   (in case its queued multiple times).
		// - remove the current trial stim from the prerolled list. The assumption is that it was
		//   prerolled before being played (updatePrerollList should have been called after it was
		//   played. THIS SHOULD BE DONE AFTER CLEANUP! Why? If a stim is repeated on consecutive
		//   trials, it will be in the pending preroll list. After one trial completes, however, it is
		//   added to the cleanup list. Because the same stim is in the pending list, it is rewound and
		//   put back in the prerolled list.
		//
		//   FIXME: logic of handling consecutive stimuli....
		//
		// - preroll any stim in pending list
		// - cleanup any remaining in cleanup list
		if (trial < 0)
		{
			QList<_PTS> l(getNextTrials(phaseName));
			appendToListNoDuplicates(m_pendingPrerolledList, l);
		}
		else
		{
			QList<_PTS> nextTrials = getNextTrials(phaseName, trial);
			foreach(auto pts, getNextTrials(phaseName, trial))
			{
				if (!m_prerolledList.contains(pts) && !m_pendingPrerolledList.contains(pts))
					m_pendingPrerolledList.append(pts);
			}
			removeFromPrerolled(phaseName, trial);
			m_ptsNowPlaying = getPTS(phaseName, trial);
		}

		//processCleanupList();
		processPendingList();
	}
}

void Preroller::processCleanupList()
{
	int idToBeCleaned;
	int indexFound = -1;

	QList<unsigned int>::iterator i;
	for (i=m_pendingCleanupList.begin(); i!= m_pendingCleanupList.end(); i++)
	{
		QList<_PTS>::iterator j;
		for (j=m_pendingPrerolledList.begin(); j != m_pendingPrerolledList.end() && j->stimid != *i; j++) {};
		if (j != m_pendingPrerolledList.end())
		{
			// this id is in the pending preroll list.
			// do not cleanup, instead rewind it and remove the found element from pending preroll list.
			HGMM::instance().rewind(j->stimid);

			// append to prerolled list
			appendToListNoDuplicates(m_prerolledList, *j);

			// and remove from pending preroll list
			m_pendingPrerolledList.erase(j);
		}
		else
		{
			HGMM::instance().cleanup(*i);
		}
	}
	m_pendingCleanupList.clear();
}

// If a stim is in the prerolled list, do not preroll again.
// Same for the 'nowPlaying' stim m_ptsNowPlaying.
// Note that if its in the prerolled list, it may be there because it was prerolled for
// a different phase/trial than the one which put it in the pending list.
// TODO need mutable list iterator. Must remove as we iterate thru pending preroll list.
void Preroller::processPendingList()
{
	_PTS pts;
	unsigned int i=0;
	while (i < m_pendingPrerolledList.size())
	{
		// iterate through the prerolled list, and if the stimid
		// associated with the current item from the pendingPrerolled list
		// is in the prerolled list, then do not preroll it at this time.
		// If it is NOT in the prerolled list, then preroll it.
		bool bFound = false;

		bFound = (m_pendingPrerolledList.at(i).stimid == m_ptsNowPlaying.stimid);
		if (!bFound)
			foreach(pts, m_prerolledList)
				if (m_pendingPrerolledList.at(i).stimid == pts.stimid) bFound = true;

		if (bFound)
			i++;
		else
		{
			HGMM::instance().preroll(m_pendingPrerolledList.at(i).stimid);
			// remove from list, but do NOT increment counter
			appendToListNoDuplicates(m_prerolledList, m_pendingPrerolledList.takeAt(i));
		}
	}
}

void Preroller::addToCleanupList(unsigned int id)
{
	m_pendingCleanupList.append(id);
	processCleanupList();
}

void Preroller::addToCleanupList(const QString& phaseName)
{
	// If anything is in prerolled list with this phase name....
	unsigned int i=0;
	while (i < m_prerolledList.size())
	{
		// if the phase of the item at this index is the same as the one passed,
		// then delete that element from the prerolled list and add it to
		// the cleanup list.
		if (phaseName == m_prerolledList.at(i).name)
		{
			_PTS ptsRemoved = m_prerolledList.takeAt(i);
			qDebug() << "Removing from prerolledList: " << ptsRemoved;
			m_pendingCleanupList.append(ptsRemoved.stimid);
		}
		else
			i++;
	}

	// if for some reason there are still things in the pending preroll list from this phase,
	// remove them from the list. Don't have to clean these up.

	i = 0;
	while (i < m_pendingPrerolledList.size())
	{
		if (phaseName == m_pendingPrerolledList.at(i).name)
		{
			_PTS ptsRemoved = m_pendingPrerolledList.takeAt(i);
			qDebug() << "Removing from pending preroll list: " << ptsRemoved;
		}
		else
			i++;
	}
	processCleanupList();
}

void Preroller::removeFromPrerolled(const QString& phaseName, int trial)
{
	qDebug() << "Remove from prerolled: " << getPTS(phaseName, trial);
	qDebug() << "prerolled list...";
	qDebug() << m_prerolledList;
	m_prerolledList.removeAll(getPTS(phaseName, trial));
	qDebug() << "prerolled list...";
	qDebug() << m_prerolledList;
}

void Preroller::appendToListNoDuplicates(QList<_PTS>& l, const _PTS& pts)
{
	if (!l.contains(pts))
		l.append(pts);
}

void Preroller::appendToListNoDuplicates(QList<_PTS>& l, const QList<_PTS>& ptslist)
{
	QList<_PTS>::iterator j;
	foreach (auto pts, ptslist)
	{
		appendToListNoDuplicates(l, pts);
	}
}
// append the list to the pending list. Check first if the elements exist already - don't add multiple times.
// Calls here won't need to check if trials were already added to list.
void Preroller::appendToPending(const QList<_PTS>& l)
{
	appendToListNoDuplicates(m_pendingPrerolledList, l);
}


bool Preroller::hasPhase(const QString& name, int *index)
{
	int i;
	bool bFound = false;
	for (i=0; i<m_phases.size() && !(bFound = (name == m_phases[i].name)); i++) ;
	if (bFound && index)
		*index = i;
	return bFound;
}

_PTS Preroller::getPTS(const QString& phaseName, int trial)
{
	int index;
	if (hasPhase(phaseName, &index))
	{
		if (trial >= 0 && trial < m_phases.at(index).stimidList.size())
			return _PTS(phaseName, trial, m_phases.at(index).stimidList.at(trial));
	}
	return _PTS(QString(), -1, 0);
}


QList<_PTS> Preroller::getNextTrials(const QString& phase, int trial)
{
	QList<_PTS> l;
	// find phase
	int phaseIndex=-1;
	if (hasPhase(phase, &phaseIndex))
	{
		// 'p' has the phase name, list of stimids (in trial order), and bool indicating
		// whether its a habituation phase.
		_phsthab p(m_phases.at(phaseIndex));
		int istart = trial+1;
		if (trial < 0)
		{
			istart = 0;
		}

		bool bDone = false;	// set to true when we want to get something from
							// the next phase but there are no  more phases
		while (l.size()<m_nInPhase && !bDone)
		{
			for (int i=istart; i < p.stimidList.size() && l.size() < m_nInPhase; i++)
				l.append(_PTS(p.name, i, p.stimidList.at(i)));
			if (l.size() < m_nInPhase)
			{
				phaseIndex++;
				if (phaseIndex < m_phases.size())
				{
					p = m_phases.at(phaseIndex);
					istart = 0;
				}
				else
					bDone = true;
			}
		}

		if (!bDone && p.isHabPhase)
		{
			phaseIndex++;
			if (phaseIndex < m_phases.size())
			{
				p = m_phases.at(phaseIndex);
				// Get a couple of trials from trial which follows a habituation phase, just in case
				for (int i=0; i<m_nInPhase && i<p.stimidList.size(); i++)
					l.append(_PTS(p.name, i, p.stimidList.at(i)));
			}
		}
	}

	return l;
}

void Preroller::printStatus(const QString& s)
{
	qDebug() << ".";
	qDebug() << ".";
	qDebug() << ".";
	qDebug() << s;
	qDebug() << ".";
	qDebug() << ".";
	qDebug() << ".";

	qDebug() << "Preroller status - stimuli lists";
	for (const auto& phsthab : m_phases)
	{
		qDebug() << "Phase " << phsthab.name << " hab? " << phsthab.isHabPhase;
		for (const auto stimid: phsthab.stimidList)
		{
			qDebug() << stimid;
		}
	}

	qDebug() << "Now playing: " << m_ptsNowPlaying;
	qDebug() << "pending preroll request...";
	for (const auto& pts: m_pendingPrerolledList)
		qDebug() << pts;

	qDebug() << "prerolled...";
	for (const auto& pts: m_prerolledList)
		qDebug() << pts;
	qDebug() << "pending cleanup...";
	for (const auto& i: m_pendingCleanupList)
		qDebug() << i;
	qDebug() << "Preroller status - done.";
}
