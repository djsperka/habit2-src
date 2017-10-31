/*
 *  HMediaManagerUtil.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 9/18/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <hgst/HStimulusWidget.h>
#include "HMediaManagerUtil.h"
#include <QtGlobal>

#if QT_VERSION >= 0x050000
#include <QDesktopWidget>
#include <QApplication>
#else
#include "HVIPlayer.h"
#include "HAudioPlayer.h"
#endif
#include "HWorkspaceUtil.h"


#if QT_VERSION >= 0x050000


// Always first create a media manager.

HGMM* createMediaManager(const Habit::ExperimentSettings& es)
{
	HGMM *pmm = NULL;
	QDir rootDir;
	habutilGetStimulusRootDir(rootDir);

	if (es.getStimulusDisplayInfo().getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		QRect rect = QApplication::desktop()->screenGeometry(habutilGetMonitorID(HPlayerPositionType::Center));
		HStimulusWidget *pSingle = new HStimulusWidget(es.getStimulusDisplayInfo(), rect.width(), rect.height());
		pmm = new HGMM(pSingle, rootDir, es.getStimulusDisplayInfo().getUseISS(), es.getStimulusDisplayInfo().getBackGroundColor());

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

		pmm = new HGMM(pLeft, pRight, rootDir, es.getStimulusDisplayInfo().getUseISS(), es.getStimulusDisplayInfo().getBackGroundColor());
	}
	Q_ASSERT(pmm);
	return pmm;
}


HGMM* createMediaManager(const Habit::ExperimentSettings& es, int screenWidth, int screenHeight)
{
	return createMediaManager(es.getStimulusDisplayInfo(), screenWidth, screenHeight);
}

HGMM* createMediaManager(const Habit::StimulusDisplayInfo& sdi, int screenWidth, int screenHeight)
{
	HGMM* pmm;
	QDir rootDir;
	habutilGetStimulusRootDir(rootDir);

	//	    HVideoWidget(QGst::Ui::VideoWidget *widget, float screenWidth, float screenHeight, const HDisplayType& dType = HDisplayType::HDisplayTypeOriginalSize, bool bMaintainAspectRatio = true, QWidget *parent = 0);

	if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HStimulusWidget *pSingle = new HStimulusWidget(sdi, screenWidth, screenHeight);
		pmm = new HGMM(pSingle, rootDir, sdi.getUseISS(), sdi.getBackGroundColor());
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HStimulusWidget *pLeft = new HStimulusWidget(sdi, screenWidth, screenHeight);
		HStimulusWidget *pRight = new HStimulusWidget(sdi, screenWidth, screenHeight);
		pmm = new HGMM(pLeft, pRight, rootDir, sdi.getUseISS(), sdi.getBackGroundColor());
	}
	else
	{
		qFatal(QString("Unknown stimulus layout type in createMediaManager: ").append(sdi.getStimulusLayoutType().name()).toStdString().c_str());
	}

	return pmm;
}

#else

HGMM* createMediaManager(const Habit::ExperimentSettings& es)
{
	HGMM* pmm = new HGMM();

	// Stimulus Display info
	Habit::StimulusDisplayInfo sdi = es.getStimulusDisplayInfo();
	// Create players for each monitor that will be used. 
	int iControl = habutilGetMonitorID(HPlayerPositionType::Control);
	int iLeft = habutilGetMonitorID(HPlayerPositionType::Left);
	int iCenter = habutilGetMonitorID(HPlayerPositionType::Center);
	int iRight = habutilGetMonitorID(HPlayerPositionType::Right);
	const HStimulusLayoutType& layoutType = sdi.getStimulusLayoutType();

	QDir baseDir;
	habutilGetStimulusRootDir(baseDir);


	if (iControl > -1 && sdi.getUseISS())
	{
		HPlayer *audio = new HAudioPlayer(iControl, NULL, baseDir);
		pmm->addPlayer(HPlayerPositionType::Sound, audio, iControl);
	}
	if (layoutType == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		HPlayer *single = new HVIPlayer(iCenter, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor(), sdi.isOriginalAspectRatioMaintained());
		single->setPreferBufferedStimulus(false);
		pmm->addPlayer(HPlayerPositionType::Center, single, iCenter);
	}
	else if (layoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		HPlayer *left = new HVIPlayer(iLeft, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor(), sdi.isOriginalAspectRatioMaintained());
		HPlayer *right = new HVIPlayer(iRight, NULL, baseDir, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor(), sdi.isOriginalAspectRatioMaintained());
		pmm->addPlayer(HPlayerPositionType::Left, left, iLeft);
		pmm->addPlayer(HPlayerPositionType::Right, right, iRight);
	}
	
	return pmm;
}


HPreviewMediaManager* createPreviewMediaManager(const Habit::StimulusDisplayInfo& sdi)
{
	HPreviewMediaManager* pmm = new HPreviewMediaManager(sdi.getStimulusLayoutType());
	HPlayer *psound = NULL;
	if (sdi.getUseISS())
	{
		psound = new HAudioPlayer(-1, NULL, dirStimRoot);
		pmm->addPlayer(HPlayerPositionType::Sound, p, -1);
	}

	HPlayer *p0 = new HVIPlayer(-1, NULL, dirStimRoot, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor());
	p0->setPreferBufferedStimulus(false);
	HPlayer *p1l = new HVIPlayer(-1, NULL, dirStimRoot, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor());
	p1l->setPreferBufferedStimulus(false);
	HPlayer *p1r = new HVIPlayer(-1, NULL, dirStimRoot, sdi.getDisplayType() == HDisplayType::HDisplayTypeFullScreen, sdi.getBackGroundColor());
	p1r->setPreferBufferedStimulus(false);

	pmm->addPlayers(p0, p1l, p1r, psound);


}

#endif  /* QT_VERSION < 5 */
