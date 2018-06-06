/*
 * HLoggerObject.cpp
 *
 *  Created on: Jun 6, 2018
 *      Author: dan
 */

#include <HLoggerObject.h>
#include <QMutex>
#include <QMutexLocker>
#include <iostream>

QMutex f_mutex;

HLoggerObject::HLoggerObject() {
	// TODO Auto-generated constructor stub

}

HLoggerObject::~HLoggerObject() {
	// TODO Auto-generated destructor stub
}

void HLoggerObject::logMessage(const QString& s)
{
	emit message(s);
}


void HLoggerObject::loggingHandler(QtMsgType type, const QMessageLogContext &, const QString& msg)
{
    char symbol;
	switch (type)
	{
	case QtDebugMsg:
		symbol = 'D';
		break;
	case QtInfoMsg:
		symbol = 'I';
		break;
	case QtCriticalMsg:
		symbol = 'C';
		break;
	case QtWarningMsg:
		symbol = 'W';
		break;
	case QtFatalMsg:
		symbol = 'F';
		break;
	}

    QString output = QString("[%1] %2").arg( symbol ).arg( msg );

    std::cout << msg.toStdString() << std::endl;

    instance()->logMessage(output);
}

