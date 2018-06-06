/*
 * HLoggerWidget.h
 *
 *  Created on: Jun 6, 2018
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HABIT_HLOGGERWIDGET_H_
#define LIBS_HABUTIL_HABIT_HLOGGERWIDGET_H_

#include <QtWidgets/qplaintextedit.h>

class HLoggerWidget: public QPlainTextEdit {

	Q_OBJECT

public:
	HLoggerWidget(QWidget *parent = NULL);
	virtual ~HLoggerWidget();

public slots:
	void appendMessage(const QString&);
};

#endif /* LIBS_HABUTIL_HABIT_HLOGGERWIDGET_H_ */
