/*
 * HStimulusUtil.cpp
 *
 *  Created on: Dec 5, 2014
 *      Author: Oakeslab
 */

#include "HStimulusUtil.h"
#include "HWorkspaceUtil.h"
#include <QDir>
#include <QtDebug>
#include <QRegExp>
#include <QColor>

static const QString sreCOLOR("color\\((.*)\\)");
static const QString sreRGB("rgb\\((\\d+),(\\d+),(\\d+)\\)");


bool habutilStimulusColorOK(const QString& color)
{
	bool b = false;
	QRegExp reColor(sreCOLOR);
	QRegExp reRGB(sreRGB);

	if (reColor.exactMatch(color))
	{
		QString specColor(reColor.cap(1));
		if (specColor == QString("background"))
			b = true;
		else
		{
			QColor c(specColor);
			b = c.isValid();
		}
		if (!b)
			qDebug() << "Color spec is invalid: " << color;
	}
	else if (reRGB.exactMatch(color))
	{
		bool bR, bG, bB;
		int iR, iG, iB;
		iR = reRGB.cap(1).toInt(&bR);
		iG = reRGB.cap(1).toInt(&bG);
		iB = reRGB.cap(1).toInt(&bB);
		b = (bR && bG && bB && (0<=iR<=255) && (0<=iG<=255) && (0<=iB<=255));
		if (!b)
			qDebug() << "Color spec rgb values out of range (0-255): " << color;
	}
	return b;
}


bool habutilStimulusFilesFound(const Habit::HStimulusSettings& settings, const HStimulusLayoutType& layoutType)
{
	bool b = true;
	QDir stimRootDir;
	habutilGetStimulusRootDir(stimRootDir);
	if (layoutType == HStimulusLayoutType::HStimulusLayoutSingle)
		b = (
				(
					!settings.getCenterStimulusInfo().getFileName().isEmpty() &&
					(stimRootDir.exists(settings.getCenterStimulusInfo().getFileName()) || habutilStimulusColorOK(settings.getCenterStimulusInfo().getFileName()))
				) ||
				settings.getCenterStimulusInfo().isBackground()
			);
	else if (layoutType == HStimulusLayoutType::HStimulusLayoutLeftRight)
		b = (
				(
					!settings.getLeftStimulusInfo().getFileName().isEmpty() &&
					(stimRootDir.exists(settings.getLeftStimulusInfo().getFileName()) || habutilStimulusColorOK(settings.getLeftStimulusInfo().getFileName()))
				) ||
				settings.getLeftStimulusInfo().isBackground()
			) &&
			(
				(
					!settings.getRightStimulusInfo().getFileName().isEmpty() &&
					(stimRootDir.exists(settings.getRightStimulusInfo().getFileName()) || habutilStimulusColorOK(settings.getRightStimulusInfo().getFileName()))
				) ||
				settings.getRightStimulusInfo().isBackground()
			);
	else
	{
		b = false;
		qWarning() << "habutilStimulusFilesFound: stimulus layout type " << layoutType.name() << " is not handled here.";
	}
	return b;
}



