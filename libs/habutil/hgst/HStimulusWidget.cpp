/*
 * HVideoWidget.cpp
 *
 *  Created on: Aug 17, 2017
 *      Author: dan
 */


#include <hgst/HStimulusWidget.h>



HStimulusWidget::HStimulusWidget(const Habit::StimulusDisplayInfo& sdi, float screenWidth, float screenHeight, QWidget *parent)
: QFrame(parent)
, m_sdi(sdi)
, m_screenWidth(screenWidth)
, m_screenHeight(screenHeight)
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
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setContentsMargins(0, 0, 0, 0);
    //QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //policy.setHeightForWidth(true);
    //setSizePolicy(policy);
    //hbox->addStretch(1);
    hbox->addWidget(m_videoWidget);
    //hbox->addStretch(1);
    setLayout(hbox);
}

HStimulusWidget::~HStimulusWidget()
{
}

