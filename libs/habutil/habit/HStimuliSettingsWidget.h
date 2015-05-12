/*
 * HStimuliSettingsWidget.h
 *
 *  Created on: Oct 21, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULISETTINGSWIDGET_H_
#define HSTIMULISETTINGSWIDGET_H_

#include <QWidget>
#include <QScrollArea>
#include "HTypes.h"
#include "stimulisettings.h"
#include "HStimulusSettingsListWidget.h"
#include "HStimulusOrderListWidget.h"

namespace GUILib
{
	class HStimuliSettingsWidget: public QWidget
	{
		Q_OBJECT
	private:
		Habit::StimuliSettings m_stimuli;
		const HStimulusLayoutType* m_pStimulusLayout;
		HStimulusSettingsListWidget *m_pStimulusSettingsListWidget;
		HStimulusOrderListWidget *m_pStimulusOrderListWidget;
		QPushButton *m_pbImport;

		void create();
		void connections();
	public:
		HStimuliSettingsWidget(const Habit::StimuliSettings& stimuli, const HStimulusLayoutType& stimLayout = HStimulusLayoutType::HStimulusLayoutSingle, QWidget *parent=0);
		virtual ~HStimuliSettingsWidget() {};
		void setStimulusLayoutType(const HStimulusLayoutType& type);

		// Get stimulisettings as currently defined in the widget (including changes)
		Habit::StimuliSettings getStimuliSettings();
	protected slots:
		void stimulusLayoutTypeChanged(int);
		void importClicked();
	};
}



#if 0

namespace GUILib
{
	class HStimuliSettingsWidget: public QScrollArea
	{
		Q_OBJECT
	private:
		Habit::StimuliSettings m_stimuli;
		const HStimulusLayoutType* m_pStimulusLayout;
		QWidget *m_list;

		void initialize();

	public:
		HStimuliSettingsWidget(const Habit::StimuliSettings& stimuli, const HStimulusLayoutType& stimLayout = HStimulusLayoutType::HStimulusLayoutSingle, QWidget *parent=0);
		virtual ~HStimuliSettingsWidget() {};

		void setStimulusLayoutType(const HStimulusLayoutType& type) { Q_UNUSED(type);};
	};
};

#endif



#endif /* HSTIMULISETTINGSWIDGET_H_ */
