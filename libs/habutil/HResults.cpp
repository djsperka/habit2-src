/*
 * HResults.cpp
 *
 *  Created on: May 9, 2013
 *      Author: Oakeslab
 */

#include "HResults.h"
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
, m_reliabilitySettings(r.reliabilitySettings())
, m_log(r.eventLog())
{};


// This constructor used for ORIGINAL RUN. The filename is the results filename the settings and log are taken from.
HResults::HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::SubjectSettings& ss, const HEventLog& log, const QString filename, const QString version)
: m_version(version)
, m_originalFilename("NOT_SAVED")
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


// This constructor used for RELIABILTY RUN
HResults::HResults(const Habit::ExperimentSettings& es, const Habit::RunSettings& rs, const Habit::ReliabilitySettings& bs, const HEventLog& log, const QString origFilename, const QString filename, const QString version)
: m_version(version)
, m_originalFilename(origFilename)
, m_filename(filename)
, m_pResultsType(&HResultsType::HResultsTypeReliabilityRun)
, m_experimentSettings(es)
, m_runSettings(rs)
, m_reliabilitySettings(bs)
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
			out << originalFilename();
			out << experimentSettings() << runSettings() << reliabilitySettings() << eventLog();
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
	Habit::ReliabilitySettings bs;
	HEventLog log;
	HResults* results = NULL;
	log.clear();
	if (file.open(QIODevice::ReadOnly))
	{
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
			in >> originalFilename >> es >> rs >> bs >> log;
			results = new HResults(es, rs, bs, log, originalFilename, filename);
		}
		file.close();
	}
	return results;
}

