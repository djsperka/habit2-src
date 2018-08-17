/*
 *  HPhase.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 8/27/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HPhase.h"
#include "HTrial.h"
#include "HElapsedTimer.h"
#include "HGMM.h"
#include <QFinalState>
#include <QtDebug>

HPhase::HPhase(HExperiment& exp, HPhaseCriteria* pcriteria, HEventLog& log, const QList< QPair<int, QString> >& stimuli, const Habit::HPhaseSettings& phaseSettings, const Habit::HLookSettings& lookSettings, const Habit::AttentionGetterSettings& agSettings, bool bTestingInput)
	: HExperimentChildState(exp, log, phaseSettings.getName())
	, m_pcriteria(pcriteria)
	, m_stimuli(stimuli)
	, m_phaseSettings(phaseSettings)
	, m_lookSettings(lookSettings)
	, m_itrial(0)
{
	QAbstractTransition* trans;
	m_sTrial = new HTrial(*this, log, phaseSettings, lookSettings, agSettings, bTestingInput);
	setInitialState(m_sTrial);
	HPhaseTrialCompleteState* sTrialComplete = new HPhaseTrialCompleteState(*this, log);
	m_sTrial->addTransition(m_sTrial, SIGNAL(finished()), sTrialComplete);
	QFinalState* sFinal = new QFinalState(this);
	
	// There are two event transitions from the TrialComplete state. 
	trans = new HAllTrialsDoneTransition();
	trans->setTargetState(sFinal);
	sTrialComplete->addTransition(trans);
	trans = new HNewTrialTransition();
	trans->setTargetState(m_sTrial);
	sTrialComplete->addTransition(trans);
	
	// signal when trial complete entered must be caught
	connect(sTrialComplete, SIGNAL(entered()), this, SLOT(onTrialCompleteEntered()));

	// Set object name - trick to allow updating status labels when this phase is entered.
	setObjectName(QString(phaseSettings.getName()));

	connect(this, SIGNAL(phaseStarted(QString)), &exp, SIGNAL(phaseStarted(QString)));
	connect(m_sTrial, SIGNAL(trialStarted(int, int)), &exp, SIGNAL(trialStarted(int, int)));


	checkPrerollStatus();
};


void HPhase::screenStarted(int playerid, const QString& filename)
{
	eventLog().append(new HScreenStartEvent(filename, playerid, HElapsedTimer::elapsed()));
}

void HPhase::agStarted(int id)
{
	eventLog().append(new HAGStartEvent(id, HElapsedTimer::elapsed()));
}

void HPhase::stimStarted(int stimid)
{
	eventLog().append(new HStimStartEvent(stimid, HElapsedTimer::elapsed()));
}

void HPhase::checkPrerollStatus(int trialnumber, int repeat)
{
	// If trialnumber is negative, then preroll the first stim
	if (trialnumber < 0)
	{
		if (m_stimuli.size() > 0)
		{
			qDebug() << "HPhase::checkPrerollStatus() - preroll first stimulus";
			experiment().getMediaManager().preroll(m_stimuli[0].first);
		}
	}
	else
	{
		int prerollID = m_itrial+1;
		if (prerollID < m_stimuli.size())
		{
			qDebug() << "HPhase::checkPrerollStatus() - trial/repeat " << trialnumber << "/" << repeat << " : m_itrial " << m_itrial << " current stim " << m_stimuli[m_itrial].first << " will preroll(" << m_stimuli[prerollID].first << ")";
			experiment().getMediaManager().preroll(m_stimuli[prerollID].first);
		}
		else
		{
			qDebug() << "HPhase::checkPrerollStatus() - trial/repeat " << trialnumber << "/" << repeat << " : m_itrial " << m_itrial << " current stim " << m_stimuli[m_itrial].first << "At last stim in phase; nothing to preroll.";
		}
	}

	// set phase accumulated look time if needed
	if (m_pcriteria->habituationType() == HHabituationType::HHabituationTypeTotalLookingTime)
	{
		int t = m_phaseSettings.habituationSettings().getTotalLookLengthToEnd() - eventLog().getPhaseLog().totalLookingTime();
		experiment().getLookDetector().setPhaseAccumulatedLookTime(t);
		qDebug() << "HPhase::checkPrerollStatus - set phaseAccumulatedLookTime " << t;
	}

}

void HPhase::onEntry(QEvent* e)
{
	Q_UNUSED(e);
	
	// Post to log
	HState::onEntry(e);

	// Set trial number
	m_itrial = 0;
	m_sTrial->setTrialNumber(m_itrial);

	// post 'phase start' event to event log.
	eventLog().append(new HPhaseStartEvent(m_phaseSettings.getName(), m_phaseSettings.getSeqno(), HElapsedTimer::elapsed()));
	

	// connect media manager signal screen(int) to slot screenStarted(int)
	// connect media manager signal agStarted(int) to slot agStarted(int)
	// connect media manager signal stimStarted(int) to slot stimStarted(int)
	// disconnect onExit() from this state.
	QObject::connect(&experiment().getMediaManager(), SIGNAL(screen(int, const QString&)), this, SLOT(screenStarted(int, const QString&)));
	QObject::connect(&experiment().getMediaManager(), SIGNAL(agStarted(int)), this, SLOT(agStarted(int)));
	QObject::connect(&experiment().getMediaManager(), SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));

	// update look detector settings  to per-phase values
	experiment().getLookDetector().setIsInclusiveLookTime(m_lookSettings.getInclusiveLookTime());
	experiment().getLookDetector().setMinLookTime(m_lookSettings.getMinLookTime());
	experiment().getLookDetector().setMinLookAwayTime(m_lookSettings.getMinLookAwayTime());
	experiment().getLookDetector().setMaxAccumulatedLookTime(m_phaseSettings.getIsMaxAccumulatedLookTime() ? m_phaseSettings.getMaxAccumulatedLookTime() : 0);
	experiment().getLookDetector().setMaxLookAwayTime(m_phaseSettings.getIsMaxLookAwayTime() ? m_phaseSettings.getMaxLookAwayTime() : 0);

	// emit signal to let the world know this phase has started
	// djs 10-7-2015 Moved this to be the last statement in the function.
	// NOTE: onEntry() is called first, then QAbstractState emits SIGNAL(entered()),
	// so this signal comes before the state is "entered"?
	emit phaseStarted(m_phaseSettings.getName());

};

void HPhase::onExit(QEvent* e)
{
	Q_UNUSED(e);

	// Post to log
	HState::onExit(e);

	// post 'phase end' event to event log. 
	eventLog().append(new HPhaseEndEvent(HElapsedTimer::elapsed()));

	// connect media manager signal screen(int) to slot screenStarted(int)
	// disconnect onExit() from this state.
	QObject::disconnect(&experiment().getMediaManager(), SIGNAL(screen(int, const QString&)), this, SLOT(screenStarted(int, const QString&)));
	QObject::disconnect(&experiment().getMediaManager(), SIGNAL(agStarted(int)), this, SLOT(agStarted(int)));
	QObject::disconnect(&experiment().getMediaManager(), SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	
};


void HPhase::requestCurrentStim()
{
	if (m_itrial < m_stimuli.size())
	{
		qDebug() << "HPhase::requestCurrentStim(): stim " << m_stimuli.at(m_itrial).first << " label " << m_stimuli.at(m_itrial).second;
		eventLog().append(new HStimLabelRequestEvent(m_stimuli.at(m_itrial).first, m_stimuli.at(m_itrial).second, HElapsedTimer::elapsed()));
		experiment().getMediaManager().stim(m_stimuli.at(m_itrial).first);
	}
	else 
	{
		eventLog().append(new HStimLabelRequestEvent(-1, QString("ERROR"), HElapsedTimer::elapsed()));
		experiment().getMediaManager().defaultStim();
		qDebug() << "Bad trial index (" << m_itrial << ")for phase " << m_name << " max " << m_stimuli.size();
	}

}

void HPhase::requestAG()
{
	eventLog().append(new HAGRequestEvent(HElapsedTimer::elapsed()));
	experiment().getMediaManager().ag();
}

void HPhase::onTrialCompleteEntered()
{
	bool isHabituated = false;
	int eventNumber = HEventType::HEventUndefined.number();
	if (m_pcriteria->isPhaseComplete(eventLog().getPhaseLog(), isHabituated, eventNumber))
	{
		machine()->postEvent(new HAllTrialsDoneEvent());

		// check event number
		if (eventNumber == HEventType::HHabituationSuccess.number())
		{
			eventLog().append(new HHabituationSuccessEvent(HElapsedTimer::elapsed()));
		}
		else if (eventNumber == HEventType::HHabituationFailure.number())
		{
			eventLog().append(new HHabituationFailureEvent(HElapsedTimer::elapsed()));
		}
		else if (eventNumber != HEventType::HEventUndefined.number())
		{
			qWarning() << "Unhandled event type from HPhaseCriteria::isPhaseComplete";
		}
	}
	else 
	{
		m_sTrial->setTrialNumber(++m_itrial);
		machine()->postEvent(new HNewTrialEvent());
	}
};

