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


// This constructor used for RELIABILTY RUN

/*
 * 2-24-15 djs Remove reliability settings for now, clearing out unused code. Will probably need to add this back soon. Cleaner, I hope.
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
*/

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
			//in >> originalFilename >> es >> rs >> bs >> log;
			//results = new HResults(es, rs, bs, log, originalFilename, filename);
		}
		file.close();
	}
	else
	{
		qCritical() << "Cannot open results file " << filename;
	}
	return results;
}

bool HResults::scanTrials(const HTrialScanner& scanner) const
{
	bool b = true;
	QString subjectId;
	QMap<int, QString> mapStimIdNames;

	b = true;
	if (type() == HResultsType::HResultsTypeOriginalRun)
	{
		subjectId = subjectSettings().getSubjectName();
	}
	else if (type() == HResultsType::HResultsTypeTestRun)
	{
		subjectId = QString("TESTING");
	}
	else if (type() == HResultsType::HResultsTypeReliabilityRun)
	{
		subjectId = QString("R-%1").arg(subjectSettings().getSubjectName());
	}
	else
	{
		qCritical() << "Unknown results type. Cannot save HResults to CSV.";
		return false;
	}

	b = scanner.init();


	if (b)
	{
		QString sPhase;
		HTrialResult trialResult;
		bool bInsidePhase = false;
		bool bInsideTrial = false;
		bool bHabituated = false;
		bool bHaveStimRequest = false;
		QString sPendingStimLabel;
		QString sOrderName;
		HEvent* e;

		foreach(e, this->eventLog())
		{

			if (e->type() == HEventType::HEventPhaseStart)
			{
				HPhaseStartEvent* pse = static_cast<HPhaseStartEvent*>(e);
				sPhase = pse->phase();
				sOrderName = "";
				if (runSettings().map().contains(pse->seqno()))
				{
					Habit::PhaseRunSettings prs = runSettings().map().value(pse->seqno());
					sOrderName = prs.getOrderName();
					if (sOrderName.isEmpty()) sOrderName = "default";

					if (prs.isOrderRandomized())
					{
						sOrderName += " (" + getRandomizationType(prs.getRandomizeMethod()).name() + ")";
					}
					else
					{
						sOrderName += " (none)";
					}

				}
				else
				{
					sOrderName = "unknown (unknown)";
				}

				if (bInsidePhase) qCritical("Found phase start without preceding phase end event!");
				bInsidePhase = true;
			}
			else if (e->type() == HEventType::HEventPhaseEnd)
			{
				bInsidePhase = false;
			}
			else if (e->type() == HEventType::HEventTrialStart)
			{
				HTrialStartEvent* ptse = static_cast<HTrialStartEvent*>(e);
				bHaveStimRequest = false;
				if (bInsideTrial) qCritical("Found trial start without preceding trial end event!");

				// initialize trialResult
				trialResult = HTrialResult(HTrialKey(sPhase, ptse->trialnumber()+1, ptse->repeatnumber()));

				// throw in
				trialResult.setSubjectID(subjectId);
				if (bHabituated)
					trialResult.setHabituated(true);
				else
					trialResult.setHabituated(false);
				trialResult.setTrialStartTime(ptse->timestamp());
			}
			else if (e->type() == HEventType::HEventStimRequest)
			{
				bHaveStimRequest = true;
				sPendingStimLabel = "";
			}
			else if (e->type() == HEventType::HEventStimLabelRequest)
			{
				HStimLabelRequestEvent* pslre = static_cast<HStimLabelRequestEvent*>(e);
				bHaveStimRequest = true;
				sPendingStimLabel = pslre->label();
			}
			else if (e->type() == HEventType::HEventStimStart)
			{
				HStimStartEvent* psse = static_cast<HStimStartEvent*>(e);
				trialResult.setStimId(psse->stimid());
				trialResult.setStimName(mapStimIdNames.value(psse->stimid()));
				trialResult.setStimLabel(sPendingStimLabel);
			}
			else if (e->type() == HEventType::HEventScreenStart)
			{
				HScreenStartEvent* psse = static_cast<HScreenStartEvent*>(e);
				const HPlayerPositionType& ppt = getPlayerPositionType(psse->playerid());
				// Do not keep filename unless stim request has been seen. Otherwise we will
				// record attention getter file(s).
				if (bHaveStimRequest)
				{
					if (ppt == HPlayerPositionType::Sound)
					{
						trialResult.setStimISS(psse->filename());
					}
					else if (ppt == HPlayerPositionType::Left)
					{
						trialResult.setStimLeft(psse->filename());
					}
					else if (ppt == HPlayerPositionType::Right)
					{
						trialResult.setStimRight(psse->filename());
					}
					else if (ppt == HPlayerPositionType::Center)
					{
						trialResult.setStimCenter(psse->filename());
					}
					else
					{
						qWarning() << "Warning: cannot match player id to monitor.";
					}
				}
			}
			else if (e->type() == HEventType::HEventLook)
			{
				HLookEvent* ple = static_cast<HLookEvent*>(e);
				trialResult.appendLook(ple->look());
			}
			else if (e->type() == HEventType::HEventIncompleteLook)
			{
				HIncompleteLookEvent* ple = static_cast<HIncompleteLookEvent*>(e);
				trialResult.appendLook(ple->look());
			}
			else if (e->type() == HEventType::HEventLookTrans)
			{
				trialResult.appendEvent(e);
			}
			else if (e->type() == HEventType::HEventLookEnabled)
			{
				HLookEnabledEvent* ple = static_cast<HLookEnabledEvent*>(e);
				trialResult.setLookEnabledTime(ple->timestamp());
				trialResult.appendEvent(e);
			}
			else if (e->type() == HEventType::HEventLookDisabled)
			{
				HLookDisabledEvent* pld = static_cast<HLookDisabledEvent*>(e);
				trialResult.setLookDisabledTime(pld->timestamp());
				trialResult.appendEvent(e);
			}
			else if (e->type() == HEventType::HEventTrialEnd)
			{
				// set trial end type and flush
				HTrialEndEvent* pte = static_cast<HTrialEndEvent*>(e);
				trialResult.setEndType(pte->endtype().name());
				trialResult.setTrialEndTime(pte->timestamp());	// automatically sets total look(away) times
				trialResult.setOrderName(sOrderName);

				scanner.trial(trialResult);

				// clear pending stim label
				sPendingStimLabel = "";
			}
			else if (e->type() == HEventType::HEventStimulusSettings)
			{
				HStimulusSettingsEvent* pss = static_cast<HStimulusSettingsEvent*>(e);
				mapStimIdNames.insert(pss->stimindex(), pss->settings().getName());
			}
			else if (e->type() == HEventType::HHabituationSuccess)
			{
				bHabituated = true;
			}
		}
	}
	if (b) b = scanner.done();
	return b;
}


bool HResults::saveToCSV(const QString& filename, bool bReplace) const
{
	bool b = false;
	QFile file(filename);

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream out(&file);
		CSV1ResultsDumper csv1(out);
		CSV2ResultsDumper csv2(out);
		if (!bReplace)
		{
			b = scanTrials(csv1);
			if (b) out << endl;
			if (b) b = scanTrials(csv2);
		}
		else
		{
			ReplaceLooksResultsDumper r1(this->experimentSettings().getHLookSettings(), csv1);
			ReplaceLooksResultsDumper r2(this->experimentSettings().getHLookSettings(), csv2);
			b = scanTrials(r1);
			if (b) out << endl;
			if (b) b = scanTrials(r2);
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

