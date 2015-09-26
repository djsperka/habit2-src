/*
 * HExperimentUtil.cpp
 *
 *  Created on: May 15, 2014
 *      Author: Oakeslab
 */

#include "HExperimentUtil.h"
#include "HLookDetectorUtil.h"
#include "HExperimentUtil.h"
#include "HPhase.h"
#include "HTrialGenerator.h"
#include "HPhaseCriteriaUtil.h"
#include <QStateMachine>
#include <QFinalState>


using namespace Habit;

QStateMachine* createExperiment(QWidget *w, const Habit::RunSettings& runSettings, const Habit::ExperimentSettings& experimentSettings, HEventLog& log, HLookDetector* pld, HMediaManager* pmm)
{
	HPhase* psPreTest = (HPhase *)NULL;
	HPhase* psHabituation = (HPhase *)NULL;
	HPhase* psTest = (HPhase *)NULL;
	HPhaseCriteria* pcritPreTest = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritHabituation = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritTest = (HPhaseCriteria *)NULL;
	QStateMachine *psm;

	// TODO: THE LOOK DETECTOR IS PASSED AS AN ARG.
	// Create look detector. Type of look detector is buried in the experiment settings; what we receive is a generic
	// HLookDetector* .
	//pld = createLookDetector(experimentSettings, log, w);

	// TODO: this connection must be made externally
	//
	// connect attention() and look() signals to a slot so we can forward the info to the event log
	//connect(pld, SIGNAL(attention()), this, SLOT(onAttention()));
	//connect(pld, SIGNAL(look(HLook)), this, SLOT(onLook(HLook)));

	// TODO: THE MEDIA MANAGER IS CREATED EXTERNALLY
	// Create media manager.
	// The stimuli configured for each phase are pulled using the experiment settings.
	// One by one the stimuli are added to the media manager's player. As each stim is added,
	// its ordinal position in the player's set of stim is paired with the StimulusSettings
	// object, and the pair is stored in the StimulusSettingsList objects l1, l2, l3.
	//m_pmm = createMediaManager(m_experimentSettings, this);

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

	// Construct state machine.
	psm = new QStateMachine();

	// TODO THESE CONNECTIONS MADE EXTERALLY
	// connect the state machine's finished() signal to this dialog's close() slot
	//connect(m_psm, SIGNAL(finished()), this, SLOT(onExperimentFinished()));
	//connect(m_psm, SIGNAL(started()), this, SLOT(onExperimentStarted()));

	// This is a single super-state that holds all the phases.
	HExperiment* sExperiment = new HExperiment(log, *pmm, *pld);
	psm->addState(sExperiment);
	psm->setInitialState(sExperiment);
	QFinalState* sFinal = new QFinalState;
	psm->addState(sFinal);
	sExperiment->addTransition(sExperiment, SIGNAL(finished()), sFinal);
	//QObject::connect(sExperiment, SIGNAL(playStim(int)), m_pmm, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()

	//TEMPsExperiment->addTransition(m_pbStopTrials, SIGNAL(clicked()), sFinal);

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

	//TODO These connections made externally
	// Set some slots to update text labels in the control panel
	//connect(sExperiment, SIGNAL(phaseStarted(QString)), this, SLOT(onPhaseStarted(QString)));
	//connect(sExperiment, SIGNAL(trialStarted(int, int)), this, SLOT(onTrialStarted(int, int)));

}
