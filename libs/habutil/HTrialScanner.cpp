/*
 * HTrialScanner.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: Oakeslab
 */

#include "HTrialScanner.h"
#include "HLookerReprocessor.h"

bool CSV1ResultsDumper::init() const
{
	// put headers in output file
	out() << HTrialResult::headers.join(",") << endl;
	return true;
}

bool CSV1ResultsDumper::trial(const HTrialResult& row) const
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
	out() << endl;
	return true;
}

bool CSV1ResultsDumper::done() const
{
	return true;
}

bool CSV2ResultsDumper::init() const
{
	out() << HTrialResult::headers.at(HTrialResult::indPhase) << ","
			<< HTrialResult::headers.at(HTrialResult::indTrial) << ","
			<< HTrialResult::headers.at(HTrialResult::indRepeat) << ","
			<< "Num,Complete,Direction,Start,End,Diff" << endl;
	return true;
}

bool CSV2ResultsDumper::trial(const HTrialResult& row) const
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
					<< num << ",FALSE," << l.direction().name() << "," << l.startMS() << "," << l.endMS() << "," << l.lookMS() << endl;
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
							<< num << ",TRUE," << sub.direction().name() << "," << sub.startMS() << "," << sub.endMS() << "," << sub.lookMS() << endl;
				}
			}
		}
	}
	return true;
}

bool CSV2ResultsDumper::done() const
{
	return true;
}

ReplaceLooksResultsDumper::ReplaceLooksResultsDumper(const Habit::HLookSettings& settings, const HTextResultsDumper& dumper)
: HTextResultsDumper(dumper.out())
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



