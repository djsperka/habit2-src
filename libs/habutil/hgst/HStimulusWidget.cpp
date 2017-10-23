/*
 * HVideoWidget.cpp
 *
 *  Created on: Aug 17, 2017
 *      Author: dan
 */


#include <hgst/HStimulusWidget.h>
#include <hgst/HStimulusLayout.h>



HStimulusWidget::HStimulusWidget(const Habit::StimulusDisplayInfo& sdi, float screenWidth, float screenHeight, QWidget *parent)
: QFrame(parent)
, m_sdi(sdi)
, m_sizeTargetScreen(screenWidth, screenHeight)
{
	// frame style and shape
	setFrameShape(QFrame::Box);
	setLineWidth(2);

	// background color
	QPalette p = palette();
	p.setColor(backgroundRole(), sdi.getBackGroundColor());
	setPalette(p);
	setAutoFillBackground(true);

	// create video widget
	m_videoWidget = new HVideoWidget(sdi.getDisplayType(), sdi.isOriginalAspectRatioMaintained(), this);

	// layout
    HStimulusLayout *layout = new HStimulusLayout(m_videoWidget, sdi.getDisplayType(), sdi.isOriginalAspectRatioMaintained(), m_sizeTargetScreen);
    setLayout(layout);
}

HStimulusWidget::~HStimulusWidget()
{
}

