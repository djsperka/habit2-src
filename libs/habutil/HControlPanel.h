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

#include "runsettings.h"
#include "stimulussettings.h"
#include "stimulisettings.h"
#include "experimentsettings.h"
#include "subjectsettings.h"
#include "OutputGenerator.h"

#include "HMediaManager.h"
#include "HLookDetector.h"
#include "HStateMachine.h"
#include "HPhase.h"

#include <QDialog>
#include <QMap>

class QPushButton;
class QLabel;
class QVBoxLayout;
class QString;

class HControlPanel : public QDialog
{
	Q_OBJECT
	
public:
	HControlPanel(const Habit::SubjectSettings& ss, const Habit::RunSettings& runSettings, QWidget* w = 0);
	~HControlPanel() {};
	
private:
	void createComponents();
	void makeConnections();
	void doLayout();
	void createExperiment();
	void loadFromDB();
	void populateSSMap(const Habit::AttentionGetterSettings& ags, const Habit::StimulusSettingsList& l1, const Habit::StimulusSettingsList& l2, const Habit::StimulusSettingsList& l3);
	void updateFileStatusLabels(Habit::StimulusSettings& ss);
	
private slots:
	void onStartTrials();
	void onNextTrial();
	void onStopTrials();
	void onLook(HLook l);
	void onExperimentFinished();
	void onAGStarted();
	void onStimStarted(int i);
	void onCleared();
	
protected:
	void closeEvent (QCloseEvent * e); 
	//bool eventFilter(QObject *target, QEvent *event);
	
private:
	Habit::RunSettings m_runSettings;
	Habit::SubjectSettings m_subjectSettings;
	Habit::ExperimentSettings m_experimentSettings;
	QMap<int, Habit::StimulusSettings> m_mapSS;
	HMediaManager* m_pmm;
	HLookDetector* m_pld;
	QStateMachine* m_psm;
	HPhase* m_psPreTest;
	HPhase* m_psHabituation;
	HPhase* m_psTest;
	Habit::StimulusSettingsList m_pStimulusSettingsList;
	
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
