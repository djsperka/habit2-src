/*
 * HVideoWidget.cpp
 *
 *  Created on: Aug 23, 2017
 *      Author: dan
 */

#include <hgst/HVideoWidget.h>
#include <QtDebug>


HVideoWidget::HVideoWidget(QWidget *parent)
: QWidget(parent)
{
}

HVideoWidget::~HVideoWidget()
{
}

void HVideoWidget::stimulusChanged()
{
	qDebug() << "stimulus changed, size " << m_sizeStimulus;
	return;
}
