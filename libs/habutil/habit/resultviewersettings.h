
#ifndef HABIT_RESULT_VIEWER_SETTINGS_H
#define HABIT_RESULT_VIEWER_SETTINGS_H

#include <QtCore/QString>

namespace Habit {

class ResultViewerSettings {
public:
	static const QString VALUES_SEPARATOR;
	static const QString SUBJECT_NAME;
	static const QString BIRTHDATE;
	static const QString TESTDATE;
	static const QString OBSERVER;
	static const QString CONDITION;
	static const QString CELLNO;
	static const QString COMMENTS;
	static const QString ONGFILE;
	static const QString RELDATE;
	static const QString RELOBSERVER;
	static const QString RELCOMMENTS;
	static const QString PRETEST;
	static const QString HABITUATION;
	static const QString TEST;
	static const QString EXPERIMENT_SUMMARY;
	static const QString LOOK_TIMELINE;
	static const QString ATTENTION_LATENCIES;
	static const QString ATTENTION_ON_TIMES;
	static const QString LEFT_MONITOR_LOOK_TIMES;
	static const QString RIGHT_MONITOR_LOOK_TIMES;
	static const QString CENTER_MONITOR_LOOK_TIMES;
	static const QString TOTAL_LOOK_TIMES;
	static const QString TRIAL_DURATIONS;
	static const QString NUMBER_OF_LEFT_LOOKS;
	static const QString NUMBER_OF_CENTER_LOOKS;
	static const QString NUMBER_OF_RIGHT_LOOKS;
	static const QString TOAL_NUMBER_OF_LOOKS;
	static const QString LEFT_MONITOR_FILENAMES;
	static const QString CENTER_MONITOR_FILENAMES;
	static const QString RIGHT_MONITOR_FILENAMES;
	static const QString INDEPENDENT_MONITOR_FILENAMES;
	static const QString STIMULUS_NAMES;
	static const QString TRUE_VALUE;
	static const QString FALSE_VALUE;
	static const QString EQUAL_SIGN;

public:
	ResultViewerSettings() 
		: id_(-1)
		, isSubjectNameShown_(true)
		, isSubjectBirthdateShown_(true)
		, isTestDateShown_(true)
		, isObserverShown_(true)
		, isConditionShown_(true)
		, isCellNoShown_(true)
		, isCommentsShown_(true)
		, isOngFileNameShown_(true)
		, isRelDateShown_(true)
		, isRelObserverShown_(true)
		, isRelCommentsShown_(true)
		, isPretestShown_(true)
		, isHabituationShown_(true)
		, isTestShown_(true)
		, isExperimentSummaryShown_(true)
		, isLookTimeLineShown_(true)
	{}

	~ResultViewerSettings() {}

public:
	int getId() const {return id_;}
	bool isSubjectNameShown() const {return isSubjectNameShown_;}
	bool isSubjectBirthdateShown() const {return isSubjectBirthdateShown_;}
	bool isTestDateShown() const {return isTestDateShown_;}
	bool isObserverShown() const {return isObserverShown_;}
	bool isConditionShown() const {return isConditionShown_;}
	bool isCellNoShown() const {return isCellNoShown_;}
	bool isCommentsShown() const {return isCommentsShown_;}
	bool isOngFileNameShown() const {return isOngFileNameShown_;}
	bool isRelDateShown() const {return isRelDateShown_;}
	bool isRelObserverShown() const {return isRelObserverShown_;}
	bool isRelCommentsShown() const {return isRelCommentsShown_;}
	bool isPretestShown() const {return isPretestShown_;}
	bool isHabituationShown() const {return isHabituationShown_;}
	bool isTestShown() const {return isTestShown_;}
	bool isExperimentSummaryShown() const {return isExperimentSummaryShown_;}
	bool isLookTimelineShown() const {return isLookTimeLineShown_;}
	bool isAttentionLatenciesShown() const {return isAttentionLatenciesShown_;}
	bool isAttentionOnTimesShown() const {return isAttentionOnTimesShown_;}
	bool isLeftMonitorLookTimesShown() const {return isLeftMonitorLookTimesShown_;}
	bool isCenterMonitorLookTimesShown() const {return isCenterMonitorLookTimesShown_;}
	bool isRightMonitorLookTimesShown() const {return isRightMonitorLookTimesShown_;}
	bool isTotalLookTimesShown() const {return isTotalLookTimesShown_;}
	bool isTrialDurationsShown() const {return isTrialDurationsShown_;}
	bool isNumberOfLeftLooksShown() const {return isNumberOfLeftLooksShown_;}
	bool isNumberOfRightLooksShown() const {return isNumberOfRightLooksShown_;}
	bool isNumberOfCenterLooksShown() const {return isNumberOfCenterLooksShown_;}
	bool isTotalNumberOfLooksShown() const {return isTotalNumberOfLooksShown_;}
	bool isLeftMonitorFilenamesShown() const {return isLeftMonitorFilenamesShown_;}
	bool isCenterMonitorFilenamesShown() const {return isCenterMonitorFilenamesShown_;}
	bool isRightMonitorFilenamesShown() const {return isRightMonitorFilenamesShown_;}
	bool isIndependentFilenamesShown() const {return isIndependentFilenamesShown_;}
	bool isStimulusNamesShown() const {return isStimulusNamesShown_;}

	bool isReliabilityEnabled() const {return isReliabilityEnabled_;}
	bool isPretestEnabled() const {return isPretestEnabled_;}
	bool isHabituationenabled() const {return isHabituationenabled_;}
	bool isTestEnabled() const {return isTestEnabled_;}
	bool isAttentionLatenciesEnabled() const {return isAttentionLatenciesEnabled_;}
	bool isAttentionOnTimesEnabled() const {return isAttentionOnTimesEnabled_;}
	bool isLeftMonitorLookTimesEnabled() const {return isLeftMonitorLookTimesEnabled_;}
	bool isCenterMonitorLookTimesEnabled() const {return isCenterMonitorLookTimesEnabled_;}
	bool isRightMonitorLookTimesEnabled() const {return isRightMonitorLookTimesEnabled_;}
	bool isTotalLookTimesEnabled() const {return isTotalLookTimesEnabled_;}
	bool isTrialDurationsEnabled() const {return isTrialDurationsEnabled_;}
	bool isNumberOfLeftLooksEnabled() const {return isNumberOfLeftLooksEnabled_;}
	bool isNumberOfRightLooksEnabled() const {return isNumberOfRightLooksEnabled_;}
	bool isNumberOfCenterLooksEnabled() const {return isNumberOfCenterLooksEnabled_;}
	bool isTotalNumberOfLooksEnabled() const {return isTotalNumberOfLooksEnabled_;}
	bool isLeftMonitorFilenamesEnabled() const {return isLeftMonitorFilenamesEnabled_;}
	bool isCenterMonitorFilenamesEnabled() const {return isCenterMonitorFilenamesEnabled_;}
	bool isRightMonitorFilenamesEnabled() const {return isRightMonitorFilenamesEnabled_;}
	bool isIndependentFilenamesEnabled() const {return isIndependentFilenamesEnabled_;}
	bool isStimulusNamesEnabled() const {return isStimulusNamesEnabled_;}

	void setId(int id) {id_ = id;}
	void setSubjectNameShown(bool isShown) {isSubjectNameShown_ = isShown;}
	void setSubjectBirthdateShown(bool isShown) {isSubjectBirthdateShown_ = isShown;}
	void setTestDateShown(bool isShown) {isTestDateShown_ = isShown;}
	void setObserverShown(bool isShown) {isObserverShown_ = isShown;}
	void setConditionShown(bool isShown) {isConditionShown_ = isShown;}
	void setCellNoShown(bool isShown) {isCellNoShown_ = isShown;}
	void setCommentsShown(bool isShown) {isCommentsShown_ = isShown;}
	void setOngFileShown(bool isShown) {isOngFileNameShown_ = isShown;}
	void setRelDateShown(bool isShown) {isRelDateShown_ = isShown;}
	void setRelObserverShown(bool isShown) {isRelObserverShown_ = isShown;}
	void setRelCommentsShown(bool isShown) {isRelCommentsShown_ = isShown;}
	void setPretestShown(bool isShown) {isPretestShown_ = isShown;}
	void setHabituationShown(bool isShown) {isHabituationShown_ = isShown;}
	void setTestShown(bool isShown) {isTestShown_ = isShown;}
	void setExperimentSummaryShown(bool isShown) {isExperimentSummaryShown_ = isShown;}
	void setLookTimelineShown(bool isShown) {isLookTimeLineShown_ = isShown;}
	void setAttentionLatenciesShown(bool isShown)  { isAttentionLatenciesShown_ = isShown;}
	void setAttentionOnTimesShown(bool isShown)  { isAttentionOnTimesShown_ = isShown;}
	void setLeftMonitorLookTimesShown(bool isShown)  { isLeftMonitorLookTimesShown_ = isShown;}
	void setCenterMonitorLookTimesShown(bool isShown)  { isCenterMonitorLookTimesShown_ = isShown;}
	void setRightMonitorLookTimesShown(bool isShown)  { isRightMonitorLookTimesShown_ = isShown;}
	void setTotalLookTimesShown(bool isShown)  { isTotalLookTimesShown_ = isShown;}
	void setTrialDurationsShown(bool isShown)  { isTrialDurationsShown_ = isShown;}
	void setNumberOfLeftLooksShown(bool isShown)  { isNumberOfLeftLooksShown_ = isShown;}
	void setNumberOfRightLooksShown(bool isShown)  { isNumberOfRightLooksShown_ = isShown;}
	void setNumberOfCenterLooksShown(bool isShown)  { isNumberOfCenterLooksShown_ = isShown;}
	void setTotalNumberOfLooksShown(bool isShown)  { isTotalNumberOfLooksShown_ = isShown;}
	void setLeftMonitorFilenamesShown(bool isShown)  { isLeftMonitorFilenamesShown_ = isShown;}
	void setCenterMonitorFilenamesShown(bool isShown)  { isCenterMonitorFilenamesShown_ = isShown;}
	void setRightMonitorFilenamesShown(bool isShown)  { isRightMonitorFilenamesShown_ = isShown;}
	void setIndependentFilenamesShown(bool isShown)  { isIndependentFilenamesShown_ = isShown;}
	void setStimulusNamesShown(bool isShown)  { isStimulusNamesShown_ = isShown;}

	void setReliabilityEnabled(bool isEnabled)  {isReliabilityEnabled_ = isEnabled; if (!isEnabled) { isOngFileNameShown_ = isRelCommentsShown_ = isRelDateShown_ = isRelObserverShown_ = false; } }
	void setPretestEnabled(bool isEnabled)  { isPretestEnabled_ = isEnabled; if (!isEnabled) { isPretestShown_ = false; } }
	void setHabituationenabled(bool isEnabled)  { isHabituationenabled_ = isEnabled; if (!isEnabled) { isHabituationShown_ = false; } }
	void setTestEnabled(bool isEnabled)  { isTestEnabled_ = isEnabled; if (!isEnabled) { isPretestShown_ = false; } }
	void setAttentionLatenciesEnabled(bool isEnabled)  { isAttentionLatenciesEnabled_ = isEnabled;}
	void setAttentionOnTimesEnabled(bool isEnabled)  { isAttentionOnTimesEnabled_ = isEnabled;}
	void setLeftMonitorLookTimesEnabled(bool isEnabled)  { isLeftMonitorLookTimesEnabled_ = isEnabled;}
	void setCenterMonitorLookTimesEnabled(bool isEnabled)  { isCenterMonitorLookTimesEnabled_ = isEnabled;}
	void setRightMonitorLookTimesEnabled(bool isEnabled)  { isRightMonitorLookTimesEnabled_ = isEnabled;}
	void setTotalLookTimesEnabled(bool isEnabled)  { isTotalLookTimesEnabled_ = isEnabled;}
	void setTrialDurationsEnabled(bool isEnabled)  { isTrialDurationsEnabled_ = isEnabled;}
	void setNumberOfLeftLooksEnabled(bool isEnabled)  { isNumberOfLeftLooksEnabled_ = isEnabled;}
	void setNumberOfRightLooksEnabled(bool isEnabled)  { isNumberOfRightLooksEnabled_ = isEnabled;}
	void setNumberOfCenterLooksEnabled(bool isEnabled)  { isNumberOfCenterLooksEnabled_ = isEnabled;}
	void setTotalNumberOfLooksEnabled(bool isEnabled)  { isTotalNumberOfLooksEnabled_ = isEnabled;}
	void setLeftMonitorFilenamesEnabled(bool isEnabled)  { isLeftMonitorFilenamesEnabled_ = isEnabled;}
	void setCenterMonitorFilenamesEnabled(bool isEnabled)  { isCenterMonitorFilenamesEnabled_ = isEnabled;}
	void setRightMonitorFilenamesEnabled(bool isEnabled)  { isRightMonitorFilenamesEnabled_ = isEnabled;}
	void setIndependentFilenamesEnabled(bool isEnabled)  { isIndependentFilenamesEnabled_ = isEnabled;}
	void setStimulusNamesEnabled(bool isEnabled)  { isStimulusNamesEnabled_ = isEnabled;}

	void fromString(const QString& str);
	QString toString() const;

private:
	void assign(const QString& name, const QString& value);
	QString getStringValue(const QString& name, bool value) const;

private:
	int id_;
	bool isSubjectNameShown_;
	bool isSubjectBirthdateShown_;
	bool isTestDateShown_;
	bool isObserverShown_;
	bool isConditionShown_;
	bool isCellNoShown_;
	bool isCommentsShown_;
	bool isOngFileNameShown_;
	bool isRelDateShown_;
	bool isRelObserverShown_;
	bool isRelCommentsShown_;
	bool isPretestShown_;
	bool isHabituationShown_;
	bool isTestShown_;
	bool isExperimentSummaryShown_;
	bool isLookTimeLineShown_;
	bool isAttentionLatenciesShown_;
	bool isAttentionOnTimesShown_;
	bool isLeftMonitorLookTimesShown_;
	bool isCenterMonitorLookTimesShown_;
	bool isRightMonitorLookTimesShown_;
	bool isTotalLookTimesShown_;
	bool isTrialDurationsShown_;
	bool isNumberOfLeftLooksShown_;
	bool isNumberOfRightLooksShown_;
	bool isNumberOfCenterLooksShown_;
	bool isTotalNumberOfLooksShown_;
	bool isLeftMonitorFilenamesShown_;
	bool isCenterMonitorFilenamesShown_;
	bool isRightMonitorFilenamesShown_;
	bool isIndependentFilenamesShown_;
	bool isStimulusNamesShown_;

	bool isReliabilityEnabled_;
	bool isPretestEnabled_;
	bool isHabituationenabled_;
	bool isTestEnabled_;

	bool isAttentionLatenciesEnabled_;
	bool isAttentionOnTimesEnabled_;
	bool isLeftMonitorLookTimesEnabled_;
	bool isCenterMonitorLookTimesEnabled_;
	bool isRightMonitorLookTimesEnabled_;
	bool isTotalLookTimesEnabled_;
	bool isTrialDurationsEnabled_;
	bool isNumberOfLeftLooksEnabled_;
	bool isNumberOfRightLooksEnabled_;
	bool isNumberOfCenterLooksEnabled_;
	bool isTotalNumberOfLooksEnabled_;
	bool isLeftMonitorFilenamesEnabled_;
	bool isCenterMonitorFilenamesEnabled_;
	bool isRightMonitorFilenamesEnabled_;
	bool isIndependentFilenamesEnabled_;
	bool isStimulusNamesEnabled_;
};

} // namespace Habit

#endif // HABIT_RESULT_VIEWER_SETTINGS_H