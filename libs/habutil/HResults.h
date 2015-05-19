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
//#include "reliabilitysettings.h"
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
	// DJS 2-24-15 HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::ReliabilitySettings& rs, const HEventLog& log, const QString origFilename = QString(""), const QString filename = QString(""), const QString version = QString(""));

	virtual ~HResults();

	const QString& version() const { return m_version; };
	const HResultsType& type() const { return *m_pResultsType; };
	const QString& originalFilename() const { return m_originalFilename; };
	const QString& filename() const { return m_filename; };
	const Habit::ExperimentSettings& experimentSettings() const { return m_experimentSettings; };
	const Habit::RunSettings& runSettings() const { return m_runSettings; };
	const Habit::SubjectSettings& subjectSettings() const { return m_subjectSettings; };
	// 2-24-15 djs const Habit::ReliabilitySettings& reliabilitySettings() const { return m_reliabilitySettings; };
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
	// 2-24-15 djs Habit::ReliabilitySettings m_reliabilitySettings;
	HEventLog m_log;
};

class HTrialResultsRow: public QList<QString>
{
public:
	static QStringList headers;
	enum
	{
		indSubjectId = 0,
		indPhase = 1,
		indOrderName = 2,
		indTrial = 3,
		indRepeat = 4,
		indEndType = 5,
		indHabituated = 6,
		indStimId = 7,
		indStimName = 8,
		indStimLabel = 9,
		indStimLeft = 10,
		indStimCenter = 11,
		indStimRight = 12,
		indStimISS = 13,
		indTrialStartTime = 14,
		indTrialEndTime = 15,
		indTotalLook = 16,
		indTotalAway = 17,
		indNInit = 18,
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
	void setTrial(int itrial) { (*this)[indTrial] = QString("%1").arg(itrial); };
	void setOrderName(QString name) { (*this)[indOrderName] = name; };
	void setRepeat(int irepeat) { (*this)[indRepeat] = QString("%1").arg(irepeat); };
	void setEndType(QString type) { (*this)[indEndType] = type; };
	void setHabituated(QString habituated) { (*this)[indHabituated] = habituated; };
	void setStimId(int stimid) { (*this)[indStimId] = QString("%1").arg(stimid); };
	void setStimName(QString stim) { (*this)[indStimName] = stim; };
	void setStimLabel(QString label) { (*this)[indStimLabel] = label; };
	void setStimLeft(QString stim) { (*this)[indStimLeft] = stim; };
	void setStimCenter(QString stim) { (*this)[indStimCenter] = stim; };
	void setStimRight(QString stim) { (*this)[indStimRight] = stim; };
	void setStimISS(QString stim) { (*this)[indStimISS] = stim; };
	void setTotalLook(QString look) { (*this)[indTotalLook] = look; };
	void setTotalLookAway(QString away) { (*this)[indTotalAway] = away; };
	void setTrialStartTime(int t) { (*this)[indTrialStartTime] = QString("%1").arg(t); m_lastLookEndTime = t; };
	void setTrialEndTime(int t)
	{
		(*this)[indTrialEndTime] = QString("%1").arg(t);
		(*this)[indTotalLook] = QString("%1").arg(m_totalLookTime);
		(*this)[indTotalAway] = QString("%1").arg(m_totalLookAwayTime);
	};
	void appendLook(HLook look);
	//void appendLook(QString direc, QString startMS, QString endMS, QString timeMS) { append(direc); append(startMS); append(endMS); append(timeMS); };
	const QList<HLook>& looks() const { return m_looks; };

private:
	int m_lastLookEndTime;
	int m_totalLookTime;
	int m_totalLookAwayTime;
	QList<HLook> m_looks;
	void init()
	{
		m_lastLookEndTime = 0;
		m_totalLookTime = 0;
		m_totalLookAwayTime = 0;
		clear();
		m_looks.clear();
		for (int i=0; i<indNInit; i++) append(QString());
	};

};

QTextStream& operator<<(QTextStream& out, const HTrialResultsRow& row);


#endif /* HRESULTS_H_ */
