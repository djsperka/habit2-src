/*
 * StimDisplayWidget.cpp
 *
 *  Created on: Jan 14, 2020
 *      Author: dan
 */

#include "StimDisplayWidget.h"
#include <QHBoxLayout>

StimDisplayWidget::StimDisplayWidget(const Habit::StimulusDisplayInfo& sdi, unsigned long width, unsigned long height, const QWidget *parent)
{
	QHBoxLayout *hbox = new QHBoxLayout;
	if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HStimulusWidget *w0 = new HStimulusWidget(sdi, 320, 240, this);
		m_vecVideoWidgets.push_back(w0->getHVideoWidget());
		hbox->addWidget(w0);
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HStimulusWidget *w0 = new HStimulusWidget(sdi, 320, 240, this);
		HStimulusWidget *w1 = new HStimulusWidget(sdi, 320, 240, this);
		m_vecVideoWidgets.push_back(w0->getHVideoWidget());
		m_vecVideoWidgets.push_back(w1->getHVideoWidget());
		hbox->addWidget(w0);
		hbox->addWidget(w1);
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		HStimulusWidget *w0 = new HStimulusWidget(sdi, 320, 240, this);
		HStimulusWidget *w1 = new HStimulusWidget(sdi, 320, 240, this);
		HStimulusWidget *w2 = new HStimulusWidget(sdi, 320, 240, this);
		m_vecVideoWidgets.push_back(w0->getHVideoWidget());
		m_vecVideoWidgets.push_back(w1->getHVideoWidget());
		m_vecVideoWidgets.push_back(w2->getHVideoWidget());
		hbox->addWidget(w0);
		hbox->addWidget(w1);
		hbox->addWidget(w2);
	}
	setLayout(hbox);
}

StimDisplayWidget::StimDisplayWidget(unsigned int n, bool bOriginalSize, bool bMaintainAspectRatio, unsigned int rgba, unsigned long width, unsigned long height, const QWidget *parent)
{
	QHBoxLayout *hbox = new QHBoxLayout;
	for (int i=0; i<n; i++)
	{
		HStimulusWidget *w = new HStimulusWidget(bOriginalSize, bMaintainAspectRatio, rgba, 320, 240, this);
		m_vecVideoWidgets.push_back(w->getHVideoWidget());
		hbox->addWidget(w);
	}
	setLayout(hbox);
}

StimDisplayWidget::~StimDisplayWidget()
{
}

HVideoWidget *StimDisplayWidget::getVideoWidget(int index)
{
	HVideoWidget *pwidget = NULL;
	if (index < m_vecVideoWidgets.size())
		pwidget = m_vecVideoWidgets[index];
	return pwidget;
}
