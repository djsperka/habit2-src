/*
 *  HPhaseCriteria.h
 *  habutil
 *
 *  Created by Oakes Lab on 10/9/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HPHASECRITERIA_H_
#define _HPHASECRITERIA_H_

#include "HLook.h"
#include "criterionsettings.h"
#include <QList>


// TrialLooks saves a list of looks for a given trial. 
// Has some convenience functions for computing things about the looks. 

class TrialLooks : public QList<HLook>
{
public:
	enum TrialStatus {
		TrialCompleted, TrialRunning
	};
	TrialLooks(): m_status(TrialRunning) {};
	~TrialLooks() {};
	bool isCompleted() const;
	int totalLookingTime() const;
	void completed() { m_status = TrialCompleted; };
private:
	TrialStatus m_status;
};


// Abstract class used for determining whether a phase is completed.
// Subclasses must implement isPhaseComplete(). 

class HPhaseCriteria: public QObject
{
	Q_OBJECT
public:
	HPhaseCriteria() : QObject() {};
	virtual ~HPhaseCriteria() {};
	virtual bool isPhaseComplete() = 0;
	int nCompleted();
	int totalLookingTime();
protected slots:
	void trialStarted();
	void trialCompleted();
	void trialAborted();
	void gotLook(HLook l);
protected:
	QList<TrialLooks> m_trials;
};


class HPhaseFixedNCriteria: public HPhaseCriteria
{
	Q_OBJECT
public:
	HPhaseFixedNCriteria(int N): HPhaseCriteria(), m_N(N) {};
	virtual ~HPhaseFixedNCriteria() {};
	bool isPhaseComplete();
private:
	int m_N;
};

class HPhaseTotalLookingTimeCriteria: public HPhaseCriteria
{
	Q_OBJECT
public:
	HPhaseTotalLookingTimeCriteria(int msTotal) : HPhaseCriteria(), m_msTotal(msTotal) {};
	virtual ~HPhaseTotalLookingTimeCriteria() {};
	bool isPhaseComplete();
private:
	int m_msTotal;
};

class HPhaseHabituationCriteria: public HPhaseCriteria
{
	Q_OBJECT
public:
	HPhaseHabituationCriteria(Habit::CriterionSettings c) : HPhaseCriteria(), m_c(c) {};
	virtual ~HPhaseHabituationCriteria() {};
	bool isPhaseComplete();
	bool getBasisSum(int& iBasisSum, int& iBasisWindowStart);
	
	// Get the total looking time for the trials 'window' starting at the trial with index 'ifirst'. 
	// If there are not enough trials completed to make a 'window' then return 0.
	
	bool getWindowSum(int& iWindowSum, int ifirst);
private:
	Habit::CriterionSettings m_c;
	
};	


#endif