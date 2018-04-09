/*
 * HStimulusSettingsWidget.h
 *
 *  Created on: Oct 9, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSSETTINGSWIDGET_H_
#define HSTIMULUSSETTINGSWIDGET_H_

#include <QWidget>
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
		GUILib::HStimulusInfoWidget *m_pLeft;
		GUILib::HStimulusInfoWidget *m_pCenter;
		GUILib::HStimulusInfoWidget *m_pRight;
		GUILib::HStimulusInfoWidget *m_pSound;
		QLineEdit *m_pName;		// make this editable?

		// create components that are part of this class, do stuff to them that doesn't require sdi or ppt.
		void components();

		// get the editor widget that contains one or more StimulusInfoWidgets
		QWidget *getEditorWidget(const Habit::StimulusDisplayInfo& sdi);
		QWidget *getEditorWidget(const HPlayerPositionType& single);

		// make the layout for this editor
		QLayout *makeLayout(QWidget *pwidget);

	public:
		HStimulusSettingsWidget(const Habit::StimulusSettings& settings, const Habit::StimulusDisplayInfo& info, QWidget *parent=0);
		HStimulusSettingsWidget(const Habit::StimulusSettings& settings, const HPlayerPositionType& ppt, QWidget *parent=0);
		virtual ~HStimulusSettingsWidget();

		// Get the settings as currently defined in the widget (i.e. incorporating user changes)
		Habit::StimulusSettings getStimulusSettings();

		// just get the name - for validation by the editor
		QString getName();

	signals:
		void stimulusSettingsChanged();
	};
};

#endif /* HSTIMULUSSETTINGSWIDGET_H_ */
