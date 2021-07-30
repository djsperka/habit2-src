/*
 * HResults.h
 *
 *  Created on: May 9, 2013
 *      Author: Oakeslab
 */

#ifndef HRESULTS_H_
#define HRESULTS_H_


#include "HTypes.h"
#include "experimentsettings.h"
#include "runsettings.h"
#include "subjectsettings.h"
#include "HEventLog.h"
#include <QTextStream>


class HTrialScanner;

class HResults {
public:
	HResults(HResults& r);

	// This constructor used for ORIGINAL RUN.
	HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::SubjectSettings& ss, const HEventLog& log, const QString filename = QString(""), const QString version = QString(""));

	// constructor for TEST RUN
	HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const HEventLog& log, const QString filename = QString(""), const QString version = QString(""));

	// This constructor used for RELIABILTY RUN. The filename is that for the ORIGINAL run, and the event log is from the reliability run, not the original run.
	// DJS 2-24-15 HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::ReliabilitySettings& rs, const HEventLog& log, const QString origFilename = QString(""), const QString filename = QString(""), const QString version = QString(""));

	virtual ~HResults();

	const QString& version() const { return m_version; };
	const HResultsType& type() const { return *m_pResultsType; };
	const QString& originalFilename() const { return m_originalFilename; };
	const QString& filename() const { return m_filename; };
	const Habit::ExperimentSettings& experimentSettings() const { return m_experimentSettings; };
	const Habit::RunSettings& runSettings() const { return m_runSettings; };
	const Habit::SubjectSettings& subjectSettings() const { return m_subjectSettings; };
	const HEventLog& eventLog() const { return m_log; };

	// Load an HResults object from a file.
	static HResults* load(const QString& filename);

	// Analyze results file -- look for duplicate Look
	static int checkHabFileForDups(const QString& sPath, QTextStream& output);

	// save to file. File will be CLOBBERED!
	bool save(const QString& filename) const;

	// save per-trial results to a CSV file
	bool saveToCSV(const QString& filename, bool bReplace = false) const;
	bool scanTrials(const HTrialScanner& scanner) const;

private:
	HResults();

	void initializeTrialRow(QList<QString>& row);

	QString m_version;
	QString m_originalFilename;
	QString m_filename;
	const HResultsType* m_pResultsType;
	Habit::ExperimentSettings m_experimentSettings;
	Habit::RunSettings m_runSettings;
	Habit::SubjectSettings m_subjectSettings;
	// 2-24-15 djs Habit::ReliabilitySettings m_reliabilitySettings;
	HEventLog m_log;
};


#endif /* HRESULTS_H_ */
