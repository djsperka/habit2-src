
#ifndef HABIT_OUTPUT_GENERATOR_H
#define HABIT_OUTPUT_GENERATOR_H

#include "stimulussettings.h"
#include "experimentsettings.h"
#include "subjectsettings.h"
#include "runsettings.h"
#include "resultviewersettings.h"
#include "reliabilitysettings.h"

namespace Habit {

class TrialLogItem {
	
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
	TrialLogItem(int type, int durat, StimulusSettings settings) { lookType = type; duration = durat; stimulusSettings = settings;};
	TrialLogItem() {} ;
	int lookType;
	int duration;
	StimulusSettings stimulusSettings;
};

QDataStream & operator << (QDataStream& stream, TrialLogItem item);
QDataStream & operator >> (QDataStream& stream, TrialLogItem& item);


class TrialInfo {
public:
	TrialInfo()
	{}

	~TrialInfo() {}

	void reset() {
		logItems.clear();
	}
	void addLogItem(int type, int duration, StimulusSettings settings);

	QList<TrialLogItem> logItems;
private:
};

QDataStream & operator << (QDataStream& stream, TrialInfo item);
QDataStream & operator >> (QDataStream& stream, TrialInfo& item);

class OutputGenerator
{
public:

	OutputGenerator();
	~OutputGenerator();

	void setExperiment(int id);
	void setSubjectInformation(const SubjectSettings& si);
	void setRunSettings(const RunSettings& rs);
	void setCurrentTrial(int st);
	void setStimulusSettings(const StimulusSettings& ss);
	void setResultViewerSettings(const ResultViewerSettings& rvs);
	void changeTrial(int st);
	
	void addLogItem(int type, int duration);
	void summarize();

	TrialInfo getHabituationTrial() const {return habituationInfo_;}
	TrialInfo getPretestTrial() const {return pretestInfo_;}
	TrialInfo getTestTrial() const {return testInfo_;}
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

	SubjectSettings getSubjectSettings() { return subjectInformation_; }
	RunSettings getRunSettings() { return  runSettings_; }

	void setReliabilitiesSettings(const Habit::ReliabilitySettings & reliability) { reliabilitySettings_ = reliability; }
	Habit::ReliabilitySettings getReliabilitiesSettings() { return reliabilitySettings_; }

private:
	QStringList getTrialSummary(const QString& phaseName);
	QStringList getPhaseSummary(const QString& phaseName);
	QStringList getPhaseLog(const QString& phaseName);
	QStringList getPhaseLookSummary(const QString& phaseName);

private:
	ExperimentSettings experimentSettings_;
	SubjectSettings subjectInformation_;
	RunSettings runSettings_;
	StimulusSettings stimulusSettings_;
	
	StimulusSettings currentStimulusSettings_;
	ResultViewerSettings settings_;
	int currentTrial_;
	TrialInfo habituationInfo_;
	TrialInfo pretestInfo_;
	TrialInfo testInfo_;
	TrialInfo currentTrialInfo_;

	ReliabilitySettings reliabilitySettings_;

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

} // namespace Habit

#endif // HABIT_OUTPUT_GENERATOR_H