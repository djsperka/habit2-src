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

#include <QGuiApplication>
#include <QScreen>
#include "HWorkspaceUtil.h"

void initializeMediaManager(const Habit::ExperimentSettings& es)
{
	QDir rootDir = habutilGetStimulusRootDir();
	HGMM::instance().reset(es, rootDir);

	if (es.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		QRect rect = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Center))->geometry();
		HStimulusWidget *pSingle = new HStimulusWidget(es.getStimulusDisplayInfo(), rect.width(), rect.height());

		HGMM::instance().setWidgets(pSingle);
		pSingle->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pSingle->move(rect.x(), rect.y());
		qDebug() << "Center player index " << habutilGetMonitorID(HPlayerPositionType::Center) << " moved to rect " << rect;
	}
	else if (es.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		QRect rectLeft = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Left))->geometry();
		HStimulusWidget *pLeft = new HStimulusWidget(es.getStimulusDisplayInfo(), rectLeft.width(), rectLeft.height());
		pLeft->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pLeft->move(rectLeft.x(), rectLeft.y());
		qDebug() << "Left player index " << habutilGetMonitorID(HPlayerPositionType::Left) << " moved to rect " << rectLeft;

		QRect rectRight = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Right))->geometry();
		HStimulusWidget *pRight = new HStimulusWidget(es.getStimulusDisplayInfo(), rectRight.width(), rectRight.height());
		pRight->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pRight->move(rectRight.x(), rectRight.y());
		qDebug() << "Right player index " << habutilGetMonitorID(HPlayerPositionType::Right) << " moved to rect " << rectRight;

		HGMM::instance().setWidgets(pLeft, pRight);
	}
	else if (es.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		QRect rectLeft = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Left))->geometry();
		HStimulusWidget *pLeft = new HStimulusWidget(es.getStimulusDisplayInfo(), rectLeft.width(), rectLeft.height());
		pLeft->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pLeft->move(rectLeft.x(), rectLeft.y());
		qDebug() << "Left player index " << habutilGetMonitorID(HPlayerPositionType::Left) << " moved to rect " << rectLeft;

		QRect rectCenter = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Center))->geometry();
		HStimulusWidget *pCenter = new HStimulusWidget(es.getStimulusDisplayInfo(), rectCenter.width(), rectCenter.height());
		pCenter->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pCenter->move(rectCenter.x(), rectCenter.y());
		qDebug() << "Center player index " << habutilGetMonitorID(HPlayerPositionType::Center) << " moved to rect " << rectCenter;

		QRect rectRight = QGuiApplication::screens().at(habutilGetMonitorID(HPlayerPositionType::Right))->geometry();
		HStimulusWidget *pRight = new HStimulusWidget(es.getStimulusDisplayInfo(), rectRight.width(), rectRight.height());
		pRight->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pRight->move(rectRight.x(), rectRight.y());
		qDebug() << "Right player index " << habutilGetMonitorID(HPlayerPositionType::Right) << " moved to rect " << rectRight;

		HGMM::instance().setWidgets(pLeft, pCenter, pRight);
	}

	return;
}

void initializeMediaManager(const Habit::ExperimentSettings& es, int screenWidth, int screenHeight)
{
//	return initializeMediaManager(es.getStimulusDisplayInfo(), screenWidth, screenHeight);
	QDir rootDir = habutilGetStimulusRootDir();
	HGMM::instance().reset(es, rootDir);

	Habit::StimulusDisplayInfo sdi = es.getStimulusDisplayInfo();
	if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HStimulusWidget *pSingle = new HStimulusWidget(sdi, screenWidth, screenHeight);
		HGMM::instance().modifyStimulusDisplay(sdi, rootDir);
		HGMM::instance().setWidgets(pSingle);
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HStimulusWidget *pLeft = new HStimulusWidget(sdi, screenWidth, screenHeight);
		HStimulusWidget *pRight = new HStimulusWidget(sdi, screenWidth, screenHeight);
		HGMM::instance().modifyStimulusDisplay(sdi, rootDir);
		HGMM::instance().setWidgets(pLeft, pRight);
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		HStimulusWidget *pLeft = new HStimulusWidget(sdi, screenWidth, screenHeight);
		HStimulusWidget *pRight = new HStimulusWidget(sdi, screenWidth, screenHeight);
		HStimulusWidget *pCenter = new HStimulusWidget(sdi, screenWidth, screenHeight);
		HGMM::instance().modifyStimulusDisplay(sdi, rootDir);
		HGMM::instance().setWidgets(pLeft, pCenter, pRight);
	}
	else
	{
		qFatal("Unknown stimulus layout type in initializeMediaManager");
	}

	return;
}
