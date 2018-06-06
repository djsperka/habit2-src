/*
 * HLoggerWidget.cpp
 *
 *  Created on: Jun 6, 2018
 *      Author: dan
 */

#include <HLoggerWidget.h>
#include <HLoggerObject.h>

HLoggerWidget::HLoggerWidget(QWidget *parent)
: QPlainTextEdit(parent)
{
	// connect self to logger object signal
	connect(HLoggerObject::instance(), SIGNAL(message(const QString&)), this, SLOT(appendMessage(const QString&)), Qt::QueuedConnection);
}

HLoggerWidget::~HLoggerWidget()
{
	disconnect(HLoggerObject::instance(), SIGNAL(message(const QString&)), this, SLOT(appendMessage(const QString&)));
}

void HLoggerWidget::appendMessage(const QString& s)
{
	appendPlainText(s);
}
