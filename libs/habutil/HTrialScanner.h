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

class HTrialScanner
{
public:
	HTrialScanner() {};
	HTrialScanner(const HTrialScanner& s) {Q_UNUSED(s)};
	virtual ~HTrialScanner() {};
	virtual bool init() const = 0;
	virtual bool trial(const HTrialResult& t) const = 0;
	virtual bool done() const = 0;
};

class HTextResultsDumper: public HTrialScanner
{
	QTextStream& m_out;
	public:
	HTextResultsDumper(QTextStream& out) : HTrialScanner(), m_out(out) {};
	HTextResultsDumper(const HTextResultsDumper& d) : HTrialScanner(), m_out(d.out()) {};
	virtual ~HTextResultsDumper() {};
	QTextStream& out() const { return m_out; };
};

class CSV1ResultsDumper: public HTextResultsDumper
{
	public:
	CSV1ResultsDumper(QTextStream& out) : HTextResultsDumper(out) {};
	CSV1ResultsDumper(const CSV1ResultsDumper& d) : HTextResultsDumper(d.out()) {};
	virtual ~CSV1ResultsDumper() {};
	virtual bool init() const;
	virtual bool trial(const HTrialResult& t) const;
	virtual bool done() const;
};

class CSV2ResultsDumper: public HTextResultsDumper
{
	public:
	CSV2ResultsDumper(QTextStream& out) : HTextResultsDumper(out) {};
	CSV2ResultsDumper(const CSV2ResultsDumper& d) : HTextResultsDumper(d.out()) {};
	virtual ~CSV2ResultsDumper() {};
	virtual bool init() const;
	virtual bool trial(const HTrialResult& t) const;
	virtual bool done() const;
};

class ReplaceLooksResultsDumper: public HTextResultsDumper
{
private:
	const HTextResultsDumper& m_dumper;
	Habit::HLookSettings m_lookSettings;
public:
	ReplaceLooksResultsDumper(const Habit::HLookSettings& settings, const HTextResultsDumper& dumper);
	virtual ~ReplaceLooksResultsDumper() {};
	virtual bool init() const;
	virtual bool trial(const HTrialResult& t) const;
	virtual bool done() const;
};

#endif /* HTRIALSCANNER_H_ */
