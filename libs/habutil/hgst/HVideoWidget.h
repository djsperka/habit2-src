/*
 * HVideoWidget.h
 *
 *  Created on: Aug 23, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HGST_HVIDEOWIDGET_H_
#define LIBS_HABUTIL_HGST_HVIDEOWIDGET_H_

#include <QGst/Ui/VideoWidget>
#include "HTypes.h"


class HVideoWidget: public QGst::Ui::VideoWidget
{
	Q_OBJECT

	QSize m_sizeStimulus;
	const HDisplayType& m_displayType;
	bool m_bMaintainAspectRatio;

public:
	HVideoWidget(const HDisplayType& dType, bool bMaintainAspectRatio, QWidget *parent = NULL);
	virtual ~HVideoWidget();

	void setStimulusSize(float width, float height);

	//QSize sizeHint() const;
	QSize getSize() const;

	public Q_SLOTS:
		void stimulusChanged();
};

#endif /* LIBS_HABUTIL_HGST_HVIDEOWIDGET_H_ */
