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

#include <QtGui/QDialog>

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
	
	Habit::OutputGenerator getOutputGenerator() { return m_outputGenerator; }
private:
	void createComponents();
	void makeConnections();
	void doLayout();
	void createExperiment();
	void loadFromDB();
	
//		void determineControlPanelSettings();
//		void fillStimulus(const QString& order, const Habit::StimuliSettings& ss, int type);
//		void checkControlBarOptions(const Habit::ControlBarOptions& cbo);
	
private slots:
	void onStartTrials();
	void onNextTrial();
	void onStopTrials();
	
protected:
	void closeEvent (QCloseEvent * e); 
	//bool eventFilter(QObject *target, QEvent *event);
	
private:
	Habit::RunSettings m_runSettings;
	Habit::SubjectSettings m_subjectSettings;
	Habit::ExperimentSettings m_experimentSettings;
	HMediaManager* m_pmm;
	HLookDetector* m_pld;
	QStateMachine* m_psm;
	HPhase* m_psPreTest;
	HPhase* m_psHabituation;
	HPhase* m_psTest;
	
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
	
	Habit::OutputGenerator m_outputGenerator;
};
	
#endif // HCONTROLPANEL_H
