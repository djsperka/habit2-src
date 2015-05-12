/*
 *  ImageWidget.h
 *  myp
 *
 *  Created by Oakes Lab on 5/22/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QImage>

class HImageWidget : public QWidget
{
	Q_OBJECT
	
public:
	HImageWidget(QWidget *parent = 0, bool fullScreen = false, bool aspectRatio = false, QColor background = QColor("black"));
	~HImageWidget();
	void setCurrentSource(const QString& filename);
protected:
	void paintEvent ( QPaintEvent * event );
	
private:
	QImage m_image;
	bool m_fullscreen;
	bool m_maintainAspectRatio;
	
signals:
	void painted();
};

#endif // IMAGEWIDGET_H
