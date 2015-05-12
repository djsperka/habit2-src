/*
 * HRunSettingsDialog.h
 *
 *  Created on: Dec 1, 2014
 *      Author: Oakeslab
 */

#ifndef HRUNSETTINGSDIALOG_H_
#define HRUNSETTINGSDIALOG_H_

#include <QDialog>
#include <QPushButton>
#include <QMap>
#include "experimentsettings.h"
#include "HStimulusOrderSelectionWidget.h"
#include "runsettings.h"
#include "subjectsettings.h"
#include "HSubjectSettingsWidget.h"

namespace GUILib
{
	class HRunSettingsDialog: public QDialog
	{
		Q_OBJECT

		const Habit::ExperimentSettings& m_exptSettings;
		QMap<int, GUILib::HStimulusOrderSelectionWidget*> m_map;
		GUILib::HSubjectSettingsWidget* m_pSubjectSettingsWidget;
		QPushButton *m_pbRun;
		QPushButton *m_pbCancel;
		bool m_bPreTestOrderChosen;
		bool m_bHabituationOrderChosen;
		bool m_bTestOrderChosen;

		void components(bool bTestRun);
		void connections();
		void updateRunButton();

	public:
		HRunSettingsDialog(const Habit::ExperimentSettings& s, bool bTestRun=false, QWidget* parent=NULL);
		virtual ~HRunSettingsDialog() {};
		Habit::RunSettings getRunSettings() const;
		Habit::SubjectSettings getSubjectSettings() const;
		QString getRunLabel() const;

	private slots:
		void preTestOrderChosen();
		void habituationOrderChosen();
		void testOrderChosen();
	};
}



#endif /* HRUNSETTINGSDIALOG_H_ */
