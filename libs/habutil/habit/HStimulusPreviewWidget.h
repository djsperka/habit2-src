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
#include "HStimulusWidget.h"

namespace GUILib
{

	class HStimulusPreviewWidget: public QWidget
	{
		Q_OBJECT
		bool m_bSingleStimulus;
		bool m_bListStimulus;
		unsigned int m_currentStimKey;
		bool m_bPlaying;
		HStimulusWidget *m_w0, *m_w1;
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
		QWidget* createStimulusWidget(const Habit::StimulusDisplayInfo& info);
	public:
		HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, QWidget *parent = NULL);
		~HStimulusPreviewWidget();

		// preview a single stimulus with given key
		void preview(unsigned int key, bool bPlayNow = true);

		// preview the order list consisting of the given keys (and stimulus names)
		void preview(QList< QPair<unsigned int, QString> > list, bool bPlayNow = true);

		void clear();
		void rewind();
		void setStimulusLayoutType(const HStimulusLayoutType& type);

	protected:
		// stop hgmm, detach widgets.
		void hideEvent(QHideEvent *event);

		// assign widgets to HGMM, play default stim
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
