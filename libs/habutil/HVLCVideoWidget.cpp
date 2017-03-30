/*
 * HVLCPlayerWidget.cpp
 *
 *  Created on: Mar 16, 2017
 *      Author: dan
 */

#include "HVLCVideoWidget.h"

#include <QVBoxLayout>
#include <QSize>
#include <QDesktopWidget>
#include <QApplication>
#include <QtDebug>

HVLCVideoWidget::HVLCVideoWidget(HVLCMediaPlayer *player, QWidget *parent, const QColor& bkgd, const QSize& minSize)
: QFrame(parent)
{
	m_pVlcWidgetVideo = new VlcWidgetVideo(player->vlcMediaPlayer(), this, bkgd);
	player->vlcMediaPlayer()->setVideoWidget(m_pVlcWidgetVideo);

	// We set contents margins to 0, but leave the layout margins and frame shape
	// at their defaults. That will leave a small border around the video viewer.
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(m_pVlcWidgetVideo);
	m_pVlcWidgetVideo->show();
	setLayout(layout);
	setContentsMargins(0, 0, 0, 0);
	setMinimumSize(minSize);
}

HVLCVideoWidget::HVLCVideoWidget(HVLCMediaPlayer *player, int screen, const QColor& bkgd)
: QFrame(NULL)
{
	m_pVlcWidgetVideo = new VlcWidgetVideo(player->vlcMediaPlayer(), NULL, bkgd);
	player->vlcMediaPlayer()->setVideoWidget(m_pVlcWidgetVideo);

	// Construction is similar: layout with VlcWidgetVideo as only widget.
	// Here we set the margins of the LAYOUT to 0. We also set the frame shape to
	// Qt::NoFrame. Finally, the geometry is set to the window geometry. Also the
	// FramelessWindowHint in windowFlags().
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_pVlcWidgetVideo);
	m_pVlcWidgetVideo->show();
	setLayout(layout);
	setContentsMargins(0, 0, 0, 0);
	setGeometry(QApplication::desktop()->screenGeometry(screen));
	//m_pVlcWidgetVideo->setGeometry(QApplication::desktop()->screenGeometry(screen));
	setFrameShape(QFrame::NoFrame);
	//setLineWidth(0);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
}

HVLCVideoWidget::~HVLCVideoWidget()
{
}


void HVLCVideoWidget::sizeChanged(unsigned int x, unsigned int y)
{
	// What is current size of widget?
	qInfo() << "HVLCPlayerWidget geometry(): " << this->geometry();
	qInfo() << "VLCWidgetVideo geometry(): " << m_pVlcWidgetVideo->geometry();
	//qInfo() << "Frame width " << frameWidth();
	//qInfo() << "Line widgth " << lineWidth();
	//qInfo() << "Mid Line width " << midLineWidth();

	int left, top, right, bottom;
	layout()->getContentsMargins(&left, &top, &right, &bottom);
	//qInfo() << "left, top, right, bottom = " << left << " " << top << " " << right << " " << bottom;

	// Now what should be resized?
	m_pVlcWidgetVideo->setMinimumSize(x, y);
	m_pVlcWidgetVideo->setMaximumSize(x, y);
	layout()->setAlignment(m_pVlcWidgetVideo, Qt::AlignCenter);
}
