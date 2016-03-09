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
						<< "TotalLook" << "TotalLookAway" << "TotalLeft" << "TotalCenter" << "TotalRight" << "Looks";


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

bool HResults::scanTrials(HTrialScanner& scanner) const
{
	bool b = false;
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
		b = false;
		return b;
	}

	scanner.init();

	// Now iterate through event log. Each trial is written to the csv file.
	QString sPhase;
	HTrialResultsRow row;
	bool bInsidePhase = false;
	bool bInsideTrial = false;
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
			row.setTrial(ptse->trialnumber() + 1);		// djs 11-2-2015 Use trial number, not index, in output CSV.
			row.setRepeat(ptse->repeatnumber());
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
			row.appendLook(ple->look(), true);
		}
		else if (e->type() == HEventType::HEventIncompleteLook)
		{
			HIncompleteLookEvent* ple = static_cast<HIncompleteLookEvent*>(e);
			row.appendLook(ple->look(), false);
		}
		else if (e->type() == HEventType::HEventLookTrans)
		{
			HLookTransEvent* plt = static_cast<HLookTransEvent*>(e);
			row.appendLookTransEvent(plt);
		}
		else if (e->type() == HEventType::HEventTrialEnd)
		{
			// set trial end type and flush
			HTrialEndEvent* pte = static_cast<HTrialEndEvent*>(e);
			row.setEndType(pte->endtype().name());
			row.setTrialEndTime(pte->timestamp());	// automatically sets total look(away) times
			row.setOrderName(sOrderName);

			scanner.trial(row);

			// clear pending stim label
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

	scanner.done();
	return true;
}



bool HResults::saveToCSV(const QString& filename) const
{
	bool b = false;
	QFile file(filename);

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream out(&file);
		CSV1ResultsDumper csv1(out);
		CSV2ResultsDumper csv2(out);
		b = scanTrials(csv1);
		if (b) out << endl;
		if (b) b = scanTrials(csv2);
		file.close();
	}
	else
	{
		qWarning() << "HResults::saveToCSV: Cannot open file \"" << filename << "\".";
	}
	return b;
}


#if 0
bool HResults::saveToCSV(const QString& filename) const
{
	bool b = false;
	QFile file(filename);
	QString subjectId;
	QMap<int, QString> mapStimIdNames;

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream out(&file);
		QString sAllLooking;
		QTextStream outAllLooking(&sAllLooking);

		// put some headers in for all looking
		outAllLooking << HTrialResultsRow::headers.at(0);
		for (int i=1; i<6; i++)
		{
			outAllLooking << "," << HTrialResultsRow::headers.at(i);
		}
		outAllLooking << "," << "Direction,Start,End,Diff" << endl;

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
				row.setTrial(ptse->trialnumber() + 1);		// djs 11-2-2015 Use trial number, not index, in output CSV.
				row.setRepeat(ptse->repeatnumber());
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
			else if (e->type() == HEventType::HEventLookTrans)
			{
				HLookTransEvent* plt = static_cast<HLookTransEvent*>(e);
				row.appendLookTrans(plt->transtype(), plt->timestamp());
			}
			else if (e->type() == HEventType::HEventTrialEnd)
			{
				// set trial end type and flush
				HTrialEndEvent* pte = static_cast<HTrialEndEvent*>(e);
				row.setEndType(pte->endtype().name());
				row.setTrialEndTime(pte->timestamp());	// automatically sets total look(away) times
				row.setOrderName(sOrderName);
				out << row;

				// Dump all looking into outAllLooking
				row.writeAllLooking(outAllLooking);

				// clear pending stim label
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

		// Now dump all looking into the file
		out << endl << endl << sAllLooking;

		file.close();
	}
	return b;
}
#endif

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
	return out;
}


void HTrialResultsRow::writeAllLooking(QTextStream& out) const
{

	out << this->at(0);
	for (int i=1; i<6; i++)
	{
		out << "," << this->at(i);
	}
	// now dump all looking
	QListIterator<HLook> looks(m_allLooking);
	while (looks.hasNext())
	{
		HLook l = looks.next();
		out << "," << l.direction().name() << "," << l.startMS() << "," << l.endMS() << "," << l.lookMS();
	}

	out << endl;
}

void HTrialResultsRow::appendLookTransEvent(HLookTransEvent* pevent)
{
	m_listLookTrans.append(pevent);
}


void HTrialResultsRow::appendLook(HLook look, bool bComplete)
{
	m_allLooking.append(look);

	if (bComplete)
	{
		// DJS - Do not add a fake LookAway here. It incorrectly implies that there was no looking during this period.
		// If there were incomplete looks they would not be accounted for.
//		// assume that this is a look; check for look-away gap
//		if (look.startMS() > m_lastLookEndTime)
//		{
//			HLook lookaway(HLookDirection::LookAway, m_lastLookEndTime, look.startMS());
//			m_looks.append(lookaway);
//			m_totalLookAwayTime += lookaway.lookMS();
//		}
		m_looks.append(look);
		m_totalLookTime += look.lookMS();
		m_lastLookEndTime = look.endMS();
		if (look.direction() == HLookDirection::LookLeft) m_totalLookLeftTime += look.lookMS();
		else if (look.direction() == HLookDirection::LookCenter) m_totalLookCenterTime += look.lookMS();
		else if (look.direction() == HLookDirection::LookRight) m_totalLookRightTime += look.lookMS();
		else
		{
			qDebug() << "HTrialResultsRow - cannot tally look for direction " << look.direction().name();
		}
	}
}

bool CSV1ResultsDumper::init()
{
	// put headers in output file
	out() << HTrialResultsRow::headers.join(",") << endl;
	return true;
}

bool CSV1ResultsDumper::trial(const HTrialResultsRow& row)
{
	out() << row << endl;
	return true;
}

bool CSV1ResultsDumper::done()
{
	return true;
}

bool CSV2ResultsDumper::init()
{
	out() << HTrialResultsRow::headers.at(0);
	for (int i=1; i<6; i++)
	{
		out() << "," << HTrialResultsRow::headers.at(i);
	}
	out() << "," << "Num,Complete,Direction,Start,End,Diff" << endl;
	return true;
}

bool CSV2ResultsDumper::trial(const HTrialResultsRow& row)
{
	int num=0;
	HLook l;

	// the HLooks in all_looks() will have incomplete looks - these will be a single looking period, with no sublooks.
	// There will also be complete looks - these will have at least one sublook.
	foreach (l, row.all_looks()) {
		num++;
		if (l.sublooks().size() == 0)
		{
			out() << row.at(0);
			for (int i=1; i<6; i++)
			{
				out() << "," << row.at(i);
			}
			out() << "," << num << ",FALSE," << l.direction().name() << "," << l.startMS() << "," << l.endMS() << "," << l.lookMS() << endl;
		}
		else
		{
			HLook sub;
			foreach (sub, l.sublooks()) {
				if (sub.direction().isToStim())
				{
					out() << row.at(0);
					for (int i=1; i<6; i++)
					{
						out() << "," << row.at(i);
					}
					out() << "," << num << ",TRUE," << sub.direction().name() << "," << sub.startMS() << "," << sub.endMS() << "," << sub.lookMS() << endl;
				}
			}
		}
	}
	return true;
}

bool CSV2ResultsDumper::done()
{
	return true;
}
