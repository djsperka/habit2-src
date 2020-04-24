/*
 * HVideoWidget.cpp
 *
 *  Created on: Aug 17, 2017
 *      Author: dan
 */


#include <hgst/HStimulusWidget.h>
#include <hgst/HStimulusLayout.h>
#include <QHBoxLayout>


HStimulusWidget::HStimulusWidget(const Habit::StimulusDisplayInfo& sdi, float screenWidth, float screenHeight, QWidget *parent)
: QFrame(parent)
, m_sizeTargetScreen(screenWidth, screenHeight)
{
	// frame style and shape
	//setFrameShape(QFrame::Box);
	//setLineWidth(2);

	// background color
	QPalette p = palette();
	p.setColor(backgroundRole(), sdi.getBackGroundColor());
	setPalette(p);
	setAutoFillBackground(true);

	// create video widget
	m_videoWidget = new HVideoWidget(this);

    HStimulusLayout *layout = new HStimulusLayout(m_videoWidget, sdi.getDisplayType()==HDisplayType::HDisplayTypeOriginalSize, sdi.isOriginalAspectRatioMaintained(), m_sizeTargetScreen);
    setLayout(layout);
}

HStimulusWidget::HStimulusWidget(bool bOriginalSize, bool bMaintainAspectRatio, unsigned int rgba, float screenWidth, float screenHeight, QWidget *parent)
: QFrame(parent)
, m_sizeTargetScreen(screenWidth, screenHeight)
{
	// background color
	QPalette p = palette();
	p.setColor(backgroundRole(), QColor(rgba));
	setPalette(p);
	setAutoFillBackground(true);

	// create video widget
	m_videoWidget = new HVideoWidget(this);

    HStimulusLayout *layout = new HStimulusLayout(m_videoWidget, bOriginalSize, bMaintainAspectRatio, m_sizeTargetScreen);
    setLayout(layout);
}

HStimulusWidget::~HStimulusWidget()
{
}

