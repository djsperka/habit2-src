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
#include "reliabilitysettings.h"
#include "HEventLog.h"
#include <QDataStream>


class HResults {
public:
	virtual ~HResults();

	const QString& version() const { return m_version; };
	const HResultsType& type() const { return *m_pResultsType; };
	const QString& originalFilename() const { return m_originalFilename; };
	const QString& reliabilityFilename() const { return m_reliabilityFilename; };
	const Habit::ExperimentSettings& experimentSettings() const { return m_experimentSettings; };
	const Habit::RunSettings& runSettings() const { return m_runSettings; };
	const Habit::SubjectSettings& subjectSettings() const { return m_subjectSettings; };
	const Habit::ReliabilitySettings& reliabilitySettings() const { return m_reliabilitySettings; };
	const HEventLog& eventLog() const { return m_log; };

	static HResults* load(const QString& filename);

	// save ORIGINAL RUN data
	static bool save(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::SubjectSettings& ss, const HEventLog& log, const QString& filename, const QString& version);

	// save TEST RUN data
	static bool save(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const HEventLog& log, const QString& filename, const QString& version);

	// save RELIABILITY data
	static bool save(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::ReliabilitySettings& bs, const HEventLog& log, const QString& filename, const QString& origFilename, const QString& version);


private:
	HResults();
	HResults(HResults& r);

	// This constructor used for ORIGINAL RUN. If give, the filename is the results filename the settings and log are taken from.
	HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::SubjectSettings& ss, const HEventLog& log, const QString filename = QString(""), const QString version = QString(""));

	// constructor for TEST RUN
	HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const HEventLog& log, const QString filename = QString(""), const QString version = QString(""));

	// This constructor used for RELIABILTY RUN. The filename is the RELIABILITY results file; the HResults object
	// is the results from the ORIGINAL RUN. The event log is from the reliability run, not the original run.
	HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::ReliabilitySettings& rs, const HEventLog& log, const QString origFilename = QString(""), const QString& reliabilityFilename = QString(""), const QString version = QString(""));

	QString m_version;
	QString m_originalFilename;
	QString m_reliabilityFilename;
	const HResultsType* m_pResultsType;
	Habit::ExperimentSettings m_experimentSettings;
	Habit::RunSettings m_runSettings;
	Habit::SubjectSettings m_subjectSettings;
	Habit::ReliabilitySettings m_reliabilitySettings;
	HEventLog m_log;
};

//QDataStream & operator << (QDataStream& stream, HResults r);
//QDataStream & operator >> (QDataStream& stream, HResults& r);

#endif /* HRESULTS_H_ */
