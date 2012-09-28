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

#include "HTrial.h"
#include <QList>

class HPhase: public HState
{
	Q_OBJECT
	
	QList<int> m_stimuli;
	int m_itrial;
	HTrial* m_sTrial;
public:
	HPhase(const QList<int>& stimuli, QObject* pMediaManager, HLookDetector* pLD, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG, HState* parent=0);
	virtual ~HPhase() {};
	bool advance();
	inline int currentTrialNumber() { return m_itrial; };	
	inline int currentStimNumber() { return m_stimuli.at(m_itrial); };
};


struct HAllTrialsDoneEvent : public QEvent
{
	HAllTrialsDoneEvent() : QEvent(Type(AllTrialsDoneType)) {};
	~HAllTrialsDoneEvent() {};
	enum { AllTrialsDoneType = QEvent::User + 1 };
};

struct HNewTrialEvent : public QEvent
{
	HNewTrialEvent() : QEvent(Type(NewTrialType)) {};
	~HNewTrialEvent() {};
	enum { NewTrialType = QEvent::User + 2 };
};

struct HAbortTrialEvent : public QEvent
{
	HAbortTrialEvent() : QEvent(Type(AbortTrialType)) {};
	~HAbortTrialEvent() {};
	enum { AbortTrialType = QEvent::User + 3 };
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



class HPhaseTrialCompleteState: public HState
{
	Q_OBJECT

private:
	HPhase* m_sPhase;
	
public:
	
	HPhaseTrialCompleteState(HPhase* phase) : HState("HPhaseTrialCompleteState", phase), m_sPhase(phase) {};
	~HPhaseTrialCompleteState() {};
	
signals:
	
	
protected:
	// on entry to this state we request that the stim be played. 
	// Update: Make request by sending a signal (emit) to media controller. 
	
	void onEntry(QEvent* e) 
	{
		Q_UNUSED(e);
		HState::onEntry(e);
		
		if (m_sPhase->advance())
		{
			qDebug() << "HPhaseTrialCompleteState::onEntry - advance OK, post NewTrialEvent";
			machine()->postEvent(new HNewTrialEvent());
		}
		else 
		{
			qDebug() << "HPhaseTrialCompleteState::onEntry - all trials done, post AllTrialsDoneEvent";
			machine()->postEvent(new HAllTrialsDoneEvent());
		}
	};
};



#endif
