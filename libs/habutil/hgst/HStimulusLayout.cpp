/*
 * HStimulusLayout.cpp
 *
 *  Created on: Oct 6, 2017
 *      Author: dan
 */

#include "HStimulusLayout.h"

HStimulusLayout::HStimulusLayout(HVideoWidget *pVideoWidget, const HDisplayType& displayType, bool bMaintainAspectRatio, const QSize& sizeTargetScreen)
: QLayout()
, m_pVideoWidget(pVideoWidget)
, m_displayType(displayType)
, m_bMaintainAspectRatio(bMaintainAspectRatio)
, m_targetDisplaySize(sizeTargetScreen)
{
}

HStimulusLayout::~HStimulusLayout()
{
}

int HStimulusLayout::count() const
{
	// QList::size() returns the number of QLayoutItems in the list
	return m_list.size();
}

QLayoutItem *HStimulusLayout::itemAt(int idx) const
{
	 // QList::value() performs index checking, and returns 0 if we are
	 // outside the valid range
	 return 0; //m_list.value(idx);
}

QLayoutItem *HStimulusLayout::takeAt(int idx)
{
	// QList::take does not do index checking
	return 0; //idx >= 0 && idx < m_list.size() ? m_list.takeAt(idx) : 0;
}

QSize HStimulusLayout::sizeHint() const
{
	return QSize(320, 240);
}

QSize HStimulusLayout::minimumSize() const
{
	return sizeHint();
}

void HStimulusLayout::setGeometry(const QRect& r)
{
	// get size of stimulus from video widget
	//QSize stimActualSize = m_pVideoWidget->getStimulusSize();

	// stick it in the upper left corner
	qDebug() << "setGeometry - rect " << r;

	// hack - hard code 320x240
	m_pVideoWidget->setGeometry(QRect(0, 0, 320, 240));
}
