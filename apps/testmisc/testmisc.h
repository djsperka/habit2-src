/*
 *  testmisc.h
 *  testmisc
 *
 *  Created by Oakes Lab on 10/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include <QtTest/QtTest>
#include "HEventLog.h"
#include "HLook.h"
#include "stimulussettings.h"

class TestHabutil: public QObject
{
	Q_OBJECT
public:
	TestHabutil();
signals:
//	void look(HLook);
//	void trialStarted(Habit::StimulusSettings& s);
//	void trialCompleted();
	private slots:
//	void initTestCase();
//	void testTrialLooks();
//	void testFixedN();
//	void testTotalLookingTime();
//	void testHabituationFirstFixed();
//	void testHabituationLongestFixed();
//	void testHabituationFirstSliding();
//	void testHabituationLongestSliding();
//	void testTrialLog();
	void testEventLog();
//	void testFixedNCriteria();
//	void testTotalLookingTimeCriteria();
	void testTrialGenerator();
	void testMonitorSettings();
	void testControlBarOptions();
	void testTrialsInfo();
	void testDesignSettings();
	void testCriterionSettings();
	void testHabituationSettings();
	void testStimulusDisplayInfo();
	void testStimulusInfo();
	void testStimulusSettings();
	void testAttentionGetterSettings();
	void testStimuliSettings();
private:
//	void createStimulusSettings(int i, Habit::StimulusSettings& settings);

}; 
