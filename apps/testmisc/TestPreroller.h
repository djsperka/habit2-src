/*
 * TestPreroller.h
 *
 *  Created on: Oct 29, 2018
 *      Author: dan
 */

#ifndef APPS_TESTMISC_TESTPREROLLER_H_
#define APPS_TESTMISC_TESTPREROLLER_H_

#include <QObject>
#include <QMap>
#include <QList>
#include <QtTest/QtTest>

#include "Preroller.h"

class TestPreroller: public QObject
{

	Q_OBJECT

	Preroller preroller;
	QMap<QString, QList<unsigned int> > m_mapStimidLists;

	// map of stimid lists for each phase, by seqno

	bool spyContainsStimid(const QSignalSpy& spy, unsigned int stimid);

	private Q_SLOTS:

		// call getNextTrials with no trial arg, phase is not hab phase
		void test1();

		// call getNextTrials with no trial arg, phase is a hab phase
		void test2();

		// call getNextTrials with trial arg 0, phase is not hab phase
		void test3();
		// call getNextTrials with trial arg 1, phase is not hab phase
		void test4();
		// call getNextTrials with trial arg 2, phase is not hab phase, following phase is hab
		void test5();

		// first call to preroller
		void test6();

		void initTestCase();
		void cleanupTestCase();
		void init();
		void cleanup();

public:
	TestPreroller();
	virtual ~TestPreroller();
};

#endif /* APPS_TESTMISC_TESTPREROLLER_H_ */
