/*
 * HResults.cpp
 *
 *  Created on: May 9, 2013
 *      Author: Oakeslab
 */

#include "HResults.h"
#include "HLook.h"
#include "HTrialScanner.h"
#include "HLookerReprocessor.h"
#include "HTrialKey.h"
#include <QFile>
#include <QDataStream>
#include <QCoreApplication>
#include <QtDebug>

HResults::HResults()
: m_originalFilename("")
, m_filename("NOT_SAVED")
, m_pResultsType(&HResultsType::HResultsTypeUnknown)
{}

HResults::HResults(HResults& r)
: m_version(r.version())
, m_originalFilename(r.originalFilename())
, m_filename(r.filename())
, m_pResultsType(&r.type())
, m_experimentSettings(r.experimentSettings())
, m_runSettings(r.runSettings())
, m_subjectSettings(r.subjectSettings())
// 2-24-15 djs , m_reliabilitySettings(r.reliabilitySettings())
, m_log(r.eventLog())
{};


// This constructor used for ORIGINAL RUN. The filename is the results filename the settings and log are taken from.
HResults::HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::SubjectSettings& ss, const HEventLog& log, const QString filename, const QString version)
: m_version(version)
, m_originalFilename(filename)
, m_filename(filename)
, m_pResultsType(&HResultsType::HResultsTypeOriginalRun)
, m_experimentSettings(es)
, m_runSettings(rs)
, m_subjectSettings(ss)
, m_log(log)
{};

// This constructor used for TEST RUN. The filename is the results filename the settings and log are taken from.
HResults::HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const HEventLog& log, const QString filename, const QString version)
: m_version(version)
, m_originalFilename("")
, m_filename(filename)
, m_pResultsType(&HResultsType::HResultsTypeTestRun)
, m_experimentSettings(es)
, m_runSettings(rs)
, m_log(log)
{};


HResults::~HResults() {};

// Save
bool HResults::save(const QString& filename) const
{
	bool b = false;
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QDataStream out(&file);
		out << version() << type().number();
		b = true;
		if (type() == HResultsType::HResultsTypeOriginalRun)
		{
			out << experimentSettings() << runSettings() << subjectSettings() << eventLog();
		}
		else if (type() == HResultsType::HResultsTypeTestRun)
		{
			out << experimentSettings() << runSettings() << eventLog();
		}
		else if (type() == HResultsType::HResultsTypeReliabilityRun)
		{
			qCritical() << "Saving reliability results not implemented!";
		}
		else
		{
			b = false;
		}
		file.close();
	}
	return b;
}


HResults* HResults::load(const QString& filename)
{
	QFile file(filename);
	QString version;
	QString originalFilename;
	int itype;
	Habit::ExperimentSettings es;
	Habit::RunSettings rs;
	Habit::SubjectSettings ss;
	// 2-24-15 djs Habit::ReliabilitySettings bs;
	HEventLog log;
	HResults* results = NULL;
	log.clear();
	if (file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Loading results from file " << filename;
		QDataStream in(&file);
		in >> version >> itype;
		if (itype == HResultsType::HResultsTypeOriginalRun.number())
		{
			in >> es >> rs >> ss >> log;
			results = new HResults(es, rs, ss, log, filename, version);
		}
		else if (itype == HResultsType::HResultsTypeTestRun.number())
		{
			in >> es >> rs >> log;
			results = new HResults(es, rs, log, filename);
		}
		else if (itype == HResultsType::HResultsTypeReliabilityRun.number())
		{
			qCritical() << "Loading reliability results not implemented!";
		}
		file.close();
	}
	else
	{
		qCritical() << "Cannot open results file " << filename;
	}
	return results;
}

bool HResults::saveToCSV(const QString& filename, bool bReplace) const
{
	bool b = false;
	QFile file(filename);

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream out(&file);
		CSV1ResultsScanner csv1(out);
		CSV2ResultsScanner csv2(out);
		if (!bReplace)
		{
			b = csv1.scan(*this);
			if (b) out << endl;
			if (b) b = csv2.scan(*this);
		}
		else
		{
			ReplaceLooksResultsDumper r1(this->experimentSettings().getHLookSettings(), csv1);
			ReplaceLooksResultsDumper r2(this->experimentSettings().getHLookSettings(), csv2);
			b = r1.scan(*this);
			if (b) out << endl;
			if (b) b = r2.scan(*this);
			if (b) out << endl;
		}
		file.close();
	}
	else
	{
		qWarning() << "HResults::saveToCSV: Cannot open file \"" << filename << "\".";
	}
	return b;
}

