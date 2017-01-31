/*
 * HStimulusSettingsWidget.h
 *
 *  Created on: Oct 9, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSSETTINGSWIDGET_H_
#define HSTIMULUSSETTINGSWIDGET_H_

#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include "HTypes.h"
#include "stimulussettings.h"
#include "stimulusdisplayinfo.h"
#include "HStimulusInfoWidget.h"

namespace GUILib
{

// Note: The layout type and USEiss at creation are used. NO changing, hassle.
	class HStimulusSettingsWidget: public QWidget
	{
		Q_OBJECT
	private:
		Habit::StimulusSettings m_settings;
		const Habit::StimulusDisplayInfo& m_sdi;
//		bool m_bUseSoundPlayer;
//		const HStimulusLayoutType* m_pStimulusLayout;
		GUILib::HStimulusInfoWidget *m_pLeft;
		GUILib::HStimulusInfoWidget *m_pCenter;
		GUILib::HStimulusInfoWidget *m_pRight;
		GUILib::HStimulusInfoWidget *m_pSound;
		QStackedWidget *m_pStack;
		QLineEdit *m_pName;		// make this editable?
		int m_stackidSingle;
		int m_stackidLR;
		QWidget *m_pwSingle;
		QWidget *m_pwLR;

		void initialize();
		void connections();

	public:
		HStimulusSettingsWidget(const Habit::StimulusSettings& settings, const Habit::StimulusDisplayInfo& info, QWidget *parent=0);
		virtual ~HStimulusSettingsWidget();

		// StimulusLayoutType denotes single screen, LR screens, sound/nosound.
		//void setStimulusLayoutType(const HStimulusLayoutType& type);

		// Get the settings as currently defined in the widget (i.e. incorporating user changes)
		Habit::StimulusSettings getStimulusSettings();

		// just get the name - for validation by the editor
		QString getName();

	signals:
		void stimulusSettingsChanged();
	};
};

#endif /* HSTIMULUSSETTINGSWIDGET_H_ */
