/*
 *  HControlPanel.h
 *  habutil
 *
 *  Created by Oakes Lab on 9/11/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HCONTROLPANEL_H
#define HCONTROLPANEL_H

#include <QtGui>
#include <QMap>

#include "runsettings.h"
#include "stimulussettings.h"
#include "stimulisettings.h"
#include "experimentsettings.h"

#include "HMediaManager.h"
#include "HLookDetector.h"
#include "HStateMachine.h"
#include "HExperiment.h"
#include "HEventLog.h"
#include "HMediaStatusWidget.h"
#include "HExperimentStatusWidget.h"

namespace GUILib
{

	class HControlPanel : public QDialog
	{
		Q_OBJECT

	public:
		HControlPanel(const Habit::ExperimentSettings& settings, HEventLog& log, const Habit::RunSettings& runSettings, QWidget* w = 0);
		~HControlPanel();

		const Habit::ExperimentSettings& getExperimentSettings() { return m_experimentSettings; };
	
	private:
		void components();
		void connections();
		void doLayout();
		void createExperiment(HEventLog& log);

	private slots:
		void onStartTrials();
		void onNextTrial();
		void onStopTrials();
		void onAttention();
		void onLook(HLook l);
		void onExperimentFinished();
		void onExperimentStarted();
		void onPhaseStarted(QString phaseName);
		void onTrialStarted(int trialindex, int repeatindex);

	protected:
		void closeEvent (QCloseEvent * e);
		void keyPressEvent(QKeyEvent *e);
	
	private:
		const Habit::ExperimentSettings& m_experimentSettings;
		HEventLog& m_log;
		Habit::RunSettings m_runSettings;
		HMediaManager* m_pmm;
		//HLookDetector* m_pld;
		QStateMachine* m_psm;
	
		QPushButton* m_pbStartTrials;
		QPushButton* m_pbNextTrial;
		QPushButton* m_pbStopTrials;

		QLabel* m_labelCurrentPhase;
		QLabel* m_labelCurrentPhaseValue;

		QLabel* m_labelCurrentStimulus;
		QLabel* m_labelLeftMonitorFile;
		QLabel* m_labelCenterMonitorFile;
		QLabel* m_labelRightMonitorFile;
		QLabel* m_labelSoundFile;

		QLabel* m_labelCurrentStimulusValue;
		QLabel* m_labelLeftMonitorFileValue;
		QLabel* m_labelCenterMonitorFileValue;
		QLabel* m_labelRightMonitorFileValue;
		QLabel* m_labelSoundFileValue;
	
		HMediaStatusWidget* m_pMediaStatusWidget;
		HExperimentStatusWidget* m_pExperimentStatusWidget;
	};
}; // namespace GUILib
#endif // HCONTROLPANEL_H
