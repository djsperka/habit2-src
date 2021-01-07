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

	// size of the stimulus. This is the actual size of the stimulus, when the stimulus is a
	// file (image or video). For those types of stimuli 'decodebin' is used, and the size
	// of the stim is fetched from the caps when the video pad is added.
	// For stimuli created with a description (e.g. color bkgd), make sure to set the
	// size appropriately (widget size or screen size).
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
