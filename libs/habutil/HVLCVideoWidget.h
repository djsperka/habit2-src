/*
 * HVLCPlayerWidget.h
 *
 *  Created on: Mar 16, 2017
 *      Author: dan
 */

#ifndef LIBS_HABUTIL_HVLCVIDEOWIDGET_H_
#define LIBS_HABUTIL_HVLCVIDEOWIDGET_H_

#include "HVLCMediaPlayer.h"
#include <VLCQtWidgets/WidgetVideo.h>
#include <QColor>
#include <QSize>

class HVLCVideoWidget: public QFrame
{
	Q_OBJECT

private:
	VlcWidgetVideo *m_pVlcWidgetVideo;
	void init(HVLCMediaPlayer *player, QWidget *parent=NULL);

public:
	// for a widget NOT full screen
	HVLCVideoWidget(HVLCMediaPlayer *player, QWidget *parent=NULL, const QColor& bkgd = QColor(Qt::gray), const QSize& minSize = QSize(320, 240));

	// for full screen. The 'screen' must be a valid screen index. Invalid index will be mapped
	// primary screen (same as default).
	HVLCVideoWidget(HVLCMediaPlayer *player, int screen = -1, const QColor& bkgd = QColor(Qt::gray));
	virtual ~HVLCVideoWidget();

private slots:
	void sizeChanged(unsigned int, unsigned int);

};

#endif /* LIBS_HABUTIL_HVLCVIDEOWIDGET_H_ */
