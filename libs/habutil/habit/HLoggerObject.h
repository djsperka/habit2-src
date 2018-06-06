/*
 * HLoggerObject.h
 *
 *  Created on: Jun 6, 2018
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HABIT_HLOGGEROBJECT_H_
#define LIBS_HABUTIL_HABIT_HLOGGEROBJECT_H_

#include <QtCore/qobject.h>
#include <QtGlobal>

class HLoggerObject: public QObject
{
	Q_OBJECT

	HLoggerObject();

	signals:
		void message(const QString& s);

public:
	virtual ~HLoggerObject();

	static HLoggerObject* instance()
	{
		static HLoggerObject _instance;
		return &_instance;
	}

	void logMessage(const QString& s);
	static void loggingHandler(QtMsgType type, const QMessageLogContext &, const QString& msg);
};

#endif /* LIBS_HABUTIL_HABIT_HLOGGEROBJECT_H_ */
