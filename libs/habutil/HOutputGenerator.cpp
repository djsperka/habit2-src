/*
 *  HOutputGenerator.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 10/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HOutputGenerator.h"
#include "maindao.h"

#include <QtCore/QFileInfo>

using namespace Habit;

const int HTrialLogItem::LEFT_LOOK = 0;
const int HTrialLogItem::CENTER_LOOK = 1;
const int HTrialLogItem::RIGHT_LOOK = 2;
const int HTrialLogItem::AWAY_LOOK = 3;
const int HTrialLogItem::NEW_TRIAL = 4;
const int HTrialLogItem::NO_LOOK = 5;
const int HTrialLogItem::REPEAT_TRIAL = 6;
const int HTrialLogItem::NEW_TRIAL_REPEAT = 7;

// Singleton instance of HOutputGenerator
HOutputGenerator* HOutputGenerator::f_pInstance = NULL;


void HTrialInfo::addLogItem(int type, int duration, Habit::StimulusSettings settings)
{
	HTrialLogItem item(type, duration, settings);
	logItems.append(item);
}

QDataStream & operator << (QDataStream& stream, HTrialLogItem item)
{
	stream << item.lookType;
	stream << item.duration;
	stream << item.stimulusSettings;
	return stream;
}

QDataStream & operator >> (QDataStream& stream, HTrialLogItem& item)
{
	stream >> item.lookType;
	stream >> item.duration;
	stream >> item.stimulusSettings;
	return stream;
}
	
QDataStream & operator << (QDataStream& stream, HTrialInfo item)
{
	stream << item.logItems;
	return stream;
}

QDataStream & operator >> (QDataStream& stream, HTrialInfo& item)
{
	stream >> item.logItems;
	return stream;
}
	
	
HOutputGenerator::HOutputGenerator() 
: experimentSettings_()
, subjectInformation_()
, runSettings_()
, habituationInfo_()
, pretestInfo_()
, testInfo_()
, currentTrialInfo_()
{
};
	
HOutputGenerator::~HOutputGenerator() 
{
}
	
void HOutputGenerator::setExperiment(int id) 
{
	experimentSettings_.setId(id);
	experimentSettings_.loadFromDB(true);
	trialType_ = (experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getType() == Habit::TrialsInfo::eFixedLength)?"Fixed Length":"Subject Controlled";
	experimentName_ = experimentSettings_.getName();
	habituation_ = (experimentSettings_.getHabituationSettings().getHabituationType() == HabituationSettings::eFixedNumberOfTrials)?"Number of Trials":
	((experimentSettings_.getHabituationSettings().getHabituationType() == HabituationSettings::eUseCriterionToEnd)?"Criterion to End":"Total Look Length");
}

void HOutputGenerator::setSubjectInformation(const SubjectSettings& si) 
{
	subjectInformation_ = si;
}

void HOutputGenerator::setRunSettings(const RunSettings& rs) 
{
	runSettings_ = rs;
}

void HOutputGenerator::setCurrentTrial(int st) 
{
	currentTrial_ = st;
}

void HOutputGenerator::setStimulusSettings(const StimulusSettings& ss) 
{
	stimulusSettings_ = ss;
}

void HOutputGenerator::setResultViewerSettings(const Habit::ResultViewerSettings &rvs)
{
	settings_ = rvs;
}
	
void HOutputGenerator::changeTrial(int st) 
{
	qDebug("HOutputGenerator::changeTrial: currentTrial_ %d st %d", currentTrial_, st);
	if(currentTrial_ != st) {
		switch(currentTrial_) {
			case StimulusSettings::HABITUATION :
				habituationInfo_ = currentTrialInfo_;
				break;
			case StimulusSettings::PRETEST :
				pretestInfo_ = currentTrialInfo_;
				break;
			case StimulusSettings::TEST :
				testInfo_ = currentTrialInfo_;
				break;
		}
		currentTrial_ = st;
		currentTrialInfo_.reset();
	}
}

void HOutputGenerator::summarize() 
{
	switch(currentTrial_) {
		case StimulusSettings::HABITUATION :
			habituationInfo_ = currentTrialInfo_;
			break;
		case StimulusSettings::PRETEST :
			pretestInfo_ = currentTrialInfo_;
			break;
		case StimulusSettings::TEST :
			testInfo_ = currentTrialInfo_;
			break;
	}
}
	
	
void HOutputGenerator::addLogItem(int type, int duration)
{
	qDebug("HOutputGenerator::addLogItem(type=%d, duration=%d)", type, duration);
	currentTrialInfo_.addLogItem(type, duration, stimulusSettings_);
}
	
QStringList HOutputGenerator::getTrialSummary(const QString& phaseName)
{
	QStringList log;
	
	log.append(QString("Summary of %1 Trial %2:").arg(phaseName).arg(trialcount));
	if (settings_.isStimulusNamesShown())
	{
		log.append(QString("  Stimulus Name: %1").arg(currentStimulusSettings_.getName()));
	}
	if (currentStimulusSettings_.isLeftEnabled() && settings_.isLeftMonitorFilenamesShown())
	{
		log.append(QString("    Left Monitor File: %1").arg(QFileInfo(currentStimulusSettings_.getLeftStimulusInfo().getFileName()).fileName()));
	}
	if (currentStimulusSettings_.isCenterEnabled() && settings_.isCenterMonitorFilenamesShown())
	{
		log.append(QString("    Center Monitor File: %1").arg(QFileInfo(currentStimulusSettings_.getCenterStimulusInfo().getFileName()).fileName()));
	}
	if (currentStimulusSettings_.isRightEnabled() && settings_.isRightMonitorFilenamesShown())
	{
		log.append(QString("    Right Monitor File: %1").arg(QFileInfo(currentStimulusSettings_.getRightStimulusInfo().getFileName()).fileName()));
	}
	if (currentStimulusSettings_.isIndependentSoundEnabled() && settings_.isIndependentFilenamesShown())
	{
		log.append(QString("    Independent Sound File: %1").arg(QFileInfo(currentStimulusSettings_.getIndependentSoundInfo().getFileName()).fileName()));
	}
	if (settings_.isNumberOfLeftLooksShown() && settings_.isLeftMonitorLookTimesShown())
		log.append(QString("  Number of Left Looks (Left Looking Time):    %1 (%2 sec)").arg(leftLookCount).arg(leftTime));
	else if (settings_.isNumberOfLeftLooksShown() && !settings_.isLeftMonitorLookTimesShown())
		log.append(QString("  Number of Left Looks:    %1").arg(leftLookCount));
	else if (!settings_.isNumberOfLeftLooksShown() && settings_.isLeftMonitorLookTimesShown())
		log.append(QString("  Left Looking Time:    %1 sec").arg(leftTime));
	if (settings_.isNumberOfCenterLooksShown() && settings_.isCenterMonitorLookTimesShown())
		log.append(QString("  Number of Center Looks (Center Looking Time):    %1 (%2 sec)").arg(centerLookCount).arg(centerTime));
	else if (settings_.isNumberOfCenterLooksShown() && !settings_.isCenterMonitorLookTimesShown())
		log.append(QString("  Number of Center Looks:    %1").arg(centerLookCount));
	else if (!settings_.isNumberOfCenterLooksShown() && settings_.isCenterMonitorLookTimesShown())
		log.append(QString("  Center Looking Time:    %1 sec").arg(centerTime));
	if (settings_.isNumberOfRightLooksShown() && settings_.isRightMonitorLookTimesShown())
		log.append(QString("  Number of Right Looks (Right Looking Time):    %1 (%2 sec)").arg(rightLookCount).arg(rightTime));
	else if (settings_.isNumberOfRightLooksShown() && !settings_.isRightMonitorLookTimesShown())
		log.append(QString("  Number of Right Looks:    %1").arg(rightLookCount));
	else if (!settings_.isNumberOfRightLooksShown() && settings_.isRightMonitorLookTimesShown())
		log.append(QString("  Right Looking Time:    %1 sec").arg(rightTime));
	if (settings_.isTotalNumberOfLooksShown() && settings_.isTotalLookTimesShown())
		log.append(QString("  Total Number of Looks (Total Looking Time):    %1 (%2 sec)").arg(allLookCount).arg(leftTime + centerTime + rightTime));
	else if (settings_.isTotalNumberOfLooksShown() && !settings_.isTotalLookTimesShown())
		log.append(QString("  Total Number of Looks:    %1").arg(allLookCount));
	else if (!settings_.isTotalNumberOfLooksShown() && settings_.isTotalLookTimesShown())
		log.append(QString("  Total Looking Time:    %1 sec").arg(leftTime + centerTime + rightTime));
	log.append("");
	return log;
}
	
QStringList HOutputGenerator::getPhaseSummary(const QString& phaseName)
{
	QStringList log;
	log.append(QString("Summary of %1 Phase:").arg(phaseName));
	if (settings_.isNumberOfLeftLooksShown() && settings_.isLeftMonitorLookTimesShown())
		log.append(QString("  Number of %1 Left Looks (%1 Left Looking Time):    %2 (%3 sec)").arg(phaseName).arg(leftLookCountSum).arg(leftTimeSum));
	else if (settings_.isNumberOfLeftLooksShown() && !settings_.isLeftMonitorLookTimesShown())
		log.append(QString("  Number of %1 Left Looks:    %2").arg(phaseName).arg(leftLookCountSum));
	else if (!settings_.isNumberOfLeftLooksShown() && settings_.isLeftMonitorLookTimesShown())
		log.append(QString("  %1 Left Looking Time:    %2 sec").arg(phaseName).arg(leftTimeSum));
	if (settings_.isNumberOfCenterLooksShown() && settings_.isCenterMonitorLookTimesShown())
		log.append(QString("  Number of %1 Center Looks (%1 Center Looking Time):    %2 (%3 sec)").arg(phaseName).arg(centerLookCountSum).arg(centerTimeSum));
	else if (settings_.isNumberOfCenterLooksShown() && !settings_.isCenterMonitorLookTimesShown())
		log.append(QString("  Number of %1 Center Looks:    %2").arg(phaseName).arg(centerLookCountSum));
	else if (!settings_.isNumberOfCenterLooksShown() && settings_.isCenterMonitorLookTimesShown())
		log.append(QString("  %1 Center Looking Time:    %2 sec").arg(phaseName).arg(centerTimeSum));
	if (settings_.isNumberOfRightLooksShown() && settings_.isRightMonitorLookTimesShown())
		log.append(QString("  Number of %1 Right Looks (%1 Right Looking Time):    %2 (%3 sec)").arg(phaseName).arg(rightLookCountSum).arg(rightTimeSum));
	else if (settings_.isNumberOfRightLooksShown() && !settings_.isRightMonitorLookTimesShown())
		log.append(QString("  Number of %1 Right Looks:    %2").arg(phaseName).arg(rightLookCountSum));
	else if (!settings_.isNumberOfRightLooksShown() && settings_.isRightMonitorLookTimesShown())
		log.append(QString("  %1 Right Looking Time:    %2 sec").arg(phaseName).arg(rightTimeSum));
	if (settings_.isTotalNumberOfLooksShown() && settings_.isTotalLookTimesShown())
		log.append(QString("  Total Number of %1 Looks (Total %1 Looking Time):    %2 (%3 sec)").arg(phaseName).arg(totalLookCountSum).arg(totalTimeSum));
	else if (settings_.isTotalNumberOfLooksShown() && !settings_.isTotalLookTimesShown())
		log.append(QString("  Total Number of %1 Looks:    %2").arg(phaseName).arg(totalLookCountSum));
	else if (!settings_.isTotalNumberOfLooksShown() && settings_.isTotalLookTimesShown())
		log.append(QString("  Total %1 Looking Time:    %2 sec").arg(phaseName).arg(totalTimeSum));
	log.append("");
	return log;
}
	
QStringList HOutputGenerator::getPhaseLog(const QString& phaseName)
{
	QStringList log;
	log.append(QString("==== %1 Phase ====").arg(phaseName));
	bool isShowTimeline = (settings_.isLookTimelineShown());
	allLookCount = 0;
	allLookAwayCount = 0;
	leftLookCount = 0;
	centerLookCount = 0;
	rightLookCount = 0;
	totalTime = 0;
	leftTime = 0;
	centerTime = 0;
	rightTime = 0;
	trialcount = 0;
	totalLookCountSum = 0;
	leftLookCountSum = 0;
	centerLookCountSum = 0;
	rightLookCountSum = 0;
	totalTimeSum = 0;
	leftTimeSum = 0;
	centerTimeSum = 0;
	rightTimeSum = 0;
	HTrialInfo info;
	if (phaseName == "Pretest")
		info = pretestInfo_;
	else if (phaseName == "Habituation")
		info = habituationInfo_;
	else if (phaseName == "Test")
		info = testInfo_;
	for (int i = 0; i < info.logItems.count(); i++)
	{
		//currentStimulusSettings_ = info.logItems[i].stimulusSettings;
		double duration = qRound(info.logItems[i].duration/100.0)/10.0;
		if (info.logItems[i].lookType == HTrialLogItem::REPEAT_TRIAL)
		{
			log.append(QString("Trial %1 was repeated because no looking was recorded").arg(trialcount));
		}
		if (info.logItems[i].lookType == HTrialLogItem::NEW_TRIAL)
		{
			
			if (trialcount > 0)
			{
				if (settings_.isExperimentSummaryShown())
				{
					QStringList lst = getTrialSummary(phaseName);
					log.append(lst);
				}
			}
			totalLookCountSum += allLookCount;
			//totalTimeSum += totalTime;
			leftLookCountSum += leftLookCount;
			leftTimeSum += leftTime;
			centerLookCountSum += centerLookCount;
			centerTimeSum += centerTime;
			rightLookCountSum += rightLookCount;
			rightTimeSum += rightTime;
			allLookCount = 0;
			allLookAwayCount = 0;
			leftLookCount = 0;
			centerLookCount = 0;
			rightLookCount = 0;
			totalTime = 0;
			leftTime = 0;
			centerTime = 0;
			rightTime = 0;
			trialcount++;
			if (isShowTimeline)
				log.append(QString("- %1 Trial %2 -").arg(phaseName).arg(trialcount));
			currentStimulusSettings_ = info.logItems[i].stimulusSettings;
			
		}
		if (info.logItems[i].lookType == HTrialLogItem::NEW_TRIAL_REPEAT)
		{
			
			if (trialcount > 0)
			{
				if (settings_.isExperimentSummaryShown())
				{
					QStringList lst = getTrialSummary(phaseName);
					log.append(lst);
				}
			}
			totalLookCountSum += allLookCount;
			//totalTimeSum += totalTime;
			leftLookCountSum += leftLookCount;
			leftTimeSum += leftTime;
			centerLookCountSum += centerLookCount;
			centerTimeSum += centerTime;
			rightLookCountSum += rightLookCount;
			rightTimeSum += rightTime;
			allLookCount = 0;
			allLookAwayCount = 0;
			leftLookCount = 0;
			centerLookCount = 0;
			rightLookCount = 0;
			totalTime = 0;
			leftTime = 0;
			centerTime = 0;
			rightTime = 0;
			//trialcount++;
			if (isShowTimeline)
				log.append(QString("- %1 Trial %2 -").arg(phaseName).arg(trialcount));
			currentStimulusSettings_ = info.logItems[i].stimulusSettings;
			
		}
		
		
		
		totalTime += duration;
		if (info.logItems[i].lookType == HTrialLogItem::LEFT_LOOK)
		{
			leftTime += duration;
			leftLookCount++;
			allLookCount++;
			if (isShowTimeline)
				log.append(QString("Look %1 (Left) - %2 sec").arg(allLookCount).arg(duration));
		}
		if (info.logItems[i].lookType == HTrialLogItem::CENTER_LOOK)
		{
			centerTime += duration;
			centerLookCount++;
			allLookCount++;
			if (isShowTimeline)
				log.append(QString("Look %1 (Center) - %2 sec").arg(allLookCount).arg(duration));
		}
		if (info.logItems[i].lookType == HTrialLogItem::RIGHT_LOOK)
		{
			rightTime += duration;
			rightLookCount++;
			allLookCount++;
			if (isShowTimeline)
				log.append(QString("Look %1 (Right) - %2 sec").arg(allLookCount).arg(duration));
		}
		if (info.logItems[i].lookType == HTrialLogItem::AWAY_LOOK)
		{
			//centerTime += duration;
			allLookAwayCount++;
			if (isShowTimeline)
				log.append(QString("Look Away %1 - %2 sec").arg(allLookAwayCount).arg(duration));
		}
		if (info.logItems[i].lookType == HTrialLogItem::NO_LOOK)
		{
			if (isShowTimeline)
				log.append(QString("No Look - %1 sec").arg(duration));
		}
		
	}
	if (trialcount > 0)
	{
		if (settings_.isExperimentSummaryShown())
		{
			QStringList lst = getTrialSummary(phaseName);
			log.append(lst);
			totalLookCountSum += allLookCount;
			//totalTimeSum += totalTime;
			leftLookCountSum += leftLookCount;
			leftTimeSum += leftTime;
			centerLookCountSum += centerLookCount;
			centerTimeSum += centerTime;
			rightLookCountSum += rightLookCount;
			rightTimeSum += rightTime;
		}
	}
	totalTimeSum = leftTimeSum + centerTimeSum + rightTimeSum;
	if (settings_.isExperimentSummaryShown())
	{
		QStringList lst = getPhaseSummary(phaseName);
		log.append(lst);
	}
	return log;
}
	
QStringList HOutputGenerator::getPhaseLookSummary(const QString& phaseName)
{
	QStringList log;
	log.append(QString("  %1 Phase:").arg(phaseName));
	HTrialInfo info;
	if (phaseName == "Pretest")
		info = pretestInfo_;
	else if (phaseName == "Habituation")
		info = habituationInfo_;
	else if (phaseName == "Test")
		info = testInfo_;
	int lookCount = 0;
	double timeCount = 0;
	trialcount = 0;
	for (int i = 0; i < info.logItems.count(); i++)
	{
		double duration = qRound(info.logItems[i].duration/100.0)/10.0;
		if (info.logItems[i].lookType == HTrialLogItem::REPEAT_TRIAL)
		{
			log.append(QString("    Trial %1 was repeated because no looking was recorded").arg(trialcount));
		}
		if (info.logItems[i].lookType == HTrialLogItem::NEW_TRIAL)
		{
			currentStimulusSettings_ = info.logItems[i].stimulusSettings;
			if (trialcount > 0)
			{
				
			}
			trialcount++;
			if (settings_.isStimulusNamesShown())
				log.append(QString("    Trial %1: (%2)").arg(trialcount).arg(currentStimulusSettings_.getName()));
			else
				log.append(QString("    Trial %1:").arg(trialcount));
			lookCount = 0;
			timeCount = 0;
			//currentStimulusSettings_ = info.logItems[i].stimulusSettings;
		}
		if (info.logItems[i].lookType == HTrialLogItem::NEW_TRIAL_REPEAT)
		{
			currentStimulusSettings_ = info.logItems[i].stimulusSettings;
			if (trialcount > 0)
			{
				
			}
			//trialcount++;
			if (settings_.isStimulusNamesShown())
				log.append(QString("    Trial %1: (%2)").arg(trialcount).arg(currentStimulusSettings_.getName()));
			else
				log.append(QString("    Trial %1:").arg(trialcount));
			lookCount = 0;
			timeCount = 0;
			//currentStimulusSettings_ = info.logItems[i].stimulusSettings;
		}
		if (info.logItems[i].lookType == HTrialLogItem::LEFT_LOOK)
		{
			++lookCount;
			log.append(QString("      Look %1 (Left)     -- Start Time: %2 sec, End Time:  %3 sec, Duration:  %4 sec").arg(lookCount).arg(timeCount).arg(timeCount+duration).arg(duration));
			timeCount += duration;
		}
		if (info.logItems[i].lookType == HTrialLogItem::RIGHT_LOOK)
		{
			++lookCount;
			log.append(QString("      Look %1 (Right)    -- Start Time: %2 sec, End Time:  %3 sec, Duration:  %4 sec").arg(lookCount).arg(timeCount).arg(timeCount+duration).arg(duration));
			timeCount += duration;
		}
		if (info.logItems[i].lookType == HTrialLogItem::CENTER_LOOK)
		{
			++lookCount;
			log.append(QString("      Look %1 (Center)   -- Start Time: %2 sec, End Time:  %3 sec, Duration:  %4 sec").arg(lookCount).arg(timeCount).arg(timeCount+duration).arg(duration));
			timeCount += duration;
		}
		if (info.logItems[i].lookType == HTrialLogItem::NO_LOOK)
		{
			timeCount += duration;
		}
		if (info.logItems[i].lookType == HTrialLogItem::AWAY_LOOK)
		{
			timeCount += duration;
		}
		
	}
	return log;
	
}
	
QStringList HOutputGenerator::toHtml(bool headerIncluded) 
{
	QStringList log;
	if (headerIncluded)
	{
		if (getResultsType() == "ORIGINAL RUN")
		{
			log.append("==== Subject Information ====");
			if (settings_.isSubjectNameShown())
				log.append(QString("Name: %1").arg(subjectInformation_.getSubjectName()));
			if (settings_.isSubjectBirthdateShown())
				log.append(QString("Birth Date: %1").arg(subjectInformation_.getBirthDate().toString("M/d/yy")));
			if (settings_.isTestDateShown())
				log.append(QString("Test Date: %1").arg(subjectInformation_.getTestDate().toString("M/d/yy")));
			if (settings_.isObserverShown())
				log.append(QString("Observer: %1").arg(subjectInformation_.getObserver()));
			if (settings_.isConditionShown())
				log.append(QString("Condition: %1").arg(subjectInformation_.getCondition()));
			if (settings_.isCellNoShown())
				log.append(QString("Cell Number: %1").arg(subjectInformation_.getCellNumber()));
			if (settings_.isCommentsShown())
				log.append(QString("Comments: %1").arg(subjectInformation_.getComments()));
		}
		else
		{
			log.append("Reliability Information:");
			log.append(QString("Observer: %1").arg(reliabilitySettings_.getObserver()));
			log.append(QString("Date: %1").arg(reliabilitySettings_.getDate().toString("M/d/yy")));
			log.append(QString("Comment: %1").arg(reliabilitySettings_.getComment()));
			log.append("");
		}
	}
	log.append("");
	log.append("");
	
	int pretestTrialsCount = 0;
	int habituationTrialsCount = 0;
	int testTrialsCount = 0;
	if (settings_.isPretestShown())
	{
		QStringList lst = getPhaseLog("Pretest");
		log.append(lst);
		pretestTrialsCount = trialcount;
	}
	if (settings_.isHabituationShown())
	{
		QStringList lst = getPhaseLog("Habituation");
		log.append(lst);
		habituationTrialsCount = trialcount;
	}
	if (settings_.isTestShown())
	{
		QStringList lst = getPhaseLog("Test");
		log.append(lst);
		testTrialsCount = trialcount;
	}
	
	if (settings_.isExperimentSummaryShown())
	{
		log.append(QString("  Number of Completed Pretest Trials: %1").arg(pretestTrialsCount));
		log.append(QString("  Number of Completed Habituation Trials: %1").arg(habituationTrialsCount));
		log.append(QString("  Number of Completed Test Trials: %1").arg(testTrialsCount));
		
		if (settings_.isLookTimelineShown())
		{
			log.append("    Look Summary:");
			QStringList lst = getPhaseLookSummary("Pretest");
			log.append(lst);
			lst = getPhaseLookSummary("Habituation");
			log.append(lst);
			lst = getPhaseLookSummary("Test");
			log.append(lst);
		}
	}
	
	QStringList header;
	if (headerIncluded)
	{
		header.append(QString("Experiment Name: %1").arg(experimentName_));
		header.append(QString("Settings File Name: %1").arg(settingsFileName_));
		header.append(QString("Results File Name: %1").arg(resultsFileName_));
		header.append(QString("Results Type: %1").arg(resultsType_));
		header.append(QString("Trial Type: %1").arg(trialType_));
		header.append(QString("Habituation: %1").arg(habituation_));
		header.append(QString("Habituation Result: %1").arg(habituationResult_));
	}
	
	header.append("");
	header.append(log);
	return header;
}
	
bool HOutputGenerator::save(const QString &fileName)
{
	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QDataStream out(&file);
		out << experimentName_ << settingsFileName_ << resultsFileName_ << resultsType_ << trialType_ << habituation_ << habituationResult_;
		out << runSettings_;
		out << reliabilitySettings_;
		out << pretestInfo_;
		out << habituationInfo_;
		out << testInfo_;
	}
	file.close();
	return true;
}
	
bool HOutputGenerator::load(const QString &fileName)
{
	QFile file(fileName);
	if (file.open(QIODevice::ReadOnly))
	{
		QDataStream in(&file);
		in >> experimentName_ >> settingsFileName_ >> resultsFileName_ >> resultsType_ >> trialType_ >> habituation_ >> habituationResult_;
		in >> runSettings_;
		in >> reliabilitySettings_;
		in >> pretestInfo_;
		in >> habituationInfo_;
		in >> testInfo_;
	}
	file.close();
	experimentSettings_.setId(runSettings_.getExperimentId());
	experimentSettings_.loadFromDB();
	Habit::MainDao dao;
	subjectInformation_ = dao.getSubjectSettings(runSettings_.getSubjectId());
	return true;
}
	
void HOutputGenerator:: setExperimentName(const QString & name)
{
	experimentName_ = name;
}

QString HOutputGenerator:: getExperimentName()
{
	return experimentName_;
}

void HOutputGenerator:: setSettingsFileName(const QString & name)
{
	settingsFileName_ = name;
}

QString HOutputGenerator:: getSettingsFileName()
{
	return settingsFileName_;
}

void HOutputGenerator:: setResultsFileName(const QString & name)
{
	resultsFileName_ = name;
}

QString HOutputGenerator:: getResultsFileName()
{
	return resultsFileName_;
}

void HOutputGenerator:: setResultsType(const QString & name)
{
	resultsType_ = name;
}

QString HOutputGenerator:: getResultsType()
{
	return resultsType_;
}
	
void HOutputGenerator:: setTrialType(const QString & name)
{
	trialType_ = name;
}

QString HOutputGenerator:: getTrialType()
{
	return trialType_;
}

void HOutputGenerator:: setHabituation(const QString & value)
{
	habituation_ = value;
}

QString HOutputGenerator:: getHabituation()
{
	return habituation_;
}

void HOutputGenerator:: setHabituationResult(const QString & value)
{
	habituationResult_ = value;
}

QString HOutputGenerator:: getHabituationResult()
{
	return habituationResult_;
}

bool HOutputGenerator::isHabituated()
{
	summarize();
	QList<double> totTimes;
	double totTime = 0;
	for (int i = 0; i < habituationInfo_.logItems.count(); i++)
	{
		HTrialLogItem info = habituationInfo_.logItems[i];
		if (info.lookType == HTrialLogItem::NEW_TRIAL)
		{
			if (i > 0)
			{
				totTimes.append(totTime);
			}
			totTime = 0;
		}
		if (info.lookType == HTrialLogItem::CENTER_LOOK ||
			info.lookType == HTrialLogItem::LEFT_LOOK ||
			info.lookType == HTrialLogItem::RIGHT_LOOK )
		{
			totTime += info.duration;
		}
	}
	
	if (habituationInfo_.logItems.count() > 0)
	{
		totTimes.append(totTime);
	}
	int habitType = (experimentSettings_.getHabituationSettings().getHabituationType());
	
	if (totTimes.count() >= (int)experimentSettings_.getDesignSettings().getHabituationTrialsInfo().getNumberOfTrials())
	{
		habituationResult_ = QString("Not Habituated - max trial count of %1 trials").arg(totTimes.count());
		return false;
	}
	
	if (habitType == Habit::HabituationSettings::eUseCriterionToEnd)
	{
		int windowSize = experimentSettings_.getHabituationSettings().getCriterionSettings().getWindowSize();
		bool slidingWindow = experimentSettings_.getHabituationSettings().getCriterionSettings().getWindowType() == Habit::CriterionSettings::eSlidingWindow;
		bool firstNTrial = experimentSettings_.getHabituationSettings().getCriterionSettings().getBasis() == Habit::CriterionSettings::eFirstN;
		double percent = experimentSettings_.getHabituationSettings().getCriterionSettings().getPercent() / 100.0;
		QList<double> windowSums;
		if (!slidingWindow)
		{
			if ((totTimes.count() % windowSize != 0) || (totTimes.count() / windowSize < 2) )
			{
				return false;
			}
			int windowCount = totTimes.count()/windowSize;
			
			for (int i = 0; i < windowCount; i++)
			{
				double sumTime = 0;
				int index = i * windowSize;
				for (int j = index; j < index + windowSize; j++)
				{
					sumTime += totTimes[j];
				}
				windowSums.append(sumTime);
			}
		}
		else
		{
			if (totTimes.count() < windowSize + 1)
				return false;
			
			for (int i = 0; i <= totTimes.count()-windowSize; i++)
			{
				double sumTime = 0;
				int index = i;
				for (int j = index; j < index + windowSize; j++)
				{
					sumTime += totTimes[j];
				}
				windowSums.append(sumTime);
			}
			
		}
		double criterionValue = 0;
		if (firstNTrial)
		{
			criterionValue = windowSums[0];
		}
		else
		{
			for (int i = 0; i < windowSums.count()-1; i++)
			{
				if (windowSums[i] > criterionValue)
					criterionValue = windowSums[i];
			}
		}
		if (windowSums[windowSums.count()-1] < criterionValue * percent)
		{
			habituationResult_ = QString("Habituated - after %1 trial").arg(totTimes.count());
			return true;
		}
		else
			return false;
	}
	
	if (habitType == Habit::HabituationSettings::eUseTotalLookingTimeToEnd)
	{
		double sumTime = 0;
		for (int i = 0; i < totTimes.count(); i++)
			sumTime += totTimes[i];
		double maxLookTime = experimentSettings_.getHabituationSettings().getTotalLookLengthToEnd();
		if (sumTime > maxLookTime)
		{
			habituationResult_ = QString("Habituated - after %1 trial").arg(totTimes.count());
			return true;
		}
		else
			return false;
	}
	return false;
	
}
	
