/*
 * HMediaStatusWidget.h
 *
 *  Created on: Dec 29, 2014
 *      Author: Oakeslab
 */

#ifndef HMEDIASTATUSWIDGET_H_
#define HMEDIASTATUSWIDGET_H_

#include "HTypes.h"
#include "HGMM.h"
#include <QWidget>

namespace Ui
{
	class HMediaStatusForm;
};


namespace GUILib
{
	class HMediaStatusWidget : public QWidget
	{
		Q_OBJECT

	public:
		HMediaStatusWidget(const HStimulusLayoutType& layout, bool bDisplayStimulusInfo=false, QWidget* parent = NULL);
		virtual ~HMediaStatusWidget();

		void setMediaManager(const HGMM* pmm);

	private:
		// hide this?
		HMediaStatusWidget();


		// establish connections
		void connections();

		// Make the widget reflect the contents of m_info
		void initialize();

		// display background color on status widget
		void setStatusBackground();

		// generic set status, called by setSTatusAG/Stim
		void setStatusLabels(const Habit::StimulusSettings& settings, const QString& stimOrAG);

		// display AG image on status widget
		void setStatusAG(const Habit::StimulusSettings& settings);

		// display stim image on status widget
		void setStatusStim(const Habit::StimulusSettings& settings);

		const HGMM* m_pMediaManager;
		const HStimulusLayoutType& m_layout;
		bool m_bDisplayStimulusInfo;
		Ui::HMediaStatusForm *ui;

	protected slots:
		void onAGStarted(int key);
		void onStimStarted(int key);
	};
};




#endif /* HMEDIASTATUSWIDGET_H_ */
