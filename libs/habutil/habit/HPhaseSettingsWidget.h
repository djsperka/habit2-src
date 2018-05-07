/*
 * HPhaseSettingsWidget.h
 *
 *  Created on: Feb 28, 2014
 *      Author: Oakeslab
 */

#ifndef HPHASESETTINGSWIDGET_H_
#define HPHASESETTINGSWIDGET_H_

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "HPhaseSettings.h"

namespace Ui
{
	class HPhaseSettingsForm;
}

namespace GUILib
{
	class HPhaseSettingsWidget: public QWidget
	{
		Q_OBJECT

	public:
		HPhaseSettingsWidget(QWidget *parent = 0);
		HPhaseSettingsWidget(const Habit::HPhaseSettings& settings, QWidget *parent=0);
		const Habit::HPhaseSettings& getHPhaseSettings();
		void setHPhaseSettings(const Habit::HPhaseSettings& settings) { m_settings = settings; initialize(); };

#if 0
	protected slots:

		void cbEnabledToggled(bool checked);
		void cbLookingCriteriaToggled(bool checked);
		void cbMaxStimulusTimeToggled(bool checked);
		void cbMaxNoLookTimeToggled(bool checked);
		void cbMaxLookAwayTimeToggled(bool checked);
#endif

	private:
		// Set values in widget to match those in m_settings
		void initialize();

		// Set up validators for gui components
		void setValidators();

#if 0
		// Create gui components
		void createComponents();

		// Establish connections for gui components
		void makeConnections();

		// Create layouts
		void doLayout();
#endif

		Ui::HPhaseSettingsForm *ui;
		Habit::HPhaseSettings m_settings;

#if 0
		QGroupBox* m_pgb;	// main group box
		QCheckBox* m_pcbEnabled;
		QSpinBox* m_pNTrials;

		QGroupBox* m_pgbEndTrialCriteria;

		QCheckBox *m_pcbUseLookingCriteria;
		QRadioButton *m_prbSingleCompleteLook;
		QRadioButton *m_prbAccumulatedLooking;
		QButtonGroup *m_pbgLookingCriteria;
		QLineEdit* m_peditAccumulatedLookTime;

		QCheckBox *m_pcbMaxLookAwayTime;
		QLineEdit* m_peditMaxLookAwayTime;

		QCheckBox *m_pcbMaxStimulusTime;
		QLineEdit* m_peditMaxStimulusTime;
		QRadioButton *m_prbMeasureStimulusTimeFromOnset;
		QRadioButton *m_prbMeasureStimulusTimeFromLooking;
		QButtonGroup *m_pbgMaxStimulusTime;

		QCheckBox *m_pcbMaxNoLookTime;
		QLineEdit* m_peditMaxNoLookTime;
#endif

	};
}; // namespace

#endif /* HPHASESETTINGSWIDGET_H_ */
