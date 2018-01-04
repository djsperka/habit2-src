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
#include <QStackedWidget>
#include "stimulisettings.h"
#include "stimulusdisplayinfo.h"
//#include "HPreviewMediaManager.h"
#include "HGMM.h"

namespace GUILib
{

	class HStimulusPreviewWidget: public QWidget
	{
		Q_OBJECT
		bool m_bSingleStimulus;
		bool m_bListStimulus;
		unsigned int m_iCurrentStimulusIndex;
		bool m_bPlaying;
		const Habit::HStimulusSettingsList m_stimlist;
		//HPreviewMediaManager *m_pmm;
		HGMM *m_pmm;
		QList<unsigned int> m_idList;	// list of indices into media manager, m_pmm->play(m_idList[m_idListCurrent])
		unsigned int m_idListCurrent;
		QPushButton *m_pbDown;
		QPushButton *m_pbUp;
		QLabel *m_labelStimName;
		QPushButton *m_pbNext;
		QPushButton *m_pbPrev;
		QPushButton *m_pbStop;
		QPushButton *m_pbPlay;
		QPushButton *m_pbRewind;

		void updateNavigation();
		void updateNavigation(QString stimName);

	public:
		HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, QWidget *parent = NULL);
		~HStimulusPreviewWidget();

		// preview a single stimulus. Will add stimulus to media manager and optionally start it.
		void preview(const Habit::StimulusSettings& stimulus, bool bPlay = true);
		void preview(const Habit::HStimulusSettingsList& stimuli, QList< QPair<int, QString> > list);
		void clear();
		void rewind();
		void setStimulusLayoutType(const HStimulusLayoutType& type);

	protected:
		void hideEvent(QHideEvent *event);
		void showEvent(QShowEvent *event);
	protected slots:
		void nextClicked();
		void prevClicked();
		void stopClicked();
		void playClicked();
		void rewindClicked();
	};

};

#endif /* HSTIMULUSPREVIEWWIDGET_H_ */
