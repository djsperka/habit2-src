/*
 *  HOutputGenerator.h
 *  habutil
 *
 *  Created by Oakes Lab on 10/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef HOUTPUT_GENERATOR_H
#define HOUTPUT_GENERATOR_H

#include "stimulussettings.h"
#include "experimentsettings.h"
#include "subjectsettings.h"
#include "runsettings.h"
#include "resultviewersettings.h"
#include "reliabilitysettings.h"
	
class HTrialLogItem {
	
public:
	
	static const int LEFT_LOOK;
	static const int CENTER_LOOK;
	static const int RIGHT_LOOK;
	static const int AWAY_LOOK;
	static const int NEW_TRIAL;
	static const int NO_LOOK;
	static const int REPEAT_TRIAL;
	static const int NEW_TRIAL_REPEAT;
	
public:
	HTrialLogItem(int type, int durat, Habit::StimulusSettings settings) { lookType = type; duration = durat; stimulusSettings = settings;};
	HTrialLogItem() {} ;
	int lookType;
	int duration;
	Habit::StimulusSettings stimulusSettings;
};

QDataStream & operator << (QDataStream& stream, HTrialLogItem item);
QDataStream & operator >> (QDataStream& stream, HTrialLogItem& item);


class HTrialInfo {
public:
	HTrialInfo()
	{}
	
	~HTrialInfo() {}
	
	void reset() {
		logItems.clear();
	}
	void addLogItem(int type, int duration, Habit::StimulusSettings settings);
	
	QList<HTrialLogItem> logItems;
private:
};

QDataStream & operator << (QDataStream& stream, HTrialInfo item);
QDataStream & operator >> (QDataStream& stream, HTrialInfo& item);
	


class HOutputGenerator
{
private:
	HOutputGenerator();
	~HOutputGenerator();
	
	static HOutputGenerator* f_pInstance;
public:
	
	// Use this to get the singleton instance. 
	static HOutputGenerator* instance()
	{
		if (!f_pInstance)
			f_pInstance = new HOutputGenerator();
		return f_pInstance;
	};
	
	// This deletes the current singleton and creates a new one. 
	static HOutputGenerator* initialize()
	{
		if (f_pInstance)
		{
			delete f_pInstance;
			f_pInstance = NULL;
		}
		return instance();
	};
	
	void setExperiment(int id);
	void setSubjectInformation(const Habit::SubjectSettings& si);
	void setRunSettings(const Habit::RunSettings& rs);
	void setCurrentTrial(int st);
	void setStimulusSettings(const Habit::StimulusSettings& ss);
	void setResultViewerSettings(const Habit::ResultViewerSettings& rvs);
	void changeTrial(int st);
	
	void addLogItem(int type, int duration);
	void summarize();
	
	HTrialInfo getHabituationTrial() const {return habituationInfo_;}
	HTrialInfo getPretestTrial() const {return pretestInfo_;}
	HTrialInfo getTestTrial() const {return testInfo_;}
	int getCurrentTrialType() const {return currentTrial_;}
	
	QStringList toHtml(bool headerIncluded = true);
	bool save(const QString& fileName);
	bool load(const QString& fileName);
	
	void setExperimentName(const QString& name);
	QString getExperimentName();
	void setSettingsFileName(const QString& name);
	QString getSettingsFileName();
	void setResultsFileName(const QString& name);
	QString getResultsFileName();
	void setResultsType(const QString& name);
	QString getResultsType();
	void setTrialType(const QString& name);
	QString getTrialType();
	void setHabituation(const QString& value);
	QString getHabituation();
	void setHabituationResult(const QString& value);
	QString getHabituationResult();
	bool isHabituated();
	
	Habit::SubjectSettings getSubjectSettings() { return subjectInformation_; }
	Habit::RunSettings getRunSettings() { return  runSettings_; }
	
	void setReliabilitiesSettings(const Habit::ReliabilitySettings & reliability) { reliabilitySettings_ = reliability; }
	Habit::ReliabilitySettings getReliabilitiesSettings() { return reliabilitySettings_; }
	
private:
	QStringList getTrialSummary(const QString& phaseName);
	QStringList getPhaseSummary(const QString& phaseName);
	QStringList getPhaseLog(const QString& phaseName);
	QStringList getPhaseLookSummary(const QString& phaseName);
	
private:
	Habit::ExperimentSettings experimentSettings_;
	Habit::SubjectSettings subjectInformation_;
	Habit::RunSettings runSettings_;
	Habit::StimulusSettings stimulusSettings_;
	
	Habit::StimulusSettings currentStimulusSettings_;
	Habit::ResultViewerSettings settings_;
	int currentTrial_;
	HTrialInfo habituationInfo_;
	HTrialInfo pretestInfo_;
	HTrialInfo testInfo_;
	HTrialInfo currentTrialInfo_;
	
	Habit::ReliabilitySettings reliabilitySettings_;
	
	int allLookCount ;
	int allLookAwayCount ;
	int leftLookCount ;
	int centerLookCount ;
	int rightLookCount ;
	double totalTime ;
	double leftTime ;
	double centerTime ;
	double rightTime ;
	
	int totalLookCountSum;
	int leftLookCountSum ;
	int centerLookCountSum ;
	int rightLookCountSum ;
	double totalTimeSum ;
	double leftTimeSum ;
	double centerTimeSum ;
	double rightTimeSum ;
	int trialcount ;
	
	QString experimentName_;
	QString settingsFileName_;
	QString resultsFileName_;
	QString resultsType_;
	QString trialType_;
	QString habituation_;
	QString habituationResult_;
};

#endif // HOUTPUT_GENERATOR_H