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


class HControlPanel : public QDialog
{
	Q_OBJECT
	
public:
	HControlPanel(HEventLog& log, const Habit::RunSettings& runSettings, QWidget* w = 0);
	~HControlPanel() {};
	
	const Habit::ExperimentSettings& getExperimentSettings() { return m_experimentSettings; };

private:
	void createComponents();
	void makeConnections();
	void doLayout();
	void createExperiment(HEventLog& log);
	void loadFromDB();
	void updateFileStatusLabels(Habit::StimulusSettings& ss);
	
private slots:
	void onStartTrials();
	void onNextTrial();
	void onStopTrials();
	void onAttention();
	void onLook(HLook l);
	void onExperimentFinished();
	void onExperimentStarted();
	void onAGStarted();
	void onStimStarted(int i);
	void onCleared();
	
protected:
	void closeEvent (QCloseEvent * e); 
	
private:
	HEventLog& m_log;
	Habit::RunSettings m_runSettings;
	Habit::ExperimentSettings m_experimentSettings;
	HMediaManager* m_pmm;
	//HLookDetector* m_pld;
	QStateMachine* m_psm;

	QPushButton* m_pbStartTrials;
	QPushButton* m_pbNextTrial;
	QPushButton* m_pbStopTrials;
	
	QLabel* m_labelExperimentStatus;
	QLabel* m_labelLookStatus;
	QLabel* m_labelAttentionGetterStatus;
	
	QLabel* m_labelExperimentStatusValue;
	QLabel* m_labelLookStatusValue;
	QLabel* m_labelAttentionGetterStatusValue;
	
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
};
	
#endif // HCONTROLPANEL_H
