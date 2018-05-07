#include "runsettings.h"
#include "HTypes.h"
#include <QDebug>
#include <QStringList>
#include <QRegExp>

using namespace Habit;

const QString f_sVersion("rs2000021");

RunSettings::RunSettings()
	: id_(-1)
	, experimentId_(-1)
	, subjectId_(-1)
{}

RunSettings::~RunSettings()
{
}

int RunSettings::getId() const
{
	return id_;
}

void RunSettings::setId(int id)
{
	id_ = id;
}

int RunSettings::getExperimentId() const
{
	return experimentId_;
}

void RunSettings::setExperimentId(int id)
{
	experimentId_ = id;
}

int RunSettings::getSubjectId() const
{
	return subjectId_;
}

void RunSettings::setSubjectId(int id)
{
	subjectId_ = id;
}

void RunSettings::insert(int seqno, const PhaseRunSettings& prs)
{
	phaseRunSettingsMap_.insert(seqno, prs);
}

QDataStream & Habit::operator << (QDataStream& stream, const Habit::PhaseRunSettings& prs)
{
	stream << prs.getOrderName() << prs.getOrderList() << prs.isOrderRandomized() << prs.getRandomizeMethod();
	return stream;
}

QDataStream & Habit::operator<<(QDataStream& stream, Habit::RunSettings settings)
{
	stream << f_sVersion << settings.getId() << settings.getExperimentId() << settings.getSubjectId() << settings.map();

/* original
	stream << settings.getId();
	stream << settings.getExperimentId();
	stream << settings.getSubjectId();
	stream << settings.getPretestOrderName();
	stream << settings.isPretestRandomized();
	stream << settings.getPretestRandomizeMethod();
	stream << settings.getHabituationOrderName();
	stream << settings.isHabituationRandomized();
	stream << settings.getHabituationRandomizeMethod();
	stream << settings.getTestOrderName();
	stream << settings.isTestRandomized();
	stream << settings.getTestRandomizeMethod();
	*/
	return stream;
}

QDataStream & Habit::operator >> (QDataStream& stream, Habit::RunSettings& settings)
{
	QString sVersion;
	int experimentId;
	int subjectId;
	int id;

	// Save position in stream in case this is an old version
	qint64 pos = stream.device()->pos();
	stream >> sVersion;
	if (sVersion == f_sVersion)
	{
		QMap<int, PhaseRunSettings> map;
		stream >> id; settings.setId(id);
		stream >> experimentId; settings.setExperimentId(experimentId);
		stream >> subjectId; settings.setSubjectId(subjectId);
		stream >> map; settings.setMap(map);
	}
	else
	{
		// reset stream to position before trying to read version
		stream.device()->seek(pos);

		QString orderName;
		bool isOrderRandomized;
		int randomizeMethod;
		PhaseRunSettings prs;

		stream >> id; settings.setId(id);
		stream >> experimentId; settings.setExperimentId(experimentId);
		stream >> subjectId; settings.setSubjectId(subjectId);

		stream >> orderName >> isOrderRandomized >> randomizeMethod;
		prs.setOrderName(orderName);
		prs.setOrderRandomized(isOrderRandomized);
		prs.setRandomizeMethod(randomizeMethod);
		settings.insert(HStimContext::PreTestPhase.number(), prs);

		stream >> orderName >> isOrderRandomized >> randomizeMethod;
		prs.setOrderName(orderName);
		prs.setOrderRandomized(isOrderRandomized);
		prs.setRandomizeMethod(randomizeMethod);
		settings.insert(HStimContext::HabituationPhase.number(), prs);

		stream >> orderName >> isOrderRandomized >> randomizeMethod;
		prs.setOrderName(orderName);
		prs.setOrderRandomized(isOrderRandomized);
		prs.setRandomizeMethod(randomizeMethod);
		settings.insert(HStimContext::TestPhase.number(), prs);
	}
	return stream;
}

QDataStream& Habit::operator>>(QDataStream& stream, Habit::PhaseRunSettings& prs)
{
	QString orderName;
	StimLabelList sllist;
	bool isRandomized;
	int randomizeMethod;
	stream >> orderName >> sllist >> isRandomized >> randomizeMethod;
	prs.setOrderName(orderName);
	prs.setOrderList(sllist);
	prs.setOrderRandomized(isRandomized);
	prs.setRandomizeMethod(randomizeMethod);
	return stream;
}

bool Habit::operator==(const Habit::PhaseRunSettings& lhs, const Habit::PhaseRunSettings& rhs)
{
	return	lhs.getOrderName() == rhs.getOrderName() &&
			lhs.getOrderList() == rhs.getOrderList() &&
			lhs.isOrderRandomized() == rhs.isOrderRandomized() &&
			lhs.getRandomizeMethod() == rhs.getRandomizeMethod();
}

bool Habit::operator==(const Habit::RunSettings& lhs, const Habit::RunSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getExperimentId() == rhs.getExperimentId() &&
			lhs.getSubjectId() == rhs.getSubjectId() &&
			lhs.map() == rhs.map());
}

