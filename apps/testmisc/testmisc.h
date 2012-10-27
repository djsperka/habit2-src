/*
 *  testmisc.h
 *  testmisc
 *
 *  Created by Oakes Lab on 10/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include <QtTest/QtTest>
#include "HLook.h"
#include "stimulussettings.h"

class TestHabutil: public QObject
{
	Q_OBJECT
public:
	TestHabutil();
signals:
	void look(HLook);
	void trialStarted(Habit::StimulusSettings& s);
	void trialCompleted();
	private slots:
//	void initTestCase();
//	void testTrialLooks();
//	void testFixedN();
//	void testTotalLookingTime();
//	void testHabituationFirstFixed();
//	void testHabituationLongestFixed();
//	void testHabituationFirstSliding();
//	void testHabituationLongestSliding();
	void testTrialLog();
	void testPhaseLog();
	void testFixedNCriteria();
	void testTotalLookingTimeCriteria();
	void testTrialGenerator();
private:
	void createStimulusSettings(int i, Habit::StimulusSettings& settings);
}; 
