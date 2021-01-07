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

	// this class is a single widget with the appropriate widgets for a given StimulusDisplayInfo
	// setting.
	class HStimulusPreviewWidget: public QWidget
	{
		Q_OBJECT

		const Habit::StimulusDisplayInfo& m_sdinfo;
		QVector<HStimulusWidget *> m_vecWidgets;

		// Widget (may have one or two sub-widgets depending on sdi) currently in layout
		// Replaced in slot(sdiChanged())
		QWidget *m_pCurrentStimulusWidget;

		// These two bools tell us whether a single stim preview, or a order preview, is current
		bool m_bSingleStimulus;
		bool m_bListStimulus;
		unsigned int m_currentStimKey;	// if single stim, this is the current key
		bool m_bPlaying;					// applies to both single and list
		QList<unsigned int> m_idList;	// list of indices into media manager, m_pmm->play(m_idList[m_idListCurrent])
		int m_idListCurrent;	// current index in the list m_idListCurrent[m_idist] is the current key
		QPushButton *m_pbDown;
		QPushButton *m_pbUp;
		QLabel *m_labelStimName;
		QPushButton *m_pbNext;
		QPushButton *m_pbPrev;
		QPushButton *m_pbPause;
		QPushButton *m_pbPlay;
		QPushButton *m_pbStop;
		QPushButton *m_pbRewind;

		void updateNavigation();
		void updateNavigation(QString stimName);
		QWidget* createStimulusWidget(const Habit::StimulusDisplayInfo& info);
		void initialize(const Habit::StimulusDisplayInfo& info);
	public:
		HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, QWidget *parent = NULL);
		HStimulusPreviewWidget(const Habit::StimulusDisplayInfo& info, unsigned int key, QWidget *parent = NULL);
		~HStimulusPreviewWidget();

		// preview a single stimulus with given key
		void preview(unsigned int key, bool bPlayNow = true);

		// preview the order list consisting of the given keys (and stimulus names)
		void preview(const QList<unsigned int>& list, bool bPlayNow = true);

		void clear();
		void setStimulusLayoutType(const HStimulusLayoutType& type);

		QVector<HStimulusWidget *> getStimulusWidgets();


	protected:
		// detach the widgets here, state is saved as single/list, current key, bPlaying
		void hideEvent(QHideEvent *event);

		// assign widgets to HGMM, restore state
		void showEvent(QShowEvent *event);

	Q_SIGNALS:
		void stopped();	// after stop clicked
		void preroll(unsigned int);
		void pause(unsigned int);
		void stim(unsigned int);
		void rewind(unsigned int);
		void background();

	public Q_SLOTS:
		void nextClicked();
		void prevClicked();
		void stopClicked();
		void pauseClicked();
		void playClicked();
		void rewindClicked();
		//void stimulusDisplayInfoChanged();
	};

};

#endif /* HSTIMULUSPREVIEWWIDGET_H_ */
