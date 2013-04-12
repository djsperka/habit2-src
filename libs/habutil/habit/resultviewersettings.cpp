#include "resultviewersettings.h"

#include <QtCore/QStringList>

namespace Habit {

const QString ResultViewerSettings::VALUES_SEPARATOR = ";";
const QString ResultViewerSettings::SUBJECT_NAME = "subj_name";
const QString ResultViewerSettings::BIRTHDATE = "BD";
const QString ResultViewerSettings::TESTDATE = "TD";
const QString ResultViewerSettings::OBSERVER = "observer";
const QString ResultViewerSettings::CONDITION = "condition";
const QString ResultViewerSettings::CELLNO = "cell_no";
const QString ResultViewerSettings::COMMENTS = "comments";
const QString ResultViewerSettings::ONGFILE = "ongfile";
const QString ResultViewerSettings::RELDATE = "reldate";
const QString ResultViewerSettings::RELOBSERVER = "relobserver";
const QString ResultViewerSettings::RELCOMMENTS = "relcomments";
const QString ResultViewerSettings::PRETEST = "pretest";
const QString ResultViewerSettings::HABITUATION = "habituation";
const QString ResultViewerSettings::TEST = "test";
const QString ResultViewerSettings::EXPERIMENT_SUMMARY = "ES";
const QString ResultViewerSettings::LOOK_TIMELINE = "LT";
const QString ResultViewerSettings::TRUE_VALUE = "1";
const QString ResultViewerSettings::FALSE_VALUE = "0";
const QString ResultViewerSettings::EQUAL_SIGN = "=";
const QString ResultViewerSettings::ATTENTION_LATENCIES = "attlat";
const QString ResultViewerSettings::ATTENTION_ON_TIMES = "atttimes";
const QString ResultViewerSettings::LEFT_MONITOR_LOOK_TIMES = "lmlktm";
const QString ResultViewerSettings::RIGHT_MONITOR_LOOK_TIMES = "rmlktm";
const QString ResultViewerSettings::CENTER_MONITOR_LOOK_TIMES = "cmlktm";
const QString ResultViewerSettings::TOTAL_LOOK_TIMES = "totlktm";
const QString ResultViewerSettings::TRIAL_DURATIONS = "trdur";
const QString ResultViewerSettings::NUMBER_OF_LEFT_LOOKS = "lmlknum";
const QString ResultViewerSettings::NUMBER_OF_CENTER_LOOKS = "cmlknum";
const QString ResultViewerSettings::NUMBER_OF_RIGHT_LOOKS = "rmlknum";
const QString ResultViewerSettings::TOAL_NUMBER_OF_LOOKS = "totlknum";
const QString ResultViewerSettings::LEFT_MONITOR_FILENAMES = "lmfname";
const QString ResultViewerSettings::CENTER_MONITOR_FILENAMES = "cmfname";
const QString ResultViewerSettings::RIGHT_MONITOR_FILENAMES = "rmfname";
const QString ResultViewerSettings::INDEPENDENT_MONITOR_FILENAMES = "imfname";
const QString ResultViewerSettings::STIMULUS_NAMES = "stnames";

void ResultViewerSettings::fromString(const QString& str) {
	QStringList lst = str.split(VALUES_SEPARATOR);
	for(QStringList::iterator it = lst.begin(); it != lst.end(); ++it) {
		QString item = *it;
		QStringList nameValue = item.split(EQUAL_SIGN);
		if(nameValue.size() > 1) {
			QString name = nameValue[0];
			QString value = nameValue[1];
			assign(name, value);
		}
	}
}

QString ResultViewerSettings::toString() const {
	QString str;
	str.append(getStringValue(SUBJECT_NAME, isSubjectNameShown_));
	str.append(getStringValue(BIRTHDATE, isSubjectBirthdateShown_));
	str.append(getStringValue(TESTDATE, isTestDateShown_));
	str.append(getStringValue(OBSERVER, isObserverShown_));
	str.append(getStringValue(CONDITION, isConditionShown_));
	str.append(getStringValue(CELLNO, isCellNoShown_));
	str.append(getStringValue(COMMENTS, isCommentsShown_));
	str.append(getStringValue(ONGFILE, isOngFileNameShown_));
	str.append(getStringValue(RELDATE, isRelDateShown_));
	str.append(getStringValue(RELOBSERVER, isRelObserverShown_));
	str.append(getStringValue(RELCOMMENTS, isRelCommentsShown_));
	str.append(getStringValue(PRETEST, isPretestShown_));
	str.append(getStringValue(HABITUATION, isHabituationShown_));
	str.append(getStringValue(TEST, isTestShown_));
	str.append(getStringValue(EXPERIMENT_SUMMARY, isExperimentSummaryShown_));
	str.append(getStringValue(LOOK_TIMELINE, isLookTimeLineShown_));
	str.append(getStringValue( ATTENTION_LATENCIES, isAttentionLatenciesShown_));
	str.append(getStringValue( ATTENTION_ON_TIMES, isAttentionOnTimesShown_));
	str.append(getStringValue( LEFT_MONITOR_LOOK_TIMES, isLeftMonitorLookTimesShown_));
	str.append(getStringValue( RIGHT_MONITOR_LOOK_TIMES, isRightMonitorLookTimesShown_));
	str.append(getStringValue( CENTER_MONITOR_LOOK_TIMES, isCenterMonitorLookTimesShown_));
	str.append(getStringValue( TOTAL_LOOK_TIMES, isTotalLookTimesShown_));
	str.append(getStringValue( TRIAL_DURATIONS, isTrialDurationsShown_));
	str.append(getStringValue( NUMBER_OF_LEFT_LOOKS, isNumberOfLeftLooksShown_));
	str.append(getStringValue( NUMBER_OF_CENTER_LOOKS, isNumberOfCenterLooksShown_));
	str.append(getStringValue( NUMBER_OF_RIGHT_LOOKS, isNumberOfRightLooksShown_));
	str.append(getStringValue( TOAL_NUMBER_OF_LOOKS, isTotalNumberOfLooksShown_));
	str.append(getStringValue( LEFT_MONITOR_FILENAMES, isLeftMonitorFilenamesShown_));
	str.append(getStringValue( CENTER_MONITOR_FILENAMES, isCenterMonitorFilenamesShown_));
	str.append(getStringValue( RIGHT_MONITOR_FILENAMES, isRightMonitorFilenamesShown_));
	str.append(getStringValue( INDEPENDENT_MONITOR_FILENAMES, isIndependentFilenamesShown_));
	str.append(getStringValue( STIMULUS_NAMES, isStimulusNamesShown_));
	return str;
}

void ResultViewerSettings::assign(const QString& name, const QString& value) {
	bool val = (value == TRUE_VALUE ? true : false);
	if(name == SUBJECT_NAME) {
		isSubjectNameShown_ = val;
	} else if(name == BIRTHDATE) {
		isSubjectBirthdateShown_ = val;
	} else if(name == TESTDATE) {
		isTestDateShown_ = val;
	} else if(name == OBSERVER) {
		isObserverShown_ = val;
	} else if(name == CONDITION) {
		isConditionShown_ = val;
	} else if(name == CELLNO) {
		isCellNoShown_ = val;
	} else if(name == COMMENTS) {
		isCommentsShown_ = val;
	} else if(name == ONGFILE) {
		isOngFileNameShown_ = val;
	} else if(name == RELDATE) {
		isRelDateShown_ = val;
	} else if(name == RELOBSERVER) {
		isRelObserverShown_ = val;
	} else if(name == RELCOMMENTS) {
		isRelCommentsShown_ = val;
	} else if(name == PRETEST) {
		isPretestShown_ = val;
	} else if(name == HABITUATION) {
		isHabituationShown_ = val;
	} else if(name == TEST) {
		isTestShown_ = val;
	} else if(name == EXPERIMENT_SUMMARY) {
		isExperimentSummaryShown_ = val;
	} else if(name == LOOK_TIMELINE) {
		isLookTimeLineShown_ = val;
	}
	else if(name == ATTENTION_LATENCIES) {
		isAttentionLatenciesShown_ = val;
	}
	else if(name ==  ATTENTION_ON_TIMES) { isAttentionOnTimesShown_ = val; }
	else if(name ==  LEFT_MONITOR_LOOK_TIMES) { isLeftMonitorLookTimesShown_ = val; }
	else if(name ==  RIGHT_MONITOR_LOOK_TIMES) { isRightMonitorLookTimesShown_ = val; }
	else if(name ==  CENTER_MONITOR_LOOK_TIMES) { isCenterMonitorLookTimesShown_ = val; }
	else if(name ==  TOTAL_LOOK_TIMES) { isTotalLookTimesShown_ = val; }
	else if(name ==  TRIAL_DURATIONS) { isTrialDurationsShown_ = val; }
	else if(name ==  NUMBER_OF_LEFT_LOOKS) { isNumberOfLeftLooksShown_ = val; }
	else if(name ==  NUMBER_OF_CENTER_LOOKS) { isNumberOfCenterLooksShown_ = val; }
	else if(name ==  NUMBER_OF_RIGHT_LOOKS) { isNumberOfRightLooksShown_ = val; }
	else if(name ==  TOAL_NUMBER_OF_LOOKS) { isTotalNumberOfLooksShown_ = val; }
	else if(name ==  LEFT_MONITOR_FILENAMES) { isLeftMonitorFilenamesShown_ = val; }
	else if(name ==  CENTER_MONITOR_FILENAMES) { isCenterMonitorFilenamesShown_ = val; }
	else if(name ==  RIGHT_MONITOR_FILENAMES) { isRightMonitorFilenamesShown_ = val; }
	else if(name ==  INDEPENDENT_MONITOR_FILENAMES) { isIndependentFilenamesShown_ = val; }
	else if(name ==  STIMULUS_NAMES) { isStimulusNamesShown_ = val; }
}

QString ResultViewerSettings::getStringValue(const QString& name, bool value) const {
	return name + EQUAL_SIGN + (value ? TRUE_VALUE : FALSE_VALUE) + VALUES_SEPARATOR;
}

} // namespace Habit