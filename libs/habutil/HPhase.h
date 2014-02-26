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
#include "stimulusSettings.h"
#include <QList>
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
	QList<QPair<int, Habit::StimulusSettings> > m_stimuli;
	const HPhaseType& m_ptype;
	int m_itrial;		// this is the current trial. First trial is '0'. Stim found at m_stimuli[m_itrial].second
	HTrial* m_sTrial;

public:
	HPhase(HExperiment& exp, HPhaseCriteria* pcriteria, HEventLog& log, const QList<QPair<int, Habit::StimulusSettings> >& stimuli, const HPhaseType& ptype, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG);
	virtual ~HPhase() {};
	bool advance();
	HTrial* getHTrial() { return m_sTrial; };
	inline int currentTrialNumber() { return m_itrial; };	
	inline int currentStimNumber() { return m_stimuli.at(m_itrial).first; };
	inline Habit::StimulusSettings currentStimulusSettings() { return m_stimuli.at(m_itrial).second; };
	
	void requestCurrentStim();
	void requestAG();
	const HPhaseType& ptype() const { return m_ptype; };

protected:
	virtual void onEntry(QEvent* e);
	virtual void onExit(QEvent* e);
	
public slots:
	void onTrialCompleteEntered();
	void screenStarted(int, const QString&);
	void agStarted(int);
	void stimStarted(int);
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
		return (event->type() == QEvent::Type(HAbortTrialEvent::AbortTrialType));
	};
	virtual void onTransition(QEvent* event)
	{
		Q_UNUSED(event);
	};
};



#endif
