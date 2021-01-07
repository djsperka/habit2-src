/*
 *  HMediaManagerUtil.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/18/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HGMM.h"
#include "HStimulusWidget.h"
#include "HStimPipeline.h"
#include <QtGlobal>

#include <QDesktopWidget>
#include <QGuiApplication>
#include <QScreen>
#include "HWorkspaceUtil.h"

HGMM* createMediaManager(const Habit::StimulusDisplayInfo& sdi)
{
	QVector<HStimulusWidget *> vecWidgets;
	if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		//QRect rect = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Center));
		QRect rect = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Center))->availableGeometry();
		HStimulusWidget *pSingle = new HStimulusWidget(sdi, rect.width(), rect.height());
		pSingle->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pSingle->move(rect.x(), rect.y());
		qDebug() << "Center player index " << habutilGetMonitorID(HPlayerPositionType::Center) << " moved to rect " << rect;
		vecWidgets.append(pSingle);
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		//QRect rectLeft = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Left));
		QRect rectLeft = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Left))->availableGeometry();
		HStimulusWidget *pLeft = new HStimulusWidget(sdi, rectLeft.width(), rectLeft.height());
		pLeft->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pLeft->move(rectLeft.x(), rectLeft.y());
		qDebug() << "Left player index " << habutilGetMonitorID(HPlayerPositionType::Left) << " moved to rect " << rectLeft;

		//QRect rectRight = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Right));
		QRect rectRight = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Right))->availableGeometry();
		HStimulusWidget *pRight = new HStimulusWidget(sdi, rectRight.width(), rectRight.height());
		pRight->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pRight->move(rectRight.x(), rectRight.y());
		qDebug() << "Right player index " << habutilGetMonitorID(HPlayerPositionType::Right) << " moved to rect " << rectRight;

		vecWidgets.append(pLeft);
		vecWidgets.append(pRight);
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		//QRect rectLeft = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Left));
		QRect rectLeft = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Left))->availableGeometry();
		HStimulusWidget *pLeft = new HStimulusWidget(sdi, rectLeft.width(), rectLeft.height());
		pLeft->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pLeft->move(rectLeft.x(), rectLeft.y());
		qDebug() << "Left player index " << habutilGetMonitorID(HPlayerPositionType::Left) << " moved to rect " << rectLeft;

		//QRect rectCenter = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Center));
		QRect rectCenter = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Center))->availableGeometry();
		HStimulusWidget *pCenter = new HStimulusWidget(sdi, rectCenter.width(), rectCenter.height());
		pCenter->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pCenter->move(rectCenter.x(), rectCenter.y());
		qDebug() << "Center player index " << habutilGetMonitorID(HPlayerPositionType::Center) << " moved to rect " << rectCenter;

		//QRect rectRight = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Right));
		QRect rectRight = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Right))->availableGeometry();
		HStimulusWidget *pRight = new HStimulusWidget(sdi, rectRight.width(), rectRight.height());
		pRight->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pRight->move(rectRight.x(), rectRight.y());
		qDebug() << "Right player index " << habutilGetMonitorID(HPlayerPositionType::Right) << " moved to rect " << rectRight;

		vecWidgets.append(pLeft);
		vecWidgets.append(pCenter);
		vecWidgets.append(pRight);
	}
	return new HGMM(sdi, vecWidgets, habutilGetStimulusRootDir());
}

HGMM* createMediaManager(const Habit::StimulusDisplayInfo& sdi, int w, int h)
{
	QVector<HStimulusWidget *> vecWidgets;
	if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		vecWidgets.append(new HStimulusWidget(sdi, w, h));
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		vecWidgets.append(new HStimulusWidget(sdi, w, h));
		vecWidgets.append(new HStimulusWidget(sdi, w, h));
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		vecWidgets.append(new HStimulusWidget(sdi, w, h));
		vecWidgets.append(new HStimulusWidget(sdi, w, h));
		vecWidgets.append(new HStimulusWidget(sdi, w, h));
	}
	return new HGMM(sdi, vecWidgets, habutilGetStimulusRootDir());
}
