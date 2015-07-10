/*
 * HStimuliSettingsWidget.h
 *
 *  Created on: Oct 21, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULISETTINGSWIDGET_H_
#define HSTIMULISETTINGSWIDGET_H_

#include <QWidget>
#include <QColor>
#include "HTypes.h"
#include "stimulisettings.h"
#include "HStimulusSettingsListWidget.h"
#include "HStimulusOrderListWidget.h"
#include "HStimulusPreviewWidget.h"
#include "stimulusdisplayinfo.h"

namespace GUILib
{
	class HStimuliSettingsWidget: public QWidget
	{
		Q_OBJECT
	private:
		Habit::StimuliSettings m_stimuli;
		const Habit::StimulusDisplayInfo& m_stimulusDisplayInfo;
		HStimulusSettingsListWidget *m_pStimulusSettingsListWidget;
		HStimulusOrderListWidget *m_pStimulusOrderListWidget;
		HStimulusPreviewWidget *m_pStimulusPreviewWidget;
		QPushButton *m_pbImport;

		void create();
		void connections();
	public:
		HStimuliSettingsWidget(const Habit::StimuliSettings& stimuli, const Habit::StimulusDisplayInfo& stimulusDisplayInfo, QWidget *parent=0);
		virtual ~HStimuliSettingsWidget() {};

		// Get stimulisettings as currently defined in the widget (including changes)
		Habit::StimuliSettings getStimuliSettings();
	protected slots:
		void stimulusLayoutTypeChanged(int);
		void importClicked();
		void previewStimulus(int);
		void clearStimulus();
	};
}



#endif /* HSTIMULISETTINGSWIDGET_H_ */
