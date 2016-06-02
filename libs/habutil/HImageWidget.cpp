/*
 *  HImageWidget.cpp
 *  myp
 *
 *  Created by Oakes Lab on 5/22/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HImageWidget.h"

#include <QtGui/QPainter>
#include <QtCore/QDebug>

HImageWidget::HImageWidget(QWidget *parent, bool fullScreen, bool aspectRatio, QColor background)
: QWidget(parent), m_fullscreen(fullScreen), m_maintainAspectRatio(aspectRatio)
{	
	QPalette p = palette();
	p.setColor(backgroundRole(), background);
	setPalette(p);
	setAutoFillBackground(true);
}

HImageWidget::~HImageWidget()
{
}

void HImageWidget::setCurrentSource(QImage* pImage)
{
	m_pImage = pImage;
	repaint();
	return;
}

void HImageWidget::setCurrentSource(const QString& filename)
{
	m_image.load(filename);
	QSize size = m_image.size();
	QSize parentSize = this->size();

	qDebug() << "HImageWidget::setCurrentSource(" << filename << ")";
	qDebug() << "              parent size " << parentSize.width() << "x" << parentSize.height();
	qDebug() << "              image  size " << size.width() << "x" << size.height();

#if 0
	if (size.width() > parentSize.width())
	{
		m_image =m_image.scaledToWidth(parentSize.width(), Qt::SmoothTransformation);
		size = m_image.size();
	}
	if (size.height() > parentSize.height())
	{
		m_image = m_image.scaledToWidth(parentSize.height(), Qt::SmoothTransformation);
		size = m_image.size();
	}
	qDebug() << "              image scal " << size.width() << "x" << size.height();
#endif
	if (m_fullscreen)
	{
		double scaleX = size.width()*1.0 / parentSize.width();
		double scaleY = size.height()*1.0 / parentSize.height();
		if (m_maintainAspectRatio)
		{
			m_image = (scaleX < scaleY)?(m_image.scaledToHeight(parentSize.height(), Qt::SmoothTransformation)):
			(m_image.scaledToWidth(parentSize.width(), Qt::SmoothTransformation));
		}
		else
		{
			m_image = m_image.scaled(parentSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		}
		qDebug() << "            FSimage  size " << m_image.size().width() << "x" << m_image.size().height();
	}

	m_pImage = &m_image;

	repaint();

	return;
}

void HImageWidget::paintEvent(QPaintEvent* event) 
{ 
	Q_UNUSED(event);
    QPainter painter(this); 
	QPalette palette = this->palette();
	painter.fillRect(this->rect(), palette.color(QPalette::Window));
	int x = (this->width() - m_pImage->width())/2;
	int y = (this->height() - m_pImage->height())/2;
    painter.drawImage(x, y, *m_pImage);
	emit painted();
}  
