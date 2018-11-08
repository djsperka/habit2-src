/*
 * TestPreroller.cpp
 *
 *  Created on: Oct 29, 2018
 *      Author: dan
 */

#include "TestPreroller.h"
#include <QtDebug>

TestPreroller::TestPreroller() {
	// TODO Auto-generated constructor stub

}

TestPreroller::~TestPreroller() {
	// TODO Auto-generated destructor stub
}


void TestPreroller::test1()
{
	qDebug() << "test1 called";
}

void TestPreroller::test2()
{
	qDebug() << "test2 called";
}

void TestPreroller::initTestCase()
{
	qDebug() << "initTestCase()";
}

void TestPreroller::cleanupTestCase()
{
	qDebug() << "cleanupTestCase()";
}

void TestPreroller::init()
{
	qDebug() << "init called";
}

void TestPreroller::cleanup()
{
	qDebug() << "cleanup called";
}
