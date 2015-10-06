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

HStateMachine* createExperiment(QWidget *w, const Habit::RunSettings& runSettings, const Habit::ExperimentSettings& experimentSettings, HLookDetector* pld, HMediaManager* pmm, HEventLog& log)
{
	HPhase* psPreTest = (HPhase *)NULL;
	HPhase* psHabituation = (HPhase *)NULL;
	HPhase* psTest = (HPhase *)NULL;
	HPhaseCriteria* pcritPreTest = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritHabituation = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritTest = (HPhaseCriteria *)NULL;
	HStateMachine *psm;

	// Need to know if AG is used. If it is, add attention getter settings to media manager
	if (experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed())
	{
		pmm->addAG(experimentSettings.getAttentionGetterSettings().getAttentionGetterStimulus());
	}

	// add stimuli for each phase.
	if (experimentSettings.getPreTestPhaseSettings().getIsEnabled())
		pmm->addStimuli(experimentSettings.getPreTestStimuliSettings());
	if (experimentSettings.getHabituationPhaseSettings().getIsEnabled())
		pmm->addStimuli(experimentSettings.getHabituationStimuliSettings());
	if (experimentSettings.getTestPhaseSettings().getIsEnabled())
		pmm->addStimuli(experimentSettings.getTestStimuliSettings());


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
	if (experimentSettings.getPreTestPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = experimentSettings.getPreTestPhaseSettings();
		QList< QPair<int, QString> > list;
		QList<unsigned int> stimidListInitial;
		QList< QPair<int, QString> > stimidListOrdered;

		// Get the list of ALL stim keys for the PreTest stimuli.
		// These are in the order that they were added to the stim list, which is the same as the
		// order they are pulled from the DB.
		// Any randomization or other ordering will come later.
		pmm->getContextStimList(HStimContext::PreTestPhase, stimidListInitial);

		// Now get the order list from HRunSettings.
		// This order list should consist of numbers from 0...n-1, where
		// n is the number of stimuli configured for this phase.
		if (!runSettings.getPretestOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse Pretest order (" << runSettings.getPretestOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), runSettings.isPretestRandomized(), runSettings.getPretestRandomizeMethod()==1);
		for (unsigned int i=0; i<ps.getNTrials(); i++)
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
		pcritPreTest = new HPhaseFixedNCriteria(ps.getNTrials());
		psPreTest = new HPhase(*sExperiment, pcritPreTest, log, stimidListOrdered, ps, experimentSettings.getHLookSettings(), experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed());

	}

	if (experimentSettings.getHabituationPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = experimentSettings.getHabituationPhaseSettings();
		QList< QPair<int, QString> > list;
		QList<unsigned int> stimidListInitial;
		QList< QPair<int, QString> > stimidListOrdered;

		pmm->getContextStimList(HStimContext::HabituationPhase, stimidListInitial);

		if (!runSettings.getHabituationOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse habituation order (" << runSettings.getHabituationOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), runSettings.isHabituationRandomized(), runSettings.getHabituationRandomizeMethod()==1);
		for (unsigned int i=0; i<ps.getNTrials(); i++)
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

		// Create habituation criteria object.
		pcritHabituation = createPhaseCriteria(experimentSettings.getHabituationSettings(), ps.getNTrials());
		psHabituation = new HPhase(*sExperiment, pcritHabituation, log, stimidListOrdered, ps, experimentSettings.getHLookSettings(), experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed());
	}

	if (experimentSettings.getTestPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = experimentSettings.getTestPhaseSettings();
		QList< QPair<int, QString> > list;
		QList<unsigned int> stimidListInitial;
		QList< QPair<int, QString> > stimidListOrdered;

		pmm->getContextStimList(HStimContext::TestPhase, stimidListInitial);
		if (!runSettings.getTestOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse test order (" << runSettings.getTestOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), runSettings.isTestRandomized(), runSettings.getTestRandomizeMethod()==1);
		for (unsigned int i=0; i<ps.getNTrials(); i++)
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

		pcritTest = new HPhaseFixedNCriteria(ps.getNTrials());
		psTest = new HPhase(*sExperiment, pcritTest, log, stimidListOrdered, ps, experimentSettings.getHLookSettings(), experimentSettings.getAttentionGetterSettings().isAttentionGetterUsed());
	}


	// Now assemble the experiment.
	QState* plast = NULL;
	if (psPreTest)
	{
		if (plast)
		{
			plast->addTransition(plast, SIGNAL(finished()), psPreTest);
			plast = psPreTest;
		}
		else
		{
			sExperiment->setInitialState(psPreTest);
			plast = psPreTest;
		}
	}
	if (psHabituation)
	{
		if (plast)
		{
			plast->addTransition(plast, SIGNAL(finished()), psHabituation);
			plast = psHabituation;
		}
		else
		{
			sExperiment->setInitialState(psHabituation);
			plast = psHabituation;
		}
	}
	if (psTest)
	{
		if (plast)
		{
			plast->addTransition(plast, SIGNAL(finished()), psTest);
			plast = psTest;
		}
		else
		{
			sExperiment->setInitialState(psTest);
			plast = psTest;
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
	QMapIterator<unsigned int, const Habit::StimulusSettings *> it(pmm->pmap());
	while (it.hasNext())
	{
		it.next();
		log.append(new HStimulusSettingsEvent(*it.value(), it.key()));
	}

	return psm;
}
