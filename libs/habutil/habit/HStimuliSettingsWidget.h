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
		int m_context;
		const Habit::StimulusDisplayInfo& m_sdi;
		HStimulusSettingsListWidget *m_pStimulusSettingsListWidget;
		HStimulusOrderListWidget *m_pStimulusOrderListWidget;
		HStimulusPreviewWidget *m_pStimulusPreviewWidget;
		QPushButton *m_pbImport;
		void create(const QString& labelName, const Habit::StimulusDisplayInfo& info);
		void connections();
		void populate();
	public:
		HStimuliSettingsWidget(const QString& labelName, const Habit::StimuliSettings& stimuli, int context, const Habit::StimulusDisplayInfo& sdi, QWidget *parent=0);
		virtual ~HStimuliSettingsWidget() {};

		// Get stimulisettings as currently defined in the widget (including changes)
		Habit::StimuliSettings getStimuliSettings();
	protected slots:
		void importClicked();
		void previewStimulus(int);
		void previewOrder(int);
		void clearStimulus();
		void currentStimulusSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
		void currentOrderSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
		void stimulusSelectionChanged();
		void orderSelectionChanged();
		void stimulusAdded(int row);
		void stimulusAboutToBeRemoved(int row);
		void previewStopButtonHit();
		void stimulusSettingsChanged(int rowid);
	};
}



#endif /* HSTIMULISETTINGSWIDGET_H_ */
