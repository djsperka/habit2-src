/*
 * TestPreroller.h
 *
 *  Created on: Oct 29, 2018
 *      Author: dan
 */

#ifndef APPS_TESTMISC_TESTPREROLLER_H_
#define APPS_TESTMISC_TESTPREROLLER_H_

#include <QObject>

class TestPreroller: public QObject
{

	Q_OBJECT

	private Q_SLOTS:
		void test1();
		void test2();
		void initTestCase();
		void cleanupTestCase();
		void init();
		void cleanup();

public:
	TestPreroller();
	virtual ~TestPreroller();
};

#endif /* APPS_TESTMISC_TESTPREROLLER_H_ */
