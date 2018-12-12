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
#include <QtDebug>

// dumb little pod o data representing a specific phase/trial/stim combination.
// stimid <1 is invalid (initially invalid until trials start
struct _PTS
{
	QString name;
	int trial;
	unsigned int stimid;
	_PTS(): name(), trial(-1), stimid(-1) {};
	_PTS(const QString& sName, int itrial, int istimid): name(sName), trial(itrial), stimid(istimid) {};
	bool operator==(const _PTS& pts) const
	{
		if (&pts == this) return true;
		return (pts.name == name && pts.trial == trial && pts.stimid == stimid);
	}
};
QDebug operator<<(QDebug dbg, const _PTS& pts);


// manage the prerolling and cleanup of stimuli
// Must be initialized with lists of stimulus ids for each phase, and whether each phase is a habituation phase
// (meaning that it can be ended early -- all other phases area assumed to run through all stimuli in list order).
class Preroller
{
	class _phsthab
	{
	public:
		_phsthab(const QString& n, const QList<unsigned int>& list, bool bIsHab): name(n), stimidList(list), isHabPhase(bIsHab) {};
		_phsthab(const _phsthab& p): name(p.name), stimidList(p.stimidList), isHabPhase(p.isHabPhase) {};
		QString name;			// phase name
		QList<unsigned int> stimidList;	// ordered (by trial) list of stimid for this phase.
		bool isHabPhase;		// is phase a habituation phase?
	};

	int m_nInPhase;			// number of stim in current phase to preroll

	QList<_phsthab> m_phases;
	QList<_PTS> m_prerolledList;		// stimuli that have been prerolled
	QList<_PTS> m_pendingPrerolledList;		// phase/trial to be prerolled, but not actually prerolled yet (must look up stim id)
	QList<unsigned int> m_pendingCleanupList;	// to be cleaned up...
	_PTS m_ptsNowPlaying;		// when updatePrerollList is called, the stim for trial specified is now playing
	bool hasPhase(const QString& name, int *index=NULL);
	_PTS getPTS(const QString& phaseName, int trial);
	const _phsthab& phase(int index) { return m_phases.at(index); };
	void addToPending(const QString& name, int trial, unsigned int stimid);

	void removeFromPrerolled(const QString& phaseName, int trial);

	// append the list to the pending list. Check first if the elements exist already - don't add multiple times.
	// Calls here won't need to check if trials were already added to list.
	void appendToPending(const QList<_PTS>& l);

	// append the phase/trial/stimid to the prerolled list. Check first if the element is already in list.
	void appendToPrerolled(const _PTS& pts);


	// preroll anything in the pending list. Put in prerolled list (don't wait around to find out if they actually prerolled)
	// TODO: Move to prerolled list when async done message received?
	void processPendingList();

	// Any stimuli in the cleanup list that are also in the pending preroll list should not be cleaned up.
	// Instead, rewind and put them back into the prerolled list. Remove the first instance found in the pending
	// list. If not in the pending list just clean it up. The cleanup list is cleared at the end of this method.
	void processCleanupList();

	void appendToListNoDuplicates(QList<_PTS>& l, const _PTS& pts);

	void appendToListNoDuplicates(QList<_PTS>& l, const QList<_PTS>& ptslist);




public:
	Preroller(int nInPhase = 2);
	virtual ~Preroller();

	// Add a list of ordered stimuli (ordered by trial)  from phase 'name' to the stimuli that is overseen here.
	// The order in which phases are added should match that used in the experiment.
	// The stimidList should be in the order that the stim are used in that phase.
	void addPhase(const QString& name, QList<unsigned int> stimidList, bool bIsHabituationPhase)
	{
		m_phases.append(_phsthab(name, stimidList, bIsHabituationPhase));
	}

	// Update the list of stimuli that must be prerolled given the phase/trial args. This should be called
	// when the stimulus for the phase/trial is known to be running (and hence need not be prerolled).
	// The list of cleanup and preroll stimids is processed after the preroll list is updated.
	// The trial number is used to determine the stimuli for the next 'm_nInPhase' stimuli which should be prerolled.
	// The call with the default value for the trial number (e.g. updatePrerollList("PhaseName") ) is to be used
	// to preroll the 'm_nInNextPhase' stimuli for the phase. This is intended to be used at the start of the experiment - to
	// preroll the first phase, AND when we need to preroll the next phase because the current phase is a habituation phase (and
	// consequently it might end abruptly - such phases can end before all trials are complete).
	void updatePrerollList(const QString& phaseName, int trial=-1);

	// add a stimulus to the pending cleanup list
	// cleanups are not processed here
	void addToCleanupList(unsigned int id);

	// add any stimuli from this list that are now prerolled to the cleanup list.
	// call this when a phase ends, e.g. a habituation phase that may have prerolled
	// stimuli that it didn't use because the subject habituated.
	void addToCleanupList(const QString& phaseName);

	// Get the next 'n' trials following phase/trial (do not include phase/trial).
	// default arg is for calling on an entire phase: at start of expt, to preroll the first phase,
	// or when a habituation phase starts, and the following phase is prerolled because this phase may
	// end at any given point.
	QList<_PTS> getNextTrials(const QString& phase, int trial=-1);


	const QList<_PTS>& getPrerolledList() { return m_prerolledList; };
	const QList<_PTS>& getPendingPrerolledList() { return m_pendingPrerolledList; };
	const QList<unsigned int>& getPendingCleanupList() { return m_pendingCleanupList; };
	void printStatus(const QString& s);
};

#endif /* LIBS_HABUTIL_HGST_PREROLLER_H_ */
