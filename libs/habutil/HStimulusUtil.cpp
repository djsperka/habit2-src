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
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QColor>

static const QString sreCOLOR("color\\((.*)\\)");
static const QString sreRGB("rgb\\((\\d+),(\\d+),(\\d+)\\)");

#define CHECK_ISS_FILE(f, dir) (f.isEmpty() || (dir.exists(f)))
#define CHECK_STIM_FILE(f, dir) ((!f.isEmpty() && dir.exists(f)) || habutilStimulusColorOK(f))

bool habutilStimulusColorOK(const QString& color)
{
	bool b = false;
	QRegularExpression reColor(sreCOLOR);
	QRegularExpression reRGB(sreRGB);
	QRegularExpressionMatch matchColor = reColor.match(color);
	QRegularExpressionMatch matchRGB = reRGB.match(color);
	if (matchColor.hasMatch())
	{
		QString specColor(matchColor.captured(1));
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
	else if (matchRGB.hasMatch())
	{
		bool bR, bG, bB;
		int iR, iG, iB;
		iR = matchRGB.captured(1).toInt(&bR);
		iG = matchRGB.captured(2).toInt(&bG);
		iB = matchRGB.captured(3).toInt(&bB);
		b = (bR && bG && bB && 0<=iR && iR<=255 && 0<=iG && iG<=255 && 0<=iB && iB<=255);
		if (!b)
			qDebug() << "Color spec rgb values out of range (0-255): " << color;
	}
	return b;
}


bool habutilStimulusFilesFound(const Habit::HStimulusSettings& settings, const Habit::StimulusDisplayInfo& info)
{
	bool b = true;
	QDir stimRootDir = habutilGetStimulusRootDir();
	if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		b = CHECK_STIM_FILE(settings.getCenterStimulusInfo().getFileName(), stimRootDir) ||
				settings.getCenterStimulusInfo().isBackground();
		if (info.getUseISS())
		{
			b = b && CHECK_ISS_FILE(settings.getIndependentSoundInfo().getFileName(), stimRootDir);
		}
	}
	else if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		b = (	CHECK_STIM_FILE(settings.getLeftStimulusInfo().getFileName(), stimRootDir) ||
				settings.getLeftStimulusInfo().isBackground()) &&
			(	CHECK_STIM_FILE(settings.getRightStimulusInfo().getFileName(), stimRootDir) ||
				settings.getRightStimulusInfo().isBackground());
		if (info.getUseISS())
		{
			b = b &&  CHECK_ISS_FILE(settings.getIndependentSoundInfo().getFileName(), stimRootDir);
		}

	}
	else if (info.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		b = (	CHECK_STIM_FILE(settings.getLeftStimulusInfo().getFileName(), stimRootDir) ||
				settings.getLeftStimulusInfo().isBackground()) &&
			(   CHECK_STIM_FILE(settings.getCenterStimulusInfo().getFileName(), stimRootDir) ||
				settings.getCenterStimulusInfo().isBackground()) &&
			(	CHECK_STIM_FILE(settings.getRightStimulusInfo().getFileName(), stimRootDir) ||
				settings.getRightStimulusInfo().isBackground());
		if (info.getUseISS())
		{
			b = b &&  CHECK_ISS_FILE(settings.getIndependentSoundInfo().getFileName(), stimRootDir);
		}
	}
	else
	{
		b = false;
		qWarning() << "habutilStimulusFilesFound: stimulus layout type " << info.getStimulusLayoutType().name() << " is not handled here.";
	}
	return b;
}



