#include "runsettings.h"
#include <QDebug>
#include <QStringList>
#include <QRegExp>

using namespace Habit;

RunSettings::RunSettings()
	: id_(-1)
	, experimentId_(-1)
	, subjectId_(-1)
	, habituationTestOrder_("")
	, isHabituationOrderRandomized_(false)
	, habituationRandomizeMethod(-1)
	, pretestTestOrder_("")
	, isPretestOrderRandomized_(false)
	, pretestRandomizeMethod(-1)
	, testTestOrder_("")
	, isTestOrderRandomized_(false)
	, testRandomizeMethod(-1)
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

QString RunSettings::getHabituationOrderName() const
{
	return habituationTestOrder_;
}

void RunSettings::setHabituationOrderName(const QString& order)
{
	habituationTestOrder_ = order;
	return;
}

bool RunSettings::isHabituationRandomized() const
{
	return isHabituationOrderRandomized_;
}

void RunSettings::setHabituationRandomized(bool t)
{
	isHabituationOrderRandomized_ = t;
}

int RunSettings::getHabituationRandomizeMethod() const
{
	return habituationRandomizeMethod;
}

void RunSettings::setHabituationRandomizeMethod(int m)
{
	habituationRandomizeMethod = m;
}

QString RunSettings::getPretestOrderName() const
{
	return pretestTestOrder_;
}

void RunSettings::setPretestOrderName(const QString& ordername)
{
	pretestTestOrder_ = ordername;
	return;
}

bool RunSettings::isPretestRandomized() const
{
	return isPretestOrderRandomized_;
}

void RunSettings::setPretestRandomized(bool t)
{
	isPretestOrderRandomized_ = t;
}

int RunSettings::getPretestRandomizeMethod() const
{
	return pretestRandomizeMethod;
}

void RunSettings::setPretestRandomizeMethod(int m)
{
	pretestRandomizeMethod = m;
}

QString RunSettings::getTestOrderName() const
{
	return testTestOrder_;
}

void RunSettings::setTestOrderName(const QString& order)
{
	testTestOrder_ = order;
}

bool RunSettings::isTestRandomized() const
{
	return isTestOrderRandomized_;
}

void RunSettings::setTestRandomized(bool t)
{
	isTestOrderRandomized_ = t;
}

int RunSettings::getTestRandomizeMethod() const
{
	return testRandomizeMethod;
}

void RunSettings::setTestRandomizeMethod(int m)
{
	testRandomizeMethod = m;
}

bool RunSettings::getPretestOrderList(StimLabelList& list) const
{
	list = pretestOrderList_;
	return true;
}

bool RunSettings::getHabituationOrderList(StimLabelList& list) const
{
	list = habituationOrderList_;
	return true;
}

bool RunSettings::getTestOrderList(StimLabelList& list) const
{
	list = testOrderList_;
	return true;
}


void RunSettings::setPretestOrderList(const StimLabelList& list)
{
	pretestOrderList_ = list;
}

void RunSettings::setHabituationOrderList(const StimLabelList& list)
{
	habituationOrderList_ = list;
}

void RunSettings::setTestOrderList(const StimLabelList& list)
{
	testOrderList_ = list;
}


bool RunSettings::getOrderFromString(StimLabelList& list, QString str)
{
	bool b = true;
	QRegExp resep("[ ,]+");
	// the regexp allows spaces and commas as separators. Extra spaces, and extra commas for
	// that matter, are allowed.
	QStringList orderList = str.split(resep);
	for (QStringList::iterator it = orderList.begin(); it != orderList.end() && b; ++it)
	{
		int num = it->toInt(&b);
		if (b) list.append(QPair<int, QString>(num, QString()));
	}
	return b;
}


QDataStream & Habit::operator << (QDataStream& stream, Habit::RunSettings settings)
{
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
	return stream;
}

QDataStream & Habit::operator >> (QDataStream& stream, Habit::RunSettings& settings)
{
	int experimentId_;
	int subjectId_;
	int id_;

	QString habituationOrderName;
	bool isHabituationOrderRandomized_;
	int habituationRandomizeMethod;

	QString pretestOrderName;
	bool isPretestOrderRandomized_;
	int pretestRandomizeMethod;

	QString testOrderName;
	bool isTestOrderRandomized_;
	int testRandomizeMethod;

	stream >> id_; settings.setId(id_);
	stream >> experimentId_; settings.setExperimentId(experimentId_);
	stream >> subjectId_; settings.setSubjectId(subjectId_);
	stream >> pretestOrderName; settings.setPretestOrderName(pretestOrderName);
	stream >> isPretestOrderRandomized_; settings.setPretestRandomized(isPretestOrderRandomized_);
	stream >> pretestRandomizeMethod; settings.setPretestRandomizeMethod(pretestRandomizeMethod);
	stream >> habituationOrderName; settings.setHabituationOrderName(habituationOrderName);
	stream >> isHabituationOrderRandomized_; settings.setHabituationRandomized(isHabituationOrderRandomized_);
	stream >> habituationRandomizeMethod; settings.setHabituationRandomizeMethod(habituationRandomizeMethod);
	stream >> testOrderName; settings.setTestOrderName(testOrderName);
	stream >> isTestOrderRandomized_; settings.setTestRandomized(isTestOrderRandomized_);
	stream >> testRandomizeMethod; settings.setTestRandomizeMethod(testRandomizeMethod);
	return stream;
}


bool Habit::operator==(const Habit::RunSettings& lhs, const Habit::RunSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getExperimentId() == rhs.getExperimentId() &&
			lhs.getSubjectId() == rhs.getSubjectId() &&
			lhs.getHabituationOrderName() == rhs.getHabituationOrderName() &&
			lhs.isHabituationRandomized() == rhs.isHabituationRandomized() &&
			lhs.getHabituationRandomizeMethod() == rhs.getHabituationRandomizeMethod() &&
			lhs.getPretestOrderName() == rhs.getPretestOrderName() &&
			lhs.isPretestRandomized() == rhs.isPretestRandomized() &&
			lhs.getPretestRandomizeMethod() == rhs.getPretestRandomizeMethod() &&
			lhs.getTestOrderName() == rhs.getTestOrderName() &&
			lhs.isTestRandomized() == rhs.isTestRandomized() &&
			lhs.getTestRandomizeMethod() == rhs.getTestRandomizeMethod());
}

