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


bool HResults::saveToCSV(const QString& filename) const
{
	bool b = false;
	QFile file(filename);
	QString subjectId;

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream out(&file);
		//out << version() << type().number();
		b = true;
		if (type() == HResultsType::HResultsTypeOriginalRun)
		{
			subjectId = QString("%1").arg(subjectSettings().getId());
			qDebug() << "Original run";
		}
		else if (type() == HResultsType::HResultsTypeTestRun)
		{
			subjectId = QString("TESTING");
		}
		else if (type() == HResultsType::HResultsTypeReliabilityRun)
		{
			subjectId = QString("R-%1").arg(subjectSettings().getId());
		}
		else
		{
			qCritical() << "Unknown results type. Cannot save HResults to CSV.";
			b = false;
			return b;
		}

		// Now iterate through event log. Each successful trial is written to the csv file.

		QString sPhase;
		HTrialResultsRow row;
		bool bInsidePhase = false;
		bool bInsideTrial = false;
		int totalLookTime = 0;
		int totalLookAwayTime = 0;
		bool bHabituated = false;
		QListIterator<HEvent*> events(this->eventLog());

		while (events.hasNext())
		{
			HEvent* e = events.next();
			if (e->type() == HEventType::HEventPhaseStart)
			{
				HPhaseStartEvent* pse = static_cast<HPhaseStartEvent*>(e);
				sPhase = pse->phase();
				if (bInsidePhase) qCritical("Found phase start without preceding phase end event!");
				bInsidePhase = true;
			}
			else if (e->type() == HEventType::HEventTrialStart)
			{
				HTrialStartEvent* ptse = static_cast<HTrialStartEvent*>(e);
				if (bInsideTrial) qCritical("Found trial start without preceding trial end event!");
				qDebug() << "Init Trial start subjectID " << subjectId;
				row.init(subjectId);
				row.setPhase(sPhase);
				if (bHabituated)
					row.setHabituated(QString("Yes"));
				else
					row.setHabituated(QString("No"));
				row.setTrial(QString("%1").arg(ptse->trialnumber()));
				row.setRepeat(QString("%1").arg(ptse->repeatnumber()));
				totalLookTime = 0;
				totalLookAwayTime = 0;
			}
			else if (e->type() == HEventType::HEventStimStart)
			{
				HStimStartEvent* psse = static_cast<HStimStartEvent*>(e);
				row.setStimId(QString("%1").arg(psse->stimid()));
			}
			else if (e->type() == HEventType::HEventScreenStart)
			{
				HScreenStartEvent* psse = static_cast<HScreenStartEvent*>(e);

				// TODO - must have a way to map monitor id numbers to left/center/right/iss!
				switch(psse->playerid())
				{
				case 0:
				{
					row.setStimISS(psse->filename());
					break;
				}
				case 1:
				{
					row.setStimLeft(psse->filename());
					break;
				}
				case 2:
				{
					row.setStimRight(psse->filename());
					break;
				}
				case 3:
				{
					row.setStimCenter(psse->filename());
					break;
				}
				default:
				{
					qWarning() << "Warning: cannot match player id to monitor.";
					break;
				}
				}
			}
			else if (e->type() == HEventType::HEventLook)
			{
				HLookEvent* ple = static_cast<HLookEvent*>(e);
				row.appendLook(ple->look().direction().name(), QString("%1").arg(ple->look().lookMS()));
				if (ple->look().direction() == HLookDirection::LookAway)
				{
					totalLookAwayTime += ple->look().lookMS();
				}
				if (ple->look().direction() == HLookDirection::LookLeft)
				{
					totalLookTime += ple->look().lookMS();
				}
				if (ple->look().direction() == HLookDirection::LookCenter)
				{
					totalLookTime += ple->look().lookMS();
				}
				if (ple->look().direction() == HLookDirection::LookRight)
				{
					totalLookTime += ple->look().lookMS();
				}
			}
			else if (e->type() == HEventType::HEventTrialEnd)
			{
				// check that the trial ended successfully
				HTrialEndEvent* pte = static_cast<HTrialEndEvent*>(e);
				if (pte->endtype() == HTrialEndType::HTrialEndMaxStimulusTime ||
						pte->endtype() == HTrialEndType::HTrialEndGotLook ||
						pte->endtype() == HTrialEndType::HTrialEndMaxStimulusTime ||
						pte->endtype() == HTrialEndType::HTrialEndMaxLookAwayTime)
				{
					// good trial, flush it to output
					row.setTotalLook(QString("%1").arg(totalLookTime));
					row.setTotalLookAway(QString("%1").arg(totalLookAwayTime));
					out << row;
				}
			}

		}


		file.close();
	}
	return b;
}


QTextStream& operator<<(QTextStream& out, const HTrialResultsRow& row)
{
	QListIterator<QString> items(row);
	out << items.next();
	while (items.hasNext())
	{
		out << "," << items.next();
	}
	out << endl;
	return out;
}

