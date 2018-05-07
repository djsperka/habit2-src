/*
 * HVideoWidget.h
 *
 *  Created on: Aug 23, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HVIDEOWIDGET_H_
#define LIBS_HABUTIL_HGST_HVIDEOWIDGET_H_

#include <QWidget>
#include "HTypes.h"


class HVideoWidget: public QWidget
{
	Q_OBJECT

	QSize m_sizeStimulus;

public:
	HVideoWidget(QWidget *parent = NULL);
	virtual ~HVideoWidget();

	void setStimulusSize(const QSize& size) { m_sizeStimulus = size; };
	const QSize& getStimulusSize() const { return m_sizeStimulus; };


	public Q_SLOTS:
		void stimulusChanged();
};

#endif /* LIBS_HABUTIL_HGST_HVIDEOWIDGET_H_ */
