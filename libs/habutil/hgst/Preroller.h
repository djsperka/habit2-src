/*
 * Preroller.h
 *
 *  Created on: Oct 31, 2018
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_PREROLLER_H_
#define LIBS_HABUTIL_HGST_PREROLLER_H_

#include <QString>
#include <QPair>
#include <QList>

struct _PTS
{
	QString name;
	int trial;
	int stimid;

	bool operator==(const _PTS& pts) const
	{
		if (&pts == this) return true;
		return (pts.name == name && pts.trial == trial && pts.stimid == stimid);
	}
};


class Preroller
{
	class _phsthab
	{
	public:
		_phsthab(const QString& n, const QList<int>& list, bool bIsHab): name(n), stimidList(list), isHabPhase(bIsHab) {};
		QString name;			// phase name
		QList<int> stimidList;	// ordered (by trial) list of stimid for this phase.
		bool isHabPhase;		// is phase a habituation phase?
	};

	int m_nInPhase;			// number of stim in current phase to preroll
	int m_nInNextPhase;		// number of stim in next phase to preroll (when current phase is a habituation phase)


	QList<_phsthab> m_phases;
	QList<_PTS> m_prerolledList;		// stimuli that have been prerolled
	QList<_PTS> m_pendingPrerolledList;		// phase/trial to be prerolled, but not actually prerolled yet (must look up stim id)
	QList<int> m_pendingCleanupList;	// to be cleaned up...

	bool hasPhase(const QString& name, int *index=NULL);
	const _phsthab& phase(int index) { return m_phases.at(index); };
	void addToPending(const QString& name, int trial, int stimid);

	void removeFromPrerolled(const QString& phaseName, int trial);

	// append the list to the pending list. Check first if the elements exist already - don't add multiple times.
	// Calls here won't need to check if trials were already added to list.
	void appendToPending(const QList<_PTS>& l);

	// append the phase/trial/stimid to the prerolled list. Check first if the element is already in list.
	void appendToPrerolled(const _PTS& pts);

	// Get the next 'n' trials following phase/trial (do not include phase/trial).
	QList<_PTS> getNextTrials(int n, const QString& phase, int trial);

	// Get first 'n' trials in phase.
	QList<_PTS> getNextTrials(int n, const QString& phase);

	// preroll anything in the pending list. Put in prerolled list (don't wait around to find out if they actually prerolled)
	// TODO: Move to prerolled list when async done message received?
	void processPendingList();

	// Any stimuli in the cleanup list that are also in the pending preroll list should not be cleaned up.
	// Instead, rewind and put them back into the prerolled list. Remove the first instance found in the pending
	// list. If not in the pending list just clean it up. The cleanup list is cleared at the end of this method.
	void processCleanupList();





public:
	Preroller(int nInPhase = 2, int nInNextPhase = 1);
	virtual ~Preroller();

	// Add a list of ordered stimuli (ordered by trial)  from phase 'name' to the stimuli that is overseen here.
	void addPhase(const QString& name, QList<int> stimidList, bool bIsHabituationPhase)
	{
		m_phases.append(_phsthab(name, stimidList, bIsHabituationPhase));
	}

	// Update the list of stimuli that must be prerolled given the phase/trial args. This should be called
	// when the stimulus for the phase/trial is known to be running (and hence need not be prerolled).
	// No prerolling is done here - call process() for that.
	// The trial number is used to determine the stimuli for the next 'm_nInPhase' stimuli which should be prerolled.
	// The call with the default value for the trial number (e.g. updatePrerollList("PhaseName") ) is to be used
	// to preroll the 'm_nInNextPhase' stimuli for the phase. This is intended to be used at the start of the experiment - to
	// preroll the first phase, AND when we need to preroll the next phase because the current phase is a habituation phase (and
	// consequently it might end abruptly - such phases can end before all trials are complete).
	void updatePrerollList(const QString& phaseName, int trial=-1);

	// add a stimulus to the pending cleanup list
	// cleanups are not processed here
	void addToCleanupList(int id);

};

#endif /* LIBS_HABUTIL_HGST_PREROLLER_H_ */
