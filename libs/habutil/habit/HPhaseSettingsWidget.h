/*
 * HPhaseSettingsWidget.h
 *
 *  Created on: Feb 28, 2014
 *      Author: Oakeslab
 */

#ifndef HPHASESETTINGSWIDGET_H_
#define HPHASESETTINGSWIDGET_H_

#include <QtGui>
#include "HPhaseSettings.h"

namespace GUILib
{
	class HPhaseSettingsWidget: public QWidget
	{
		Q_OBJECT

	public:
		HPhaseSettingsWidget();
		HPhaseSettingsWidget(const Habit::HPhaseSettings& settings);
		const Habit::HPhaseSettings& getHPhaseSettings();
		void setHPhaseSettings(const Habit::HPhaseSettings& settings) { m_settings = settings; initialize(); };

	protected slots:

		void cbEnabledToggled(bool checked);
		void cbLookingCriteriaToggled(bool checked);
		void cbMaxStimulusTimeToggled(bool checked);
		void cbMaxNoLookTimeToggled(bool checked);
		void cbMaxLookAwayTimeToggled(bool checked);


	private:
		// Set values in widget to match those in m_settings
		void initialize();

		// Create gui components
		void createComponents();

		// Set up validators for gui components
		void setValidators();

		// Establish connections for gui components
		void makeConnections();

		// Create layouts
		void doLayout();

		Habit::HPhaseSettings m_settings;

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
	};
}; // namespace

#endif /* HPHASESETTINGSWIDGET_H_ */
