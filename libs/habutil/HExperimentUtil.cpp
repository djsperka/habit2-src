/*
 * HExperimentUtil.cpp
 *
 *  Created on: May 15, 2014
 *      Author: Oakeslab
 */
#include <QFinalState>
#include <QList>
#include "HExperimentUtil.h"
#include "HLookDetectorUtil.h"
#include "HExperimentUtil.h"
#include "HPhase.h"
#include "HTrialGenerator.h"
#include "HPhaseCriteriaUtil.h"
#include "HStateMachine.h"
#include "HExperiment.h"

using namespace Habit;

HStateMachine* createExperiment(QWidget *w, const Habit::RunSettings& runSettings, const Habit::ExperimentSettings& experimentSettings, HGMM *phgmm, HLookDetector* pld, HEventLog& log, bool bTestingInput)
{
	Q_UNUSED(w);
	HStateMachine *psm;

	// This is a single super-state that holds all the phases.
	HExperiment* sExperiment = new HExperiment(log, *pld, *phgmm);

	// Construct state machine.
	psm = new HStateMachine(*sExperiment);
	psm->addState(sExperiment);
	psm->setInitialState(sExperiment);
	QFinalState* sFinal = new QFinalState;
	psm->addState(sFinal);
	sExperiment->addTransition(sExperiment, SIGNAL(finished()), sFinal);

	// If there is an attention getter, we add it to the MM now. Below, notice that the stimuli
	// are not added to the MM directly, but the StimulusSettings are added to the Experiment in the
	// order they will be presented. The experiment will coordinate preroll for the stimuli. The
	// attention getter is different, though, it is used repeatedly in each phase (if an AG is used
	// at all), and so its more appropriate to add it directly.

	if (experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed())
	{
		phgmm->addAG(experimentSettings.getAttentionGetterSettings().getAttentionGetterStimulus(), experimentSettings.getAttentionGetterSettings().isSoundOnly());
	}

	// Create phases and connect them by an automatic transition. The last phase is connected to the final state
	// for the experiment.
	// Each HPhase gets a list of stimuli, as pairs of <int, StimulusSettings>, and these are used in order as the
	// stimuli for that phase. The <int> part is passed to the media manager as the key to play that stimulus.
	//
	//

	QListIterator<HPhaseSettings> phaseIterator = experimentSettings.phaseIterator();
	QState* plast = NULL;
	int iFirstPhaseContext=-1;	// grab the context of the first phase, for prerolling that phase.

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

//			stimidListOrdered.clear();
//			populateMediaManager(ps, prs, stimidListOrdered);
			pcrit = createPhaseCriteria(ps.habituationSettings());
			pHPhase = new HPhase(*sExperiment, pcrit, log, ps, experimentSettings.getHLookSettings(), experimentSettings.getAttentionGetterSettings(), bTestingInput);

			if (plast)
			{
				plast->addTransition(plast, SIGNAL(finished()), pHPhase);
				plast = pHPhase;
			}
			else
			{
				sExperiment->setInitialState(pHPhase);
				plast = pHPhase;
				iFirstPhaseContext = ps.getSeqno();
			}

			// Now tell the experiment state that this phase has a certain order of stimuli.
			// This list (and the same info from other phases, in order) assists the HExperiment
			// to manage prerolling and cleaning up stim pipelines.

			Habit::HStimulusSettingsList sslist(getOrderOfStimuli(prs, ps.habituationSettings().getNTrials(), ps.stimuli()));
			sExperiment->addPhaseStimulusList(ps.getSeqno(),
					sslist,
					(ps.habituationSettings().getHabituationType()==HHabituationType::HHabituationTypeCriterion ||
							ps.habituationSettings().getHabituationType()==HHabituationType::HHabituationTypeTotalLookingTime));
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

	// preroll stimuli for the first phase
	sExperiment->prerollAsNeeded(iFirstPhaseContext);

	// Store the stimulus settings events in the log
	QMapIterator<unsigned int, HPipeline *> it(phgmm->pipelineMap());
	while (it.hasNext())
	{
	    it.next();
	    log.append(new HStimulusSettingsEvent(phgmm->getStimulusSettings(it.key()), it.key()));
	}

	sExperiment->printLists();

	return psm;
}

//void getOrderedStimidList(const HPhaseSettings& ps, const Habit::RunSettings& runSettings, QList< QPair<int, QString> >& stimidListOrdered)
//{
//	QList<unsigned int> stimidListInitial;
//	QList< QPair<int, QString> > list;
//
//	// Get the context stim list. It is a list of keys (used to call pmm->stim(key), e.g.)
//	// in the order that they were added to the stim list, which is the same as the
//	// order they are pulled from the DB.
//	// When the actual order of presentation is generated (stimidListOrdered), the algorithm
//	// need only generate a 0-based list -- the elements of that list are used to fetch the actual
//	// MM keys to use in the experiment.
//	stimidListInitial = HGMM::instance().getContextStimList(ps.getSeqno());
//
//	Q_ASSERT(runSettings.map().contains(ps.getSeqno()));
//	PhaseRunSettings prs(runSettings.map().value(ps.getSeqno()));
//	list = prs.getOrderList();
//	HTrialGenerator htg(list.size(), prs.isOrderRandomized(), prs.getRandomizeMethod()==1);
//	for (int i=0; i<ps.habituationSettings().getNTrials(); i++)
//	{
//		// Note: prior to version 2.1.0, the line below (with the "-1") was used.
//		// That was because the way orders were specified had used 1-based indexing.
//		// With the addition of configured orders I've done away with 1-based in lieu
//		// of regular old 0-based indices to eliminate a LOT of confusion in
//		// manipulating the orders.
//		//stimidListOrdered.append(stimidListInitial.at(list.at(htg.next()) - 1));
//		int itmp = htg.next();
//		stimidListOrdered.append(QPair<int, QString>(stimidListInitial.at(list.at(itmp).first), list.at(itmp).second));
//	}
//}

Habit::HStimulusSettingsList getOrderOfStimuli(const PhaseRunSettings& prs, unsigned int ntrials, const Habit::StimuliSettings& ss)
{
	qDebug() << "get Order of stimuili, ntrials " << ntrials;
	Habit::HStimulusSettingsList sslist;
	QList< QPair<int, QString> > list;
	list = prs.getOrderList();
	HTrialGenerator htg(list.size(), prs.isOrderRandomized(), prs.getRandomizeMethod()==1);
	for (int itrial=0; itrial<ntrials; itrial++)
	{
		int itmp = htg.next();

		// itmp is an index into the stimuli settings list
		sslist.append(ss.stimuli().at(list.at(itmp).first));
		//no sslist.append(ss.stimuli().at(itmp));
	}
	return sslist;
}


#if 0
void populateMediaManager(const HPhaseSettings& ps, const Habit::PhaseRunSettings& prs, QList< QPair<int, QString> >& stimidListOrdered)
{
	QList<unsigned int> stimidListInitial;
	QList< QPair<int, QString> > list;

	// Add stimuli for this phase to the media manager. The MM will keep an ordered list using the seqno (which
	// distinguishes the phase) as the 'contextStimList'.
	HGMM::instance().addStimuli(ps.stimuli(), ps.getSeqno());

	// Get the context stim list. It is a list of keys (used to call pmm->stim(key), e.g.)
	// in the order that they were added to the stim list, which is the same as the
	// order they are pulled from the DB.
	// When the actual order of presentation is generated (stimidListOrdered), the algorithm
	// need only generate a 0-based list -- the elements of that list are used to fetch the actual
	// MM keys to use in the experiment.
	stimidListInitial = HGMM::instance().getContextStimList(ps.getSeqno());

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
#endif
