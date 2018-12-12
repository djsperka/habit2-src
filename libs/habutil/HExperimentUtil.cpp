/*
 * HExperimentUtil.cpp
 *
 *  Created on: May 15, 2014
 *      Author: Oakeslab
 */
#include <QFinalState>

#include "HExperimentUtil.h"
#include "HLookDetectorUtil.h"
#include "HExperimentUtil.h"
#include "HPhase.h"
#include "HTrialGenerator.h"
#include "HPhaseCriteriaUtil.h"
#include "HStateMachine.h"

using namespace Habit;

HStateMachine* createExperiment(QWidget *w, const Habit::RunSettings& runSettings, const Habit::ExperimentSettings& experimentSettings, HLookDetector* pld, HGMM* pmm, HEventLog& log, bool bTestingInput)
{
	Q_UNUSED(w);
	HStateMachine *psm;

	// Need to know if AG is used. If it is, add attention getter settings to media manager
	if (	experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed() ||
			experimentSettings.getAttentionGetterSettings().isFixedISI() ||
			experimentSettings.getAttentionGetterSettings().isSoundOnly())
	{
		pmm->addAG(experimentSettings.getAttentionGetterSettings().getAttentionGetterStimulus(), experimentSettings.getAttentionGetterSettings().isSoundOnly());
	}

	// This is a single super-state that holds all the phases.
	HExperiment* sExperiment = new HExperiment(log, *pmm, *pld);
	// Construct state machine.
	psm = new HStateMachine(*sExperiment);
	psm->addState(sExperiment);
	psm->setInitialState(sExperiment);
	QFinalState* sFinal = new QFinalState;
	psm->addState(sFinal);
	sExperiment->addTransition(sExperiment, SIGNAL(finished()), sFinal);


	// Create phases and connect them by an automatic transition. The last phase is connected to the final state
	// for the experiment.
	// Each HPhase gets a list of stimuli, as pairs of <int, StimulusSettings>, and these are used in order as the
	// stimuli for that phase. The <int> part is passed to the media manager as the key to play that stimulus.
	//
	//

	QListIterator<HPhaseSettings> phaseIterator = experimentSettings.phaseIterator();
	QState* plast = NULL;
	while (phaseIterator.hasNext())
	{
		HPhaseCriteria* pcrit = (HPhaseCriteria *)NULL;
		HPhase* pHPhase = (HPhase *)NULL;
		const HPhaseSettings& ps = phaseIterator.next();
		QList< QPair<int, QString> > stimidListOrdered;

		if (ps.getIsEnabled())
		{
			// Now get the order list and randomization selections from HRunSettings.
			// These are saved in a PhaseRunSettings struct.
			// This order list should consist of numbers from 0...n-1, where
			// n is the number of stimuli configured for this phase.
			Q_ASSERT(runSettings.map().contains(ps.getSeqno()));
			PhaseRunSettings prs(runSettings.map().value(ps.getSeqno()));

			stimidListOrdered.clear();
			populateMediaManager(pmm, ps, prs, stimidListOrdered);
			pcrit = createPhaseCriteria(ps.habituationSettings());
			pHPhase = new HPhase(*sExperiment, pcrit, log, stimidListOrdered, ps, experimentSettings.getHLookSettings(), experimentSettings.getAttentionGetterSettings(), bTestingInput);

			if (plast)
			{
				plast->addTransition(plast, SIGNAL(finished()), pHPhase);
				plast = pHPhase;
			}
			else
			{
				sExperiment->setInitialState(pHPhase);
				plast = pHPhase;
			}
		}
	}
	QFinalState* sExperimentFinal = new QFinalState(sExperiment);
	if (plast)
	{
		plast->addTransition(plast, SIGNAL(finished()), sExperimentFinal);
	}
	else
	{
		// This is a trivial case
		sExperiment->setInitialState(sExperimentFinal);
	}

	// Store the stimulus settings events in the log
	QMapIterator<unsigned int, HPipeline *> it(pmm->pipelineMap());
	while (it.hasNext())
	{
	    it.next();
	    log.append(new HStimulusSettingsEvent(pmm->getStimulusSettings(it.key()), it.key()));
	}
#if 0
	QMapIterator<unsigned int, const Habit::StimulusSettings *> it(pmm->pmap());
	while (it.hasNext())
	{
		it.next();
		log.append(new HStimulusSettingsEvent(*it.value(), it.key()));
	}
#endif
	return psm;
}

void getOrderedStimidList(HGMM* pmm, const HPhaseSettings& ps, const Habit::RunSettings& runSettings, QList< QPair<int, QString> >& stimidListOrdered)
{
	QList<unsigned int> stimidListInitial;
	QList< QPair<int, QString> > list;

	// Get the context stim list. It is a list of keys (used to call pmm->stim(key), e.g.)
	// in the order that they were added to the stim list, which is the same as the
	// order they are pulled from the DB.
	// When the actual order of presentation is generated (stimidListOrdered), the algorithm
	// need only generate a 0-based list -- the elements of that list are used to fetch the actual
	// MM keys to use in the experiment.
	stimidListInitial = pmm->getContextStimList(ps.getSeqno());

	Q_ASSERT(runSettings.map().contains(ps.getSeqno()));
	PhaseRunSettings prs(runSettings.map().value(ps.getSeqno()));
	list = prs.getOrderList();
	HTrialGenerator htg(list.size(), prs.isOrderRandomized(), prs.getRandomizeMethod()==1);
	for (int i=0; i<ps.habituationSettings().getNTrials(); i++)
	{
		// Note: prior to version 2.1.0, the line below (with the "-1") was used.
		// That was because the way orders were specified had used 1-based indexing.
		// With the addition of configured orders I've done away with 1-based in lieu
		// of regular old 0-based indices to eliminate a LOT of confusion in
		// manipulating the orders.
		//stimidListOrdered.append(stimidListInitial.at(list.at(htg.next()) - 1));
		int itmp = htg.next();
		stimidListOrdered.append(QPair<int, QString>(stimidListInitial.at(list.at(itmp).first), list.at(itmp).second));
	}
}

void populateMediaManager(HGMM* pmm, const HPhaseSettings& ps, const Habit::PhaseRunSettings& prs, QList< QPair<int, QString> >& stimidListOrdered)
{
	QList<unsigned int> stimidListInitial;
	QList< QPair<int, QString> > list;

	// Add stimuli for this phase to the media manager. The MM will keep an ordered list using the seqno (which
	// distinguishes the phase) as the 'contextStimList'.
	pmm->addStimuli(ps.stimuli(), ps.getSeqno());

	// Get the context stim list. It is a list of keys (used to call pmm->stim(key), e.g.)
	// in the order that they were added to the stim list, which is the same as the
	// order they are pulled from the DB.
	// When the actual order of presentation is generated (stimidListOrdered), the algorithm
	// need only generate a 0-based list -- the elements of that list are used to fetch the actual
	// MM keys to use in the experiment.
	stimidListInitial = pmm->getContextStimList(ps.getSeqno());

	// Now get the order list from HRunSettings.
	// This order list should consist of numbers from 0...n-1, where
	// n is the number of stimuli configured for this phase.
	list = prs.getOrderList();

	// HTrialGenerator will generate a sequence of stimuli according to the run settings/randomization/etc.
	HTrialGenerator htg(list.size(), prs.isOrderRandomized(), prs.getRandomizeMethod()==1);
	for (int i=0; i<ps.habituationSettings().getNTrials(); i++)
	{
		// Note: prior to version 2.1.0, the line below (with the "-1") was used.
		// That was because the way orders were specified had used 1-based indexing.
		// With the addition of configured orders I've done away with 1-based in lieu
		// of regular old 0-based indices to eliminate a LOT of confusion in
		// manipulating the orders.
		//stimidListOrdered.append(stimidListInitial.at(list.at(htg.next()) - 1));
		int itmp = htg.next();
		stimidListOrdered.append(QPair<int, QString>(stimidListInitial.at(list.at(itmp).first), list.at(itmp).second));
	}

}
