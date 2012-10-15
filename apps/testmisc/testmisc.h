/*
 *  testmisc.h
 *  testmisc
 *
 *  Created by Oakes Lab on 10/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include <QtTest/QtTest>
#include "HPhaseCriteria.h"


class TestHabutil: public QObject
{
	Q_OBJECT
public:
	TestHabutil();
signals:
	void look(HLook);
	void trialStarted();
	void trialCompleted();
private slots:
	void initTestCase();
	void testTrialLooks();
	void testFixedN();
	void testTotalLookingTime();
	void testHabituationFirstFixed();
	void testHabituationLongestFixed();
	void testHabituationFirstSliding();
	void testHabituationLongestSliding();
	void testTrialGenerator();
private:
	TrialLooks m_looksNotCompleted;
	TrialLooks m_looksCompleted;
}; 
