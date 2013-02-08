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
#include "HEventLog.h"
#include "HLookDetector.h"
#include "stimulusSettings.h"
#include <QList>
#include <QEvent>
#include <QAbstractTransition>

class HPhase: public HLogState
{
	Q_OBJECT
	
	QList<QPair<int, Habit::StimulusSettings> > m_stimuli;
	QString m_name;
	int m_itrial;
	HTrial* m_sTrial;

public:
	HPhase(HEventLog& log, const QList<QPair<int, Habit::StimulusSettings> >& stimuli, QObject* pMediaManager, HLookDetector* pLD, char *name, int maxTrialLengthMS, int maxNoLookTimeMS, bool bFixedLength, bool bUseAG, HState* parent=0);
	virtual ~HPhase() {};
	bool advance();
	HTrial* getHTrial() { return m_sTrial; };
	inline int currentTrialNumber() { return m_itrial; };	
	inline int currentStimNumber() { return m_stimuli.at(m_itrial).first; };
	inline Habit::StimulusSettings currentStimulusSettings() { return m_stimuli.at(m_itrial).second; };
	
protected:
	virtual void onEntry(QEvent* e);
	virtual void onExit(QEvent* e);
	
public slots:
	void onTrialCompleteEntered();

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
