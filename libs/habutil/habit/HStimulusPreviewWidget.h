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
#include <QPushButton>
#include <QLabel>
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
		QList<unsigned int> m_idList;
		unsigned int m_idListCurrent;
		QPushButton *m_pbDown;
		QPushButton *m_pbUp;
		QLabel *m_labelStimName;
		QPushButton *m_pbNext;
		QPushButton *m_pbPrev;
		QPushButton *m_pbStop;

		void updateNavigation();
		void updateNavigation(QString stimName);

	public:
		HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, const QDir& dirStimRoot = QDir(), QWidget *parent = NULL);
		~HStimulusPreviewWidget();
		void preview(const Habit::StimulusSettings& stimulus);
		void preview(const Habit::HStimulusSettingsList& stimuli, QList< QPair<int, QString> > list);
		void clear();

	protected slots:
		void nextClicked();
		void prevClicked();
		void stopClicked();
	};

};

#endif /* HSTIMULUSPREVIEWWIDGET_H_ */
