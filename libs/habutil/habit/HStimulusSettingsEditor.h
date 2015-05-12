/*
 * HStimulusSettingsEditor.h
 *
 *  Created on: Nov 5, 2014
 *      Author: Oakeslab
 */

#ifndef HSTIMULUSSETTINGSEDITOR_H_
#define HSTIMULUSSETTINGSEDITOR_H_



#include "HStimulusSettingsWidget.h"
#include "stimulussettings.h"
#include <QDialog>
#include <QPushButton>
#include <QStringList>

namespace GUILib
{
	class HStimulusSettingsEditor: public QDialog
	{
		Q_OBJECT
	private:
		void create();
		void connections();

		const Habit::StimulusSettings& m_originalSettings;
		const QStringList& m_names;
		HStimulusSettingsWidget* m_pStimulusSettingsWidget;
		QWidget* m_pPreviewWidget;	// placeholder for now
		QPushButton *m_pbDone;
		QPushButton *m_pbCancel;
	public:
		HStimulusSettingsEditor(const Habit::StimulusSettings& settings, const HStimulusLayoutType& stimLayout, const QStringList& names, const QString& title = QString("Edit Stimulus Settings"), QWidget *parent=NULL);
		virtual ~HStimulusSettingsEditor() {};
		Habit::StimulusSettings getStimulusSettings();
	protected slots:
		void doneClicked();
	};
}


#endif /* HSTIMULUSSETTINGSEDITOR_H_ */
