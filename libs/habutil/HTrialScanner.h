/*
 * HTrialScanner.h
 *
 *  Created on: Mar 10, 2016
 *      Author: Oakeslab
 */

#ifndef HTRIALSCANNER_H_
#define HTRIALSCANNER_H_

#include "HTrialResult.h"
#include "HLookSettings.h"
#include "HResults.h"

class HTrialScanner
{
public:
	HTrialScanner() {};
	HTrialScanner(const HTrialScanner& s) {Q_UNUSED(s)};
	virtual ~HTrialScanner() {};
	virtual bool init() const = 0;
	virtual bool trial(const HTrialResult& t) const = 0;
	virtual bool done() const = 0;
	virtual bool scan(const HResults& results, QTextStream& output);
};

class NoopResultsScanner: public HTrialScanner
{
public:
	NoopResultsScanner(): HTrialScanner() {};
	virtual ~NoopResultsScanner() {};
	virtual bool init() const { return true; };
	virtual bool trial(const HTrialResult&) const { return true; };
	virtual bool done() const { return true; };
};

class HTextResultsScanner: public HTrialScanner
{
	QTextStream& m_out;
	public:
	HTextResultsScanner(QTextStream& out) : HTrialScanner(), m_out(out) {};
	HTextResultsScanner(const HTextResultsScanner& d) : HTrialScanner(), m_out(d.out()) {};
	virtual ~HTextResultsScanner() {};
	QTextStream& out() const { return m_out; };
};

class CSV1ResultsScanner: public HTextResultsScanner
{
	public:
	CSV1ResultsScanner(QTextStream& out) : HTextResultsScanner(out) {};
	CSV1ResultsScanner(const CSV1ResultsScanner& d) : HTextResultsScanner(d.out()) {};
	virtual ~CSV1ResultsScanner() {};
	virtual bool init() const;
	virtual bool trial(const HTrialResult& t) const;
	virtual bool done() const;
};

class CSV2ResultsScanner: public HTextResultsScanner
{
	public:
	CSV2ResultsScanner(QTextStream& out) : HTextResultsScanner(out) {};
	CSV2ResultsScanner(const CSV2ResultsScanner& d) : HTextResultsScanner(d.out()) {};
	virtual ~CSV2ResultsScanner() {};
	virtual bool init() const;
	virtual bool trial(const HTrialResult& t) const;
	virtual bool done() const;
};

class ReplaceLooksResultsDumper: public HTextResultsScanner
{
private:
	const HTextResultsScanner& m_dumper;
	Habit::HLookSettings m_lookSettings;
public:
	ReplaceLooksResultsDumper(const Habit::HLookSettings& settings, const HTextResultsScanner& dumper);
	virtual ~ReplaceLooksResultsDumper() {};
	virtual bool init() const;
	virtual bool trial(const HTrialResult& t) const;
	virtual bool done() const;
};

#endif /* HTRIALSCANNER_H_ */
