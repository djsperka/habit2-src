/*
 * HCriterionSettingsWidget.h
 *
 *  Created on: Mar 18, 2014
 *      Author: Oakeslab
 */

#ifndef HCRITERIONSETTINGSWIDGET_H_
#define HCRITERIONSETTINGSWIDGET_H_

#include <QtGui>
#include "HCriterionSettings.h"

namespace GUILib
{
	class HCriterionSettingsWidget: public QWidget
	{
		Q_OBJECT

	public:
		HCriterionSettingsWidget();
		HCriterionSettingsWidget(const Habit::HCriterionSettings& settings);
		const Habit::HCriterionSettings& getHCriterionSettings();
		void setHCriterionSettings(const Habit::HCriterionSettings& settings) { m_settings = settings; initialize(); };

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

#endif /* HCRITERIONSETTINGSWIDGET_H_ */
