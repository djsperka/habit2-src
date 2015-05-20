/*
 * HResults.cpp
 *
 *  Created on: May 9, 2013
 *      Author: Oakeslab
 */

#include "HResults.h"
#include "HLook.h"
#include <QFile>
#include <QDataStream>
#include <QCoreApplication>
#include <QtDebug>


// This should be kept in sync with the enum defined in HTrialResultsRow. Pain but I can't think of a better way now.
QStringList HTrialResultsRow::headers =
		QStringList() 	<< "SubjectID" << "Phase" << "Order" << "Trial" << "Repeat" << "EndType"
						<< "Habituated" << "StimID" << "StimName" << "StimLabel"
						<< "Left" << "Center" << "Right" << "ISS"
						<< "Trial Start" << "Trial End"
						<< "TotalLook" << "TotalLookAway" << "Looks";


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
			//out << originalFilename();
			//out << experimentSettings() << runSettings() << reliabilitySettings() << eventLog();
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


bool HResults::saveToCSV(const QString& filename) const
{
	bool b = false;
	QFile file(filename);
	QString subjectId;
	QMap<int, QString> mapStimIdNames;

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream out(&file);
		//out << version() << type().number();
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
			b = false;
			return b;
		}

		// put headers in output file
		out << HTrialResultsRow::headers.join(",") << endl;

		// Now iterate through event log. Each trial is written to the csv file.
		QString sPhase;
		HTrialResultsRow row;
		bool bInsidePhase = false;
		bool bInsideTrial = false;
		int totalLookTime = 0;
		int totalLookAwayTime = 0;
		bool bHabituated = false;
		bool bHaveStimRequest = false;
		QString sPendingStimLabel;
		QString sOrderName;
		QListIterator<HEvent*> events(this->eventLog());

		while (events.hasNext())
		{
			HEvent* e = events.next();
			if (e->type() == HEventType::HEventPhaseStart)
			{
				HPhaseStartEvent* pse = static_cast<HPhaseStartEvent*>(e);
				sPhase = pse->phase();

				if (pse->phasetype() == HPhaseType::PreTest) sOrderName = runSettings().getPretestOrderName();
				else if (pse->phasetype() == HPhaseType::Habituation) sOrderName = runSettings().getHabituationOrderName();
				else if (pse->phasetype() == HPhaseType::Test) sOrderName = runSettings().getTestOrderName();
				else sOrderName="";

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
				row.init(subjectId);
				row.setPhase(sPhase);
				if (bHabituated)
					row.setHabituated(QString("Yes"));
				else
					row.setHabituated(QString("No"));
				row.setTrialStartTime(ptse->timestamp());
				row.setTrial(ptse->trialnumber());
				row.setRepeat(ptse->repeatnumber());
				totalLookTime = 0;
				totalLookAwayTime = 0;
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
				row.setStimId(psse->stimid());
				row.setStimName(mapStimIdNames.value(psse->stimid()));
				row.setStimLabel(sPendingStimLabel);
			}
			else if (e->type() == HEventType::HEventScreenStart)
			{
				HScreenStartEvent* psse = static_cast<HScreenStartEvent*>(e);

				// TODO - must have a way to map monitor id numbers to left/center/right/iss!

				// Do not keep filename unless stim request has been seen. Otherwise we will
				// record attention getter file(s).
				if (bHaveStimRequest)
				{
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
			}
			else if (e->type() == HEventType::HEventLook)
			{
				HLookEvent* ple = static_cast<HLookEvent*>(e);
				row.appendLook(ple->look());
			}
			else if (e->type() == HEventType::HEventTrialEnd)
			{
				// set trial end type and flush
				HTrialEndEvent* pte = static_cast<HTrialEndEvent*>(e);
				row.setEndType(pte->endtype().name());
				row.setTotalLook(QString("%1").arg(totalLookTime));
				row.setTotalLookAway(QString("%1").arg(totalLookAwayTime));
				row.setTrialEndTime(pte->timestamp());	// automatically sets total look(away) times
				row.setOrderName(sOrderName);
				out << row;
				sPendingStimLabel = "";
			}
			else if (e->type() == HEventType::HEventStimulusSettings)
			{
				// Save stim name
				HStimulusSettingsEvent* pss = static_cast<HStimulusSettingsEvent*>(e);
				mapStimIdNames.insert(pss->stimindex(), pss->settings().getName());
				//qDebug() << "HStimulusSettingsEvent " << pss->stimindex() << " : " << pss->settings().getName();
			}
			else if (e->type() == HEventType::HHabituationSuccess)
			{
				bHabituated = true;
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

	// now dump looks
	QListIterator<HLook> looks(row.looks());
	while (looks.hasNext())
	{
		HLook l = looks.next();
		out << "," << l.direction().name() << "," << l.startMS() << "," << l.endMS() << "," << l.lookMS();
	}
	out << endl;
	return out;
}

void HTrialResultsRow::appendLook(HLook look)
{
	// assume that this is a look; check for look-away gap
	if (look.startMS() > m_lastLookEndTime)
	{
		HLook lookaway(HLookDirection::LookAway, m_lastLookEndTime, look.startMS());
		m_looks.append(lookaway);
		m_totalLookAwayTime += lookaway.lookMS();
	}
	m_looks.append(look);
	m_totalLookTime += look.lookMS();
	m_lastLookEndTime = look.endMS();

}
