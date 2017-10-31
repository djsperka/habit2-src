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
	if (experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed() || experimentSettings.getAttentionGetterSettings().isFixedISI())
	{
		pmm->addAG(experimentSettings.getAttentionGetterSettings().getAttentionGetterStimulus());
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


	// Create phases.
	// Each HPhase gets a list of stimuli, as pairs of <int, StimulusSettings>, and these are used in order as the
	// stimuli for that phase. The <int> part is passed to the media manager as the key to play that stimulus.
	//

	QListIterator<HPhaseSettings> phaseIterator = experimentSettings.phaseIterator();
	QState* plast = NULL;
	while (phaseIterator.hasNext())
	{
		HPhaseCriteria* pcrit = (HPhaseCriteria *)NULL;
		HPhase* pHPhase = (HPhase *)NULL;
		const HPhaseSettings& ps = phaseIterator.next();
		QList< QPair<int, QString> > list;
		QList<unsigned int> stimidListInitial;
		QList< QPair<int, QString> > stimidListOrdered;

		if (ps.getIsEnabled())
		{
			pmm->addStimuli(ps.stimuli(), ps.getSeqno());

			// Get the list of ALL stim keys for the PreTest stimuli.
			// These are in the order that they were added to the stim list, which is the same as the
			// order they are pulled from the DB.
			// Any randomization or other ordering will come later.
			stimidListInitial = pmm->getContextStimList(ps.getSeqno());

			// Now get the order list from HRunSettings.
			// This order list should consist of numbers from 0...n-1, where
			// n is the number of stimuli configured for this phase.
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
	QMapIterator<unsigned int, HGMMPipeline *> it(pmm->pipelineMap());
	while (it.hasNext())
	{
	    it.next();
	    log.append(new HStimulusSettingsEvent(it.value()->stimulusSettings(), it.key()));
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
