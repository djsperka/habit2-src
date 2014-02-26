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
#include <QTextStream>


class HResults {
public:
	HResults(HResults& r);

	// This constructor used for ORIGINAL RUN.
	HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::SubjectSettings& ss, const HEventLog& log, const QString filename = QString(""), const QString version = QString(""));

	// constructor for TEST RUN
	HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const HEventLog& log, const QString filename = QString(""), const QString version = QString(""));

	// This constructor used for RELIABILTY RUN. The filename is that for the ORIGINAL run, and the event log is from the reliability run, not the original run.
	HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::ReliabilitySettings& rs, const HEventLog& log, const QString origFilename = QString(""), const QString filename = QString(""), const QString version = QString(""));

	virtual ~HResults();

	const QString& version() const { return m_version; };
	const HResultsType& type() const { return *m_pResultsType; };
	const QString& originalFilename() const { return m_originalFilename; };
	const QString& filename() const { return m_filename; };
	const Habit::ExperimentSettings& experimentSettings() const { return m_experimentSettings; };
	const Habit::RunSettings& runSettings() const { return m_runSettings; };
	const Habit::SubjectSettings& subjectSettings() const { return m_subjectSettings; };
	const Habit::ReliabilitySettings& reliabilitySettings() const { return m_reliabilitySettings; };
	const HEventLog& eventLog() const { return m_log; };

	// Load an HResults object from a file.
	static HResults* load(const QString& filename);

	// save to file. File will be CLOBBERED!
	bool save(const QString& filename) const;

	// save per-trial results to a CSV file
	bool saveToCSV(const QString& filename) const;

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
	Habit::ReliabilitySettings m_reliabilitySettings;
	HEventLog m_log;
};

class HTrialResultsRow: public QList<QString>
{
public:
	enum
	{
		indSubjectId = 0,
		indPhase = 1,
		indTrial = 2,
		indRepeat = 3,
		indHabituated = 4,
		indStimId = 5,
		indStimName = 6,
		indStimLeft = 7,
		indStimCenter = 8,
		indStimRight = 9,
		indStimISS = 10,
		indTotalLook = 11,
		indTotalAway = 12,
		indLook = 13,
		indNInit = 14,
	};

	HTrialResultsRow(): QList<QString>()
	{
		init();
	}

	void init(QString& subjectId)
	{
		init();
		setId(subjectId);
	};

	void setId(QString id) { (*this)[indSubjectId] = id; };
	void setPhase(QString phase) { (*this)[indPhase] = phase; };
	void setTrial(QString trial) { (*this)[indTrial] = trial; };
	void setRepeat(QString repeat) { (*this)[indRepeat] = repeat; };
	void setHabituated(QString habituated) { (*this)[indHabituated] = habituated; };
	void setStimId(QString stimid) { (*this)[indStimId] = stimid; };
	void setStimName(QString stim) { (*this)[indStimName] = stim; };
	void setStimLeft(QString stim) { (*this)[indStimLeft] = stim; };
	void setStimCenter(QString stim) { (*this)[indStimCenter] = stim; };
	void setStimRight(QString stim) { (*this)[indStimRight] = stim; };
	void setStimISS(QString stim) { (*this)[indStimISS] = stim; };
	void setTotalLook(QString look) { (*this)[indTotalLook] = look; };
	void setTotalLookAway(QString away) { (*this)[indTotalAway] = away; };
	void appendLook(QString direc, QString timeMS) { append(direc); append(timeMS); };

private:
	void init()
	{
		clear();
		for (int i=0; i<indNInit; i++) append(QString());
	};

};

QTextStream& operator<<(QTextStream& out, const HTrialResultsRow& row);


#endif /* HRESULTS_H_ */
