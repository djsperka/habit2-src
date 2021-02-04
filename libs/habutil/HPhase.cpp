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

HPhase::HPhase(HExperiment& exp, HPhaseCriteria* pcriteria, HEventLog& log, const Habit::HPhaseSettings& phaseSettings, const Habit::HLookSettings& lookSettings, const Habit::AttentionGetterSettings& agSettings, bool bTestingInput)
	: HExperimentChildState(exp, log, phaseSettings.getName())
	, m_pcriteria(pcriteria)
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

	connect(this, SIGNAL(phaseStarted(QString, int)), &exp, SIGNAL(phaseStarted(QString, int)));
	connect(this, SIGNAL(phaseEnded(QString, int)), &exp, SIGNAL(phaseEnded(QString, int)));
};


void HPhase::screenStarted(const QString& filenameOrColor, int playerid)
{
	eventLog().append(new HScreenStartEvent(filenameOrColor, playerid, HElapsedTimer::elapsed()));
}

void HPhase::agStarted(int id)
{
	eventLog().append(new HAGStartEvent(id, HElapsedTimer::elapsed()));
}

void HPhase::stimStarted(int stimid)
{
	eventLog().append(new HStimStartEvent(stimid, HElapsedTimer::elapsed()));
	qDebug() << "stimStarted( " << stimid << " )";
}

void HPhase::trialStarted(int context, unsigned int trial, unsigned int repeat)
{
	// If phase criteria uses total looking time, update the look detector so it knows when this
	// criteria is met

	if (m_pcriteria->habituationType() == HHabituationType::HHabituationTypeTotalLookingTime)
	{
		int t = m_phaseSettings.habituationSettings().getTotalLookLengthToEnd() - eventLog().getPhaseLog().totalLookingTime();
		experiment().getLookDetector().setPhaseAccumulatedLookTime(t);
		qDebug() << "HPhase::trialStarted - set phaseAccumulatedLookTime " << t;
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
	QObject::connect(&(this->experiment().getHGMM()), SIGNAL(screenStarted(const QString&, int)), this, SLOT(screenStarted(const QString&, int)));
	QObject::connect(&(this->experiment().getHGMM()), SIGNAL(agStarted(int)), this, SLOT(agStarted(int)));
	QObject::connect(&(this->experiment().getHGMM()), SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));

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
	emit phaseStarted(m_phaseSettings.getName(), m_phaseSettings.getSeqno());

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
	QObject::disconnect(&(this->experiment().getHGMM()), SIGNAL(screenStarted(const QString&, int)), this, SLOT(screenStarted(const QString&, int)));
	QObject::disconnect(&(this->experiment().getHGMM()), SIGNAL(agStarted(int)), this, SLOT(agStarted(int)));
	QObject::disconnect(&(this->experiment().getHGMM()), SIGNAL(stimStarted(int)), this, SLOT(stimStarted(int)));
	QObject::disconnect(this, SIGNAL(phaseStarted(QString, int)), &experiment(), SIGNAL(phaseStarted(QString, int)));
	QObject::disconnect(this, SIGNAL(phaseEnded(QString, int)), &experiment(), SIGNAL(phaseEnded(QString, int)));
	
	// cleanup any remaining stim
	experiment().requestCleanup(context());
};

void HPhase::requestAG()
{
	eventLog().append(new HAGRequestEvent(HElapsedTimer::elapsed()));
	this->experiment().getHGMM().ag();
}

void HPhase::onTrialCompleteEntered()
{
	bool isHabituated = false;
	int eventNumber = HEventType::HEventUndefined.number();

	// request cleanup of stimulus
	experiment().requestCleanup(context(), m_itrial);

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

		// Even when the phase is complete, make sure to preroll as needed.
		// This should start with the next phase/context.
		qDebug() << "HPhase::onTrialCompleteEntered(): call prerollNextPhase( " << context() << ")";
		experiment().prerollNextPhase(context());

	}
	else 
	{
		// update trial number and post "new trial" event
		m_sTrial->setTrialNumber(++m_itrial);
		machine()->postEvent(new HNewTrialEvent());

		// now request prerolling. This call uses the updated trial number
		qDebug() << "HPhase::onTrialCompleteEntered(): call prerollAsNeeded(" << context() << ", " << m_itrial << ")";
		experiment().prerollAsNeeded(context(), m_itrial);
	}
};

