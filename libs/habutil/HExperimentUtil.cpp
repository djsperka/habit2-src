/*
 * HExperimentUtil.cpp
 *
 *  Created on: May 15, 2014
 *      Author: Oakeslab
 */

#include "HExperimentUtil.h"
#include "HLookDetectorUtil.h"


HExperiment* createExperiment(const Habit::RunSettings& runSettings, const Habit::ExperimentSettings& experimentSettings, HEventLog& log)
{
	HPhase* psPreTest = (HPhase *)NULL;
	HPhase* psHabituation = (HPhase *)NULL;
	HPhase* psTest = (HPhase *)NULL;
	HPhaseCriteria* pcritPreTest = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritHabituation = (HPhaseCriteria *)NULL;
	HPhaseCriteria* pcritTest = (HPhaseCriteria *)NULL;
	HLookDetector* pld = (HLookDetector *)NULL;
	HMediaManager* pmm = (HMediaManager *)NULL;

	// Create look detector. Type of look detector is buried in the experiment settings; what we receive is a generic
	// HLookDetector* .
	pld = createLookDetector(experimentSettings, log, this);

	// connect attention() and look() signals to a slot so we can forward the info to the event log
	connect(pld, SIGNAL(attention()), this, SLOT(onAttention()));
	connect(pld, SIGNAL(look(HLook)), this, SLOT(onLook(HLook)));




	// Create media manager.
	// The stimuli configured for each phase are pulled using the experiment settings.
	// One by one the stimuli are added to the media manager's player. As each stim is added,
	// its ordinal position in the player's set of stim is paired with the StimulusSettings
	// object, and the pair is stored in the StimulusSettingsList objects l1, l2, l3.
	m_pmm = createMediaManager(m_experimentSettings, NULL);

	// Need to know if AG is used. If it is, add attention getter settings to media manager
	Habit::AttentionGetterSettings ags(m_experimentSettings.getAttentionGetterSettings());
	if (ags.isAttentionGetterUsed()) m_pmm->addAG(ags.getAttentionGetterStimulus());

	// add stimuli for each phase.
	if (m_experimentSettings.getPreTestPhaseSettings().getIsEnabled())
		m_pmm->addStimuli(m_experimentSettings.getPreTestStimuliSettings());
	if (m_experimentSettings.getHabituationPhaseSettings().getIsEnabled())
		m_pmm->addStimuli(m_experimentSettings.getHabituationStimuliSettings());
	if (m_experimentSettings.getTestPhaseSettings().getIsEnabled())
		m_pmm->addStimuli(m_experimentSettings.getTestStimuliSettings());

	// Connect media manager signals to slots here so we can update display labels.
	connect(m_pmm, SIGNAL(agStarted(int)), this, SLOT(onAGStarted()));
	connect(m_pmm, SIGNAL(stimStarted(int)), this, SLOT(onStimStarted(int)));
	connect(m_pmm, SIGNAL(cleared()), this, SLOT(onCleared()));


	// Construct state machine.
	m_psm = new QStateMachine();

	// connect the state machine's finished() signal to this dialog's close() slot
	connect(m_psm, SIGNAL(finished()), this, SLOT(onExperimentFinished()));
	connect(m_psm, SIGNAL(started()), this, SLOT(onExperimentStarted()));

	// This is a single super-state that holds all the phases.
	HExperiment* sExperiment = new HExperiment(log, *m_pmm, *pld);
	m_psm->addState(sExperiment);
	m_psm->setInitialState(sExperiment);
	QFinalState* sFinal = new QFinalState;
	m_psm->addState(sFinal);
	sExperiment->addTransition(sExperiment, SIGNAL(finished()), sFinal);
 	QObject::connect(sExperiment, SIGNAL(playStim(int)), m_pmm, SLOT(stim(int)));// media player will receive this signal and emit stimStarted()

	// transition from experiment on the "Stop Trials" button being clicked()
	sExperiment->addTransition(m_pbStopTrials, SIGNAL(clicked()), sFinal);

	// Create phases.
	// Each HPhase gets a list of stimuli, as pairs of <int, StimulusSettings>, and these are used in order as the
	// stimuli for that phase. The <int> part is passed to the media manager as the key to play that stimulus.
	//
	if (m_experimentSettings.getPreTestPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = m_experimentSettings.getPreTestPhaseSettings();
		QList<int> list;
		QList<unsigned int> stimidListInitial;
		QList<unsigned int> stimidListOrdered;

		m_pmm->getContextStimList(HStimContext::PreTestPhase, stimidListInitial);

		if (!m_runSettings.getPretestOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse Pretest order (" << m_runSettings.getPretestOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), m_runSettings.isPretestRandomized(), m_runSettings.getPretestRandomizeMethod()==1);
		for (unsigned int i=0; i<ps.getNTrials(); i++)
		{
			stimidListOrdered.append(stimidListInitial.at(list.at(htg.next()) - 1));
		}
		pcritPreTest = new HPhaseFixedNCriteria(ps.getNTrials());
		psPreTest = new HPhase(*sExperiment, pcritPreTest, log, stimidListOrdered, ps, m_experimentSettings.getHLookSettings(), ags.isAttentionGetterUsed());

	}

	if (m_experimentSettings.getHabituationPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = m_experimentSettings.getHabituationPhaseSettings();
		QList<int> list;
		QList<unsigned int> stimidListInitial;
		QList<unsigned int> stimidListOrdered;

		m_pmm->getContextStimList(HStimContext::HabituationPhase, stimidListInitial);

		if (!m_runSettings.getHabituationOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse habituation order (" << m_runSettings.getHabituationOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), m_runSettings.isHabituationRandomized(), m_runSettings.getHabituationRandomizeMethod()==1);
		for (unsigned int i=0; i<ps.getNTrials(); i++)
		{
			stimidListOrdered.append(stimidListInitial.at(list.at(htg.next()) - 1));
		}

		// Create habituation criteria object.
		pcritHabituation = createPhaseCriteria(m_experimentSettings.getHabituationSettings(), ps.getNTrials());
		psHabituation = new HPhase(*sExperiment, pcritHabituation, log, stimidListOrdered, ps, m_experimentSettings.getHLookSettings(), ags.isAttentionGetterUsed());
	}

	if (m_experimentSettings.getTestPhaseSettings().getIsEnabled())
	{
		const Habit::HPhaseSettings& ps = m_experimentSettings.getTestPhaseSettings();
		QList<int> list;
		QList<unsigned int> stimidListInitial;
		QList<unsigned int> stimidListOrdered;

		m_pmm->getContextStimList(HStimContext::TestPhase, stimidListInitial);

		if (!m_runSettings.getTestOrderList(list))
		{
			QDebug(QtFatalMsg) << "Cannot parse test order (" << m_runSettings.getTestOrderList(list) << ").";
		}

		HTrialGenerator htg(list.size(), m_runSettings.isTestRandomized(), m_runSettings.getTestRandomizeMethod()==1);
		for (unsigned int i=0; i<ps.getNTrials(); i++)
		{
			stimidListOrdered.append(stimidListInitial.at(list.at(htg.next()) - 1));
		}

		pcritTest = new HPhaseFixedNCriteria(ps.getNTrials());
		psTest = new HPhase(*sExperiment, pcritTest, log, stimidListOrdered, ps, m_experimentSettings.getHLookSettings(), ags.isAttentionGetterUsed());
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
	QMapIterator<unsigned int, Habit::StimulusSettings> it(m_pmm->map());
	while (it.hasNext())
	{
		it.next();
		log.append(new HStimulusSettingsEvent(it.value(), it.key()));
	}

	// Finally, text properties of labels can be assigned on entry to various states.
	sExperiment->assignProperty(m_labelExperimentStatusValue, "text", "Running");
	if (psPreTest)
		psPreTest->assignProperty(m_labelCurrentPhaseValue, "text", "PreTest");
	if (psHabituation)
		psHabituation->assignProperty(m_labelCurrentPhaseValue, "text", "Habituation");
	if (psTest)
		psTest->assignProperty(m_labelCurrentPhaseValue, "text", "Test");

}



