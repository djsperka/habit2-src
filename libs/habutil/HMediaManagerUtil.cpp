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
#include <QApplication>
#include "HWorkspaceUtil.h"

HGMM* createMediaManager(const Habit::ExperimentSettings& es)
{
	HGMM *pmm = NULL;
	QDir rootDir = habutilGetStimulusRootDir();

	if (es.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		QRect rect = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Center));
		HStimulusWidget *pSingle = new HStimulusWidget(es.getStimulusDisplayInfo(), rect.width(), rect.height());

		//pmm = new HGMM(pSingle, rootDir, es.getStimulusDisplayInfo().getUseISS(), es.getStimulusDisplayInfo().getBackGroundColor(), HStimPipelineFactory);
		pmm = &HGMM::instance();
		pmm->reset(es, rootDir);
		pmm->setWidgets(pSingle);
		pSingle->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pSingle->move(rect.x(), rect.y());
		pSingle->showFullScreen();
		qDebug() << "Center player index " << habutilGetMonitorID(HPlayerPositionType::Center) << " moved to rect " << rect;
	}
	else if (es.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		QRect rectLeft = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Left));
		HStimulusWidget *pLeft = new HStimulusWidget(es.getStimulusDisplayInfo(), rectLeft.width(), rectLeft.height());
		pLeft->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pLeft->move(rectLeft.x(), rectLeft.y());
		pLeft->showFullScreen();
		qDebug() << "Left player index " << habutilGetMonitorID(HPlayerPositionType::Left) << " moved to rect " << rectLeft;

		QRect rectRight = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Right));
		HStimulusWidget *pRight = new HStimulusWidget(es.getStimulusDisplayInfo(), rectRight.width(), rectRight.height());
		pRight->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		pRight->move(rectRight.x(), rectRight.y());
		pRight->showFullScreen();
		qDebug() << "Right player index " << habutilGetMonitorID(HPlayerPositionType::Right) << " moved to rect " << rectRight;

		//pmm = new HGMM(pLeft, pRight, rootDir, es.getStimulusDisplayInfo().getUseISS(), es.getStimulusDisplayInfo().getBackGroundColor(), HStimPipelineFactory);
		pmm = &HGMM::instance();
		pmm->reset(es, rootDir);
		pmm->setWidgets(pLeft, pRight);
	}
	Q_ASSERT(pmm);
	return pmm;
}



HGMM* createMediaManager(const Habit::StimulusDisplayInfo& sdi, int screenWidth, int screenHeight)
{
	HGMM* pmm;
	QDir rootDir = habutilGetStimulusRootDir();

	//	    HVideoWidget(QGst::Ui::VideoWidget *widget, float screenWidth, float screenHeight, const HDisplayType& dType = HDisplayType::HDisplayTypeOriginalSize, bool bMaintainAspectRatio = true, QWidget *parent = 0);

	if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HStimulusWidget *pSingle = new HStimulusWidget(sdi, screenWidth, screenHeight);
		//pmm = new HGMM(pSingle, rootDir, sdi.getUseISS(), sdi.getBackGroundColor(), HStimPipelineFactory);
		pmm = &HGMM::instance();
		pmm->reset(pSingle, sdi, rootDir);
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HStimulusWidget *pLeft = new HStimulusWidget(sdi, screenWidth, screenHeight);
		HStimulusWidget *pRight = new HStimulusWidget(sdi, screenWidth, screenHeight);
		//pmm = new HGMM(pLeft, pRight, rootDir, sdi.getUseISS(), sdi.getBackGroundColor(), HStimPipelineFactory);
		pmm = &HGMM::instance();
		pmm->reset(pLeft, pRight, sdi, rootDir);
	}
	else
	{
		qFatal("Unknown stimulus layout type in createMediaManager");
	}

	return pmm;
}

HGMM* createMediaManager(const Habit::ExperimentSettings& es, int screenWidth, int screenHeight)
{
	return createMediaManager(es.getStimulusDisplayInfo(), screenWidth, screenHeight);
}
