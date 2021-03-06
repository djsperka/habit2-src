/*
 *  HExperiment.h
 *  habutil
 *
 *  Created by Oakes Lab on 1/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HEXPERIMENT_H_
#define _HEXPERIMENT_H_

#include "HLogState.h"
#include "HGMM.h"
#include <QMap>
#include "stimulussettings.h"
#include "stimulisettings.h"
class HLookDetector;
class HPhase;

// Number of stimuli to preroll
#define PREROLL_PADDING 2

// dumb little pod o data representing a specific phase/trial.
// Used as unique key for specifying stimuli from within the FSM classes.

struct PhT
{
	int context;
	unsigned int trial;
	PhT(): context(0), trial(0) {};
	PhT(unsigned int c, int itrial): context(c), trial(itrial) {};
	bool operator==(const PhT& pht) const
	{
		if (&pht == this) return true;
		return (pht.context == context && pht.trial == trial);
	}
	bool operator<(const PhT& pht) const
	{
		if (&pht == this) return false;
		else if (this->context < pht.context || (this->context == pht.context && this->trial < pht.trial)) return true;
		return false;
	}
};
QDebug operator<<(QDebug dbg, const PhT& pht);


// The stimuli for a phase(context) are saved here in the order they will be used. There is one StimulusSettings obj
// per trial - that is the stim played during that trial.
struct PhaseStimStuff
{
	PhaseStimStuff(const Habit::HStimulusSettingsList& l, int c, bool b) : sslist(l), context(c), isHabituation(b) {};
	PhaseStimStuff(const PhaseStimStuff& stuff): sslist(stuff.sslist), context(stuff.context), isHabituation(stuff.isHabituation) {};
	Habit::HStimulusSettingsList sslist;
	int context;
	bool isHabituation;						// true if phase is habituation
};

QDebug operator<<(QDebug dbg, const PhaseStimStuff& pss);


typedef QList< PhaseStimStuff > PhaseStimulusLists;
typedef QList< PhT > PhTList;
typedef QMap<PhT, unsigned int> PhTStimidMap;
typedef QList<unsigned int> StimidList;
typedef QList<QPair<PhT, unsigned int> > PhTStimidPairList;

class PhaseStimStuffVector: public QVector<PhaseStimStuff>
{
public:
	PhaseStimStuffVector(): QVector<PhaseStimStuff>() {}
	virtual ~PhaseStimStuffVector() {}
	bool hasContext(int c);
	const PhaseStimStuff& atContext(int c);
};

class HExperiment: public HLogState
{
	Q_OBJECT

public:
	HExperiment(HEventLog& log, HLookDetector& ld, QState* parent = 0);
	virtual ~HExperiment() {};
	
	HLookDetector& getLookDetector() { return m_ld; };
	const HLookDetector& getLookDetector() const { return m_ld; };

	// FSM hooks on entry/exit from state
	void onExit(QEvent* e);
	void onEntry(QEvent *);

	// add stimuli for phase
	void addPhaseStimulusList(int context, const Habit::HStimulusSettingsList& sslist, bool isHab);

	// debug; print lists of stim
	void printLists();

	// preroll calls
	void prerollAsNeeded(int context);
	void prerollAsNeeded(int context, unsigned int trial);
	void prerollNextPhase(int context_current);

	// stim request and cleanup
	void requestStim(int context, unsigned int trial);
	void requestCleanup(int context, unsigned int trial);
	void requestCleanup(int context);

Q_SIGNALS:
	void phaseStarted(QString phaseName, int context);
	void phaseEnded(QString phaseName, int context);
	void trialStarted(int context, unsigned int trial, unsigned int repeat);

private Q_SLOTS:
	void onAttention();
	void onLook(HLook l);
	void onLookAborted(HLook l);
	void onPhaseStarted(QString phaseName);
	void onPhaseEnded(QString phaseName);

private:
	HLookDetector& m_ld;
	PhaseStimulusLists m_phaseStimulusLists;	// list of phases in the experiment, with the stim for each
	PhTStimidMap m_prerolled;					// curated stimuli that we've prerolled here
	PhTStimidPairList m_cleanup;				// curated stimuli that need to be cleaned up

	PhTList nextTrials(int context, int trial=0, int n=PREROLL_PADDING);	// include given context/trial
	Habit::StimulusSettings getStimulusSettings(const PhT& pht);
	void prerollList(const PhTList& l);
};

#endif
