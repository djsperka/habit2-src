/*
 * HStimulusLayout.cpp
 *
 *  Created on: Oct 6, 2017
 *      Author: dan
 */

#include "HStimulusLayout.h"

HStimulusLayout::HStimulusLayout(HVideoWidget *pVideoWidget, const HDisplayType& displayType, bool bMaintainAspectRatio, const QSize& sizeTargetScreen)
: QLayout()
, m_item(NULL)
, m_displayType(displayType)
, m_bMaintainAspectRatio(bMaintainAspectRatio)
, m_targetDisplaySize(sizeTargetScreen)
{
	addWidget(pVideoWidget);
}

HStimulusLayout::~HStimulusLayout()
{
	QLayoutItem *item = takeAt(0);
	if (item) delete item;
}

void HStimulusLayout::addItem(QLayoutItem *item)
{
	// Only one item allowed
	if (m_item)
	{
		qCritical() << "HStimulusLayout - only one item allowed!";
	}
	else
	{
		// only accept item if it holds an HVideoWidget
		HVideoWidget *w = dynamic_cast<HVideoWidget *>(item->widget());
		if (!w)
		{
			qCritical() << "HStimulusLayout: Only HVideoWidget items are supported!";
		}
		else
		{
			m_item = item;
		}
	}
	return;
}

int HStimulusLayout::count() const
{
	return (m_item ? 1 : 0);
}

QLayoutItem *HStimulusLayout::itemAt(int idx) const
{
	return (idx == 0 ? m_item : NULL);
}

QLayoutItem *HStimulusLayout::takeAt(int idx)
{
	QLayoutItem *item = NULL;
	if (idx == 0)
	{
		item = m_item;
		m_item = NULL;
	}
	return item;
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
	QLayout::setGeometry(r);
	if (!m_item) return;

	// If video widget has no size set we just assign full screen size.
	// This should only happen when stim is a solid color, and this is the behavior we'd want.

	HVideoWidget *w = dynamic_cast<HVideoWidget *>(m_item->widget());
	Q_ASSERT(w);		// this was checked in addItem

	if (w->getStimulusSize().isEmpty())
	{
		m_item->setGeometry(r);
	}
	else
	{
		if (m_displayType == HDisplayType::HDisplayTypeFullScreen)
		{
			QSize s = w->getStimulusSize().scaled(r.size(), m_bMaintainAspectRatio ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio);

			qDebug() << "setGeometry() rect " << r.size() << " maintain? " << m_bMaintainAspectRatio;
			qDebug() << "scaled " << s;
			// the geometry that we assign to the widget has its position as well as the size.
			// If the scaled size doesn't match that of the given rect 'r', then figure out which side
			// has the extra and split the difference

			if (s == r.size())
				m_item->setGeometry(r);
			else
			{
				if (s.width() == r.size().width())
				{
					int diff = r.size().height() - s.height();
					QRect rnew(QPoint(r.x(), r.y()+diff/2), s);
					m_item->setGeometry(rnew);
				}
				else
				{
					int diff = r.size().width() - s.width();
					QRect rnew(QPoint(r.x()+diff/2, r.y()), s);
					m_item->setGeometry(rnew);
				}
			}
		}
		else if (m_displayType == HDisplayType::HDisplayTypeOriginalSize)
		{
			// Scale stimulus size by factor of (actual screen size)/(target screen size)
			// Here, "actual screen size" can be in an aspect ration that doesn't match that of the target screen,
			// so we kludge it by just using the height ratio: (actual height)/(target height)
			double scaleFactor = (double)r.height()/(double)m_targetDisplaySize.height();

			// get scaled size for stimulus
			QSize s = w->getStimulusSize() * scaleFactor;
			int xdiff = r.width() - s.width();
			int ydiff = r.height() - s.height();
			QRect rnew(QPoint(r.x() + xdiff/2, r.y() + ydiff/2), s);
			m_item->setGeometry(rnew);
		}
		else
		{
			qCritical() << "HStimulusLayout - Unknown display type!";
		}
	}
//	// get size of stimulus from video widget
//	//QSize stimActualSize = m_pVideoWidget->getStimulusSize();
//
//	// stick it in the upper left corner
//	qDebug() << "setGeometry - rect " << r;
//
//	// hack - hard code 320x240
//	m_pVideoWidget->setGeometry(QRect(0, 0, 320, 240));
}
