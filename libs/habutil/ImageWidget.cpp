/*
 *  ImageWidget.cpp
 *  myp
 *
 *  Created by Oakes Lab on 5/22/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ImageWidget.h"

#include <QtGui/QPainter>
#include <QtCore/QDebug>

ImageWidget::ImageWidget(QWidget *parent, bool fullScreen, bool aspectRatio)
: QWidget(parent), m_fullscreen(fullScreen), m_maintainAspectRatio(aspectRatio)
{	
	setAutoFillBackground(true);
	setObjectName("ImageWidget");
}

ImageWidget::~ImageWidget()
{
	
}

void ImageWidget::setCurrentSource(const QString& filename)
{
	m_image.load(filename);
	QSize size = m_image.size();
	QSize parentSize = this->size();
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
	}
	repaint();
	return;
}

void ImageWidget::paintEvent(QPaintEvent* event) 
{ 
	Q_UNUSED(event);
    QPainter painter(this); 
	QPalette palette = this->palette();
	painter.fillRect(this->rect(), palette.color(QPalette::Window));
	int x = (this->width() - m_image.width())/2;
	int y = (this->height() - m_image.height())/2;
    painter.drawImage(x, y, m_image); 
	emit painted();
}  
