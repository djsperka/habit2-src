/*
 * Preroller.cpp
 *
 *  Created on: Oct 31, 2018
 *      Author: dan
 */

#include "Preroller.h"
#include "HGMM.h"
#include <QtDebug>

Preroller::Preroller(int nInPhase, int nInNextPhase)
: m_nInPhase(nInPhase)
, m_nInNextPhase(nInNextPhase)
{
	// TODO Auto-generated constructor stub

}

Preroller::~Preroller() {
	// TODO Auto-generated destructor stub
}

void Preroller::updatePrerollList(const QString& phaseName, int trial)
{
	int index;
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
		// - preroll any stim in pending list
		// - cleanup any remaining in cleanup list
		if (trial < 0)
		{
			appendToPending(getNextTrials(m_nInPhase, phaseName));
		}
		else
		{
			removeFromPrerolled(phaseName, trial);
			appendToPending(getNextTrials(m_nInPhase, phaseName, trial));
		}
		processCleanupList();
		processPendingList();
	}
}

void Preroller::processCleanupList()
{
	int idToBeCleaned;
	int indexFound = -1;

	QList<int>::iterator i;
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
			appendToPrerolled(*j);

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
// TODO need mutable list iterator. Must remove as we iterate thru pending preroll list.
void Preroller::processPendingList()
{
	_PTS pts;
	foreach(pts, m_pendingPrerolledList)
	{
		if (!m_prerolledList.contains(pts))
		{
			HGMM::instance().preroll(pts.stimid);
			m_prerolledList.append(pts);
		}
	}
	m_pendingPrerolledList.clear();
}

void Preroller::addToCleanupList(int id)
{
	m_pendingCleanupList.append(id);
}

void Preroller::removeFromPrerolled(const QString& phaseName, int trial)
{
	QList<_PTS>::iterator j;
	for (j=m_prerolledList.begin(); j != m_prerolledList.end() && j->name != phaseName && j->trial != trial; j++) {};
	if (j != m_prerolledList.end())
		m_prerolledList.erase(j);
}

void Preroller::appendToPrerolled(const _PTS& pts)
{
	QList<_PTS>::iterator j;
	for (j=m_prerolledList.begin(); j != m_prerolledList.end() && j->name != pts.name && j->trial != pts.trial; j++) {};
	if (j == m_prerolledList.end())
		m_prerolledList.append(pts);
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

QList<_PTS> Preroller::getNextTrials(int n, const QString& phase, int trial)
{
	QList<_PTS> l;



	return l;
}
