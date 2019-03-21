/*
 *  HPhase.h
 *  habutil
 *
 *  Created by Oakes Lab on 8/27/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HPHASE_H_
#define _HPHASE_H_

#include "HExperimentChildState.h"
#include "HLookDetector.h"
#include "HQEvents.h"
#include "HTypes.h"
#include "HPhaseCriteria.h"
#include "HPhaseSettings.h"
#include "HLookSettings.h"
#include "stimulussettings.h"
#include "attentiongettersettings.h"
#include <QList>
#include <QPair>
#include <QEvent>
#include <QAbstractTransition>


// HPhase
//
// 

class HTrial;
class HPhaseCriteria;


class HPhase: public HExperimentChildState
{
	Q_OBJECT
	
	HPhaseCriteria *m_pcriteria;
	const Habit::HPhaseSettings m_phaseSettings;
	const Habit::HLookSettings m_lookSettings;
	int m_itrial;		// this is the current trial. First trial is '0'. Stim found at m_stimuli[m_itrial].second
	HTrial* m_sTrial;

public:
	HPhase(HExperiment& exp, HPhaseCriteria* pcriteria, HEventLog& log, const Habit::HPhaseSettings& phaseSettings, const Habit::HLookSettings& lookSettings, const Habit::AttentionGetterSettings& agSettings, bool bTestingInput=false);
	virtual ~HPhase() {};
	bool advance();
	HTrial* getHTrial() { return m_sTrial; };
	inline int currentTrialNumber() { return m_itrial; };	
	//unsigned int currentStimNumber() { return m_stimuli.at(m_itrial).first; };
	
	// seqno in db is called context or phase context in much of the source
	int context() { return m_phaseSettings.getSeqno(); };

	//void requestCurrentStim();
	void requestAG();

	const QString& name() const { return m_phaseSettings.getName(); };

protected:
	virtual void onEntry(QEvent* e);
	virtual void onExit(QEvent* e);

signals:
	void phaseStarted(QString phaseName, int context);
	void phaseEnded(QString phaseName, int context);
	void trialStarted(int trialNumber, int repeatNumber);
	
public slots:
	void onTrialCompleteEntered();
	void screenStarted(const QString&, int);
	void agStarted(int);
	void stimStarted(int);
	//void checkPrerollStatus(int trialnumber=-1, int repeat=0);
};

class HAllTrialsDoneTransition: public QAbstractTransition
{
public:
	HAllTrialsDoneTransition() {};
	~HAllTrialsDoneTransition() {};
protected:
	virtual bool eventTest(QEvent *event)
	{
		return (event->type() == QEvent::Type(HAllTrialsDoneEvent::AllTrialsDoneType));
	};
	virtual void onTransition(QEvent* event)
	{
		Q_UNUSED(event);
	};	
};

class HNewTrialTransition: public QAbstractTransition
{
public:
	HNewTrialTransition() {};
	~HNewTrialTransition() {};
protected:
	virtual bool eventTest(QEvent *event)
	{
		return (event->type() == QEvent::Type(HNewTrialEvent::NewTrialType));
	};
	virtual void onTransition(QEvent* event)
	{
		Q_UNUSED(event);
	};
};


class HAbortTrialTransition: public QAbstractTransition
{
public:
	HAbortTrialTransition() {};
	~HAbortTrialTransition() {};
protected:
	virtual bool eventTest(QEvent *event)
	{
		return (event->type() == QEvent::Type(HAbortTrialQEvent::AbortTrialType));
	};
	virtual void onTransition(QEvent* event)
	{
		Q_UNUSED(event);
	};
};



#endif
