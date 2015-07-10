/*
 * HStimulusPreviewWidget.h
 *
 *  Created on: Jul 8, 2015
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSPREVIEWWIDGET_H_
#define HSTIMULUSPREVIEWWIDGET_H_

#include <QWidget>
#include <QDir>
#include <QList>
#include "stimulisettings.h"
#include "stimulusdisplayinfo.h"
#include "HMediaManager.h"

namespace GUILib
{

	class HStimulusPreviewWidget: public QWidget
	{
		Q_OBJECT
		const Habit::HStimulusSettingsList m_stimlist;
		HMediaManager *m_pmm;

	public:
		HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, const QDir& dirStimRoot = QDir(), QWidget *parent = NULL);
		~HStimulusPreviewWidget() {};
		void preview(const Habit::StimulusSettings& stimulus);
		void clear();

	protected slots:

	};

};

#endif /* HSTIMULUSPREVIEWWIDGET_H_ */
