/*
 * HStimulusSettingsOrderImportUtil.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: Oakeslab
 */

#include "HStimulusSettingsOrderImportUtil.h"
#include <QFile>
#include <QRegExp>
#include <QTextStream>

using namespace Habit;

static const QString sreDELIM("(,|\t)");		// comma or tab delimited
static const QString sreMAYBEDELIM("(,|\t)?");
static const QString srePAD("[ ]*");			// spaces padding words are removed
static const QString sreNAME("([a-zA-Z0-9-_]+)");	// stim and order names
static const QString srePHASE("(pretest|habituation|test)");
static const QString sreSTOR("(stim|order)");
// static const QString srePATH("((/)?([^/\\0\\t,]+(/)?)+)");	// path does not allow tabs or commas in filenames/path!
static const QString srePATH("([/]?([^/\\0\\t,]+[/]?)+)");	// path does not allow tabs or commas in filenames/path!
static const QString sreNUM("([0-9]+)");
static const QString srePOSITION("(left|right|center|sound)");
static const QString sreTAIL("(.*)$");
static const QString sreHEAD("^");



bool checkPhase(const QString& s, const HStimContext& context)
{
	return 	(context == HStimContext::PreTestPhase && s == "pretest") ||
			(context == HStimContext::HabituationPhase && s == "habituation") ||
			(context == HStimContext::TestPhase && s == "test");
}


bool parseOrder(const QString& s, HStimulusOrder& order)
{
	bool b = true;
	QRegExp reDELIM(sreDELIM);
	QRegExp rePADDEDNAME(srePAD + sreNAME + srePAD);
	QRegExp reNAME(sreNAME);

	// split at delimiter
	QStringList l = s.split(reDELIM);

	// remove padding from each string
	l.replaceInStrings(rePADDEDNAME, "\\1");

	// now remove empty strings
	l = l.filter(reNAME);

	// remainder goes in the order itself
	order.setList(l);

	return b;
}

bool parseStim(const QString& s, StimulusSettings& settings)
{
	bool b = false;
	//maybe not QString sreMAYBE("(" + sreDELIM + srePAD + sreNUM + srePAD + sreDELIM + srePAD + sreNUM + srePAD + ")?");
	//QRegExp reSTIMINFO(sreHEAD + sreMAYBEDELIM + srePAD + srePOSITION + srePAD + sreDELIM + srePAD + srePATH + srePAD);
	QRegExp reSTIMINFO(srePAD + srePOSITION + srePAD + sreDELIM + srePAD + srePATH + srePAD + sreMAYBEDELIM);
	//                                        1                      2                   3,4                   5

	// [ ]*(left|right|center|sound)[ ]*
	// (,|	)
	// [ ]*((/)?([^/[ ]*(,|	)([ ]*([0-9]+)[ ]*(,|	)[ ]*([0-9]+)[ ]*)?
	int pos = 0;
	int lastpos = 0;

//	qDebug() << "parseStim: " << s;
//	qDebug() << "start at 0";

	// Each tie through the loop, verify that the match didn't skip over any chars.

	while ((pos = reSTIMINFO.indexIn(s, pos)) != -1)
	{
		if (pos-lastpos == 0)
		{
			StimulusInfo info;
			info.setFileName(reSTIMINFO.cap(3));
			info.setVolume(0);
			info.setLoopPlayBack(false);
			if (reSTIMINFO.cap(1) == "left")
				settings.setLeftStimulusInfo(info);
			else if (reSTIMINFO.cap(1) == "center")
				settings.setCenterStimulusInfo(info);
			else if (reSTIMINFO.cap(1) == "right")
				settings.setRightStimulusInfo(info);
			else if (reSTIMINFO.cap(1) == "sound")
				settings.setIndependentSoundInfo(info);
//			qDebug() << "STIMINFO pos " << pos << " POSITION " << reSTIMINFO.cap(1) << " PATH " << reSTIMINFO.cap(3) << " MAYBEDELIM " << (reSTIMINFO.cap(5).isEmpty() ? "NONE" : QString("LEN=%1").arg(reSTIMINFO.cap(5).size()));
			pos += reSTIMINFO.matchedLength();
//			if (pos < s.length()) qDebug() << "next char is " << (int)((s[pos].toAscii()));

//			qDebug() << "start at " << pos << " :" << s.right(s.size() - pos);
			lastpos = pos;
			b = true;	// if one matches, OK
		}
		else
		{
			qWarning() << "Bad line in import. Extra chars near position " << lastpos;
		}
	}
	return b;
}


bool importStimulusSettingsAndOrders(const QString& filename, Habit::StimulusSettingsList& stimList, Habit::HStimulusOrderList& orderList, const HStimContext& context)
{
	bool b = false;
	int skipped=0;
	int comment=0;
	//int nstim=0;
	//int norder=0;
	QFile file(filename);
	if (file.exists() && file.open(QIODevice::ReadOnly)) // | QIODevice::Text))
	{
		QTextStream in(&file);
		QRegExp reFULL(sreHEAD + srePAD + sreSTOR + srePAD + sreDELIM + srePAD + srePHASE + srePAD + sreDELIM + srePAD + sreNAME + srePAD + sreDELIM + sreTAIL);
		//                0         0        1                   2                3                    4                   5                    6         7
		while (!in.atEnd())
		{
			QString line = in.readLine().trimmed();

			// comment, blank, stim or order?
			if (line.isEmpty())
				skipped++;
			else if (line.at(0) == '#')
				comment++;
			else
			{
				if (reFULL.indexIn(line) == 0)
				{
					// check phase vs context for this
					if (checkPhase(reFULL.cap(3), context))
					{
						// correct phase
						if (reFULL.cap(1) == "stim")
						{
							Habit::StimulusSettings stim(reFULL.cap(5), context);
							if (parseStim(reFULL.cap(7), stim))
							{
								qDebug() << "parseStim got " << stim;
								stimList.append(stim);
							}
							else
							{
								qDebug() << "Cannot parse stimulus: " << reFULL.cap(7);
							}
						}
						else
						{
							HStimulusOrder order(context, reFULL.cap(5));
							if (parseOrder(reFULL.cap(7), order))
							{
								orderList.append(order);
							}
							else
							{
								qDebug() << "Cannot parse stim order: " << reFULL.cap(7);
							}
						}
					}
					else
					{
						qDebug() << "Ignoring import line for phase " << reFULL.cap(3);
					}
				}
				else
					qDebug() << "unmatched line " << line.trimmed();
			}
		}
		file.close();
		b = true;	// return true if file was opened ok.
	}
	return b;
}