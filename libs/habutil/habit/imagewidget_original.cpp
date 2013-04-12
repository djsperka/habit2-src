#include "imagewidget_original.h"

#include <QtGui/QPainter>
#include <QtCore/QDebug>


namespace System
{


ImageWidget::ImageWidget(QWidget *parent)
	: QWidget(parent)
{
	
}

ImageWidget::~ImageWidget()
{

}

bool ImageWidget::setFileName(const QString& fileName, bool fullScreen, bool aspectRatio )
{
	if (!_image.load(fileName))
		return false;
	QSize size = _image.size();
	QSize parentSize = this->size();
	if (size.width() > parentSize.width())
	{
		_image = _image.scaledToWidth(parentSize.width(), Qt::SmoothTransformation);
		size = _image.size();
	}
	if (size.height() > parentSize.height())
	{
		_image = _image.scaledToWidth(parentSize.height(), Qt::SmoothTransformation);
		size = _image.size();
	}
	if (fullScreen)
	{
		double scaleX = size.width()*1.0 / parentSize.width();
		double scaleY = size.height()*1.0 / parentSize.height();
		if (aspectRatio)
		{
			_image = (scaleX < scaleY)?(_image.scaledToHeight(parentSize.height(), Qt::SmoothTransformation)):
				(_image.scaledToWidth(parentSize.width(), Qt::SmoothTransformation));
		}
		else
		{
			_image = _image.scaled(parentSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		}
	}
	qDebug() << "_image.width=" << _image.width() << "_image.height" << _image.height();
	repaint();
	return true;
}

void ImageWidget::paintEvent(QPaintEvent* event) 
{ 
	Q_UNUSED(event);
    QPainter painter(this); 
	QPalette palette = this->palette();
	painter.fillRect(this->rect(), palette.color(QPalette::Window));
	int x = (this->width() - _image.width())/2;
	int y = (this->height() - _image.height())/2;
	qDebug() << "x=" << x << " y=" << y;
	qDebug() << "image.width=" << _image.width() << "image.height" << _image.height();
	qDebug() << "win.width=" << this->width() << "win.height" << this->height();
    painter.drawImage(x,y, _image); 
}  
	
};	
