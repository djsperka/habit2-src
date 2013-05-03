/*
 *  testmisc.h
 *  testmisc
 *
 *  Created by Oakes Lab on 10/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include <QtTest/QtTest>
#include <QObject>
#include "HEventLog.h"
#include "HLook.h"
#include "stimulussettings.h"

class TestHabutil: public QObject
{
	Q_OBJECT

public:
	TestHabutil();
	Habit::StimulusSettings getSS();

private slots:
//	void initTestCase();
	void testEventLog();
	void testEventLogRW();
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
	void testExperimentSettings();
	void testRunSettings();
	void testSubjectSettings();
}; 
