/*
 * HTrialScanner.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: Oakeslab
 */

#include "HTrialScanner.h"
#include "HLookerReprocessor.h"

bool HTrialScanner::scan(const HResults& results, QTextStream& output)
{
	bool b = true;
	QString subjectId;
	QMap<int, QString> mapStimIdNames;

	b = true;
	if (results.type() == HResultsType::HResultsTypeOriginalRun)
	{
		subjectId = results.subjectSettings().getSubjectName();
	}
	else if (results.type() == HResultsType::HResultsTypeTestRun)
	{
		subjectId = QString("TESTING");
	}
	else if (results.type() == HResultsType::HResultsTypeReliabilityRun)
	{
		subjectId = QString("R-%1").arg(results.subjectSettings().getSubjectName());
	}
	else
	{
		output << "Unknown results type. Cannot save HResults to CSV." << Qt::endl;
		return false;
	}

	b = this->init();

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

		foreach(e, results.eventLog())
		{

			if (e->type() == HEventType::HEventPhaseStart)
			{
				HPhaseStartEvent* pse = static_cast<HPhaseStartEvent*>(e);
				sPhase = pse->phase();
				sOrderName = "";
				if (results.runSettings().map().contains(pse->seqno()))
				{
					Habit::PhaseRunSettings prs = results.runSettings().map().value(pse->seqno());
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

				if (bInsidePhase)
				{
					qCritical("Found phase start without preceding phase end event!");
					output << "Found phase start without preceding phase end event!";
				}
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
				if (bInsideTrial)
				{
					qCritical("Found trial start without preceding trial end event!");
					output << "Found trial start without preceding trial end event!";
				}

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
						output << "Warning: cannot match player id to monitor.";
					}
				}
			}
			else if (e->type() == HEventType::HEventLook)
			{
				HLookEvent* ple = static_cast<HLookEvent*>(e);
				if (trialResult.looks().contains(ple->look()))
				{
					output << "Phase: " << trialResult.getString(HTrialResult::indPhase) << " Trial: " << trialResult.getString(HTrialResult::indTrial) << Qt::endl;
					output << "   Duplicate look: " << ple->look() << Qt::endl;
					qWarning() << "Duplicate look: " << ple->look();
				}
				else
				{
					trialResult.appendLook(ple->look());
				}
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

				this->trial(trialResult);

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
	if (b) b = this->done();
	return b;
}


bool CSV1ResultsScanner::init() const
{
	// put headers in output file
	out() << HTrialResult::headers.join(",") << Qt::endl;
	return true;
}

bool CSV1ResultsScanner::trial(const HTrialResult& row) const
{
	HLook l;
	out() << row.getString(HTrialResult::indSubjectId)
			<< "," << row.getString(HTrialResult::indPhase)
			<< "," << row.getString(HTrialResult::indOrderName)
			<< "," << row.getString(HTrialResult::indTrial)
			<< "," << row.getString(HTrialResult::indRepeat)
			<< "," << row.getString(HTrialResult::indEndType)
			<< "," << row.getString(HTrialResult::indHabituated)
			<< "," << row.getString(HTrialResult::indStimId)
			<< "," << row.getString(HTrialResult::indStimName)
			<< "," << row.getString(HTrialResult::indStimLabel)
			<< "," << row.getString(HTrialResult::indStimLeft)
			<< "," << row.getString(HTrialResult::indStimCenter)
			<< "," << row.getString(HTrialResult::indStimRight)
			<< "," << row.getString(HTrialResult::indStimISS)
			<< "," << row.getString(HTrialResult::indTrialStartTime)
			<< "," << row.getString(HTrialResult::indTrialEndTime)
			<< "," << row.getString(HTrialResult::indTotalLook)
			<< "," << row.getString(HTrialResult::indTotalAway)
			<< "," << row.getString(HTrialResult::indTotalLookLeft)
			<< "," << row.getString(HTrialResult::indTotalLookCenter)
			<< "," << row.getString(HTrialResult::indTotalLookRight)
			<< "," << row.getString(HTrialResult::indLookEnabledTime)
			<< "," << row.getString(HTrialResult::indLookDisabledTime);
	foreach (l, row.looks()) {
		if (l.isComplete())
			out() << "," << l.direction().name() << "," << l.startMS() << "," << l.endMS() << "," << l.lookMS();
	}
	out() << Qt::endl;
	return true;
}

bool CSV1ResultsScanner::done() const
{
	return true;
}

bool CSV2ResultsScanner::init() const
{
	out() << HTrialResult::headers.at(HTrialResult::indPhase) << ","
			<< HTrialResult::headers.at(HTrialResult::indTrial) << ","
			<< HTrialResult::headers.at(HTrialResult::indRepeat) << ","
			<< "Num,Complete,Direction,Start,End,Diff" << Qt::endl;
	return true;
}

bool CSV2ResultsScanner::trial(const HTrialResult& row) const
{
	int num=0;
	HLook l;

	// the HLooks in looks() will have incomplete looks - these will be a single looking period, with no sublooks.
	// There will also be complete looks - these will have at least one sublook.
	foreach (l, row.looks()) {
		num++;
		if (!l.isComplete())
		{
			out() << row.getString(HTrialResult::indPhase) << ","
					<< row.getString(HTrialResult::indTrial) << ","
					<< row.getString(HTrialResult::indRepeat) << ","
					<< num << ",FALSE," << l.direction().name() << "," << l.startMS() << "," << l.endMS() << "," << l.lookMS() << Qt::endl;
		}
		else
		{
			HLook sub;
			foreach (sub, l.sublooks()) {
				if (sub.direction().isToStim())
				{
					out() << row.getString(HTrialResult::indPhase) << ","
							<< row.getString(HTrialResult::indTrial) << ","
							<< row.getString(HTrialResult::indRepeat) << ","
							<< num << ",TRUE," << sub.direction().name() << "," << sub.startMS() << "," << sub.endMS() << "," << sub.lookMS() << Qt::endl;
				}
			}
		}
	}
	return true;
}

bool CSV2ResultsScanner::done() const
{
	return true;
}

ReplaceLooksResultsDumper::ReplaceLooksResultsDumper(const Habit::HLookSettings& settings, const HTextResultsScanner& dumper)
: HTextResultsScanner(dumper.out())
, m_dumper(dumper)
, m_lookSettings(settings)
{
}

bool ReplaceLooksResultsDumper::init() const
{
	return m_dumper.init();
}

bool ReplaceLooksResultsDumper::trial(const HTrialResult& row) const
{
	HLook l;
	HLookerReprocessor r(m_lookSettings, row.events());
	HTrialResult new_row(row);
	new_row.setLooks(r.looks());
	return m_dumper.trial(new_row);
}



bool ReplaceLooksResultsDumper::done() const
{
	return m_dumper.done();
}



