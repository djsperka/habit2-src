#include "runsettings.h"

namespace Habit {

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

RunSettings::~RunSettings() {
}

int RunSettings::getId() const {
	return id_;
}

void RunSettings::setId(int id) {
	id_ = id;
}

int RunSettings::getExperimentId() const {
	return experimentId_;
}

void RunSettings::setExperimentId(int id) {
	experimentId_ = id;
}

int RunSettings::getSubjectId() const {
	return subjectId_;
}

void RunSettings::setSubjectId(int id) {
	subjectId_ = id;
}

QString RunSettings::getHabituationOrder() const {
	return habituationTestOrder_;
}

void RunSettings::setHabituationOrder(const QString& order) {
	habituationTestOrder_ = order;
}

bool RunSettings::isHabituationRandomized() const {
	return isHabituationOrderRandomized_;
}

void RunSettings::setHabituationRandomized(bool t) {
	isHabituationOrderRandomized_ = t;
}

int RunSettings::getHabituationRandomizeMethod() const {
	return habituationRandomizeMethod;
}

void RunSettings::setHabituationRandomizeMethod(int m) {
	habituationRandomizeMethod = m;
}

QString RunSettings::getPretestOrder() const {
	return pretestTestOrder_;
}

void RunSettings::setPretestOrder(const QString& order) {
	pretestTestOrder_ = order;
}

bool RunSettings::isPretestRandomized() const {
	return isPretestOrderRandomized_;
}

void RunSettings::setPretestRandomized(bool t) {
	isPretestOrderRandomized_ = t;
}

int RunSettings::getPretestRandomizeMethod() const {
	return pretestRandomizeMethod;
}

void RunSettings::setPretestRandomizeMethod(int m) {
	pretestRandomizeMethod = m;
}

QString RunSettings::getTestOrder() const {
	return testTestOrder_;
}

void RunSettings::setTestOrder(const QString& order) {
	testTestOrder_ = order;
}

bool RunSettings::isTestRandomized() const {
	return isTestOrderRandomized_;
}

void RunSettings::setTestRandomized(bool t) {
	isTestOrderRandomized_ = t;
}

int RunSettings::getTestRandomizeMethod() const {
	return testRandomizeMethod;
}

void RunSettings::setTestRandomizeMethod(int m) {
	testRandomizeMethod = m;
}

QDataStream & operator << (QDataStream& stream, RunSettings settings)
{
	stream << settings.getExperimentId();
	stream << settings.getSubjectId();
	stream << settings.getPretestOrder();
	stream << settings.isPretestRandomized();
	stream << settings.getPretestRandomizeMethod();
	stream << settings.getHabituationOrder();
	stream << settings.isHabituationRandomized();
	stream << settings.getHabituationRandomizeMethod();
	stream << settings.getTestOrder();
	stream << settings.isTestRandomized();
	stream << settings.getTestRandomizeMethod();
	return stream;
}

QDataStream & operator >> (QDataStream& stream, RunSettings& settings)
{
	int experimentId_;
	int subjectId_;

	QString habituationTestOrder_;
	bool isHabituationOrderRandomized_;
	int habituationRandomizeMethod;

	QString pretestTestOrder_;
	bool isPretestOrderRandomized_;
	int pretestRandomizeMethod;

	QString testTestOrder_;
	bool isTestOrderRandomized_;
	int testRandomizeMethod;

	stream >> experimentId_; settings.setExperimentId(experimentId_);
	stream >> subjectId_; settings.setSubjectId(subjectId_);
	stream >> pretestTestOrder_; settings.setPretestOrder(pretestTestOrder_);
	stream >> isPretestOrderRandomized_; settings.setPretestRandomized(isPretestOrderRandomized_);
	stream >> pretestRandomizeMethod; settings.setPretestRandomizeMethod(pretestRandomizeMethod);
	stream >> habituationTestOrder_; settings.setHabituationOrder(habituationTestOrder_);
	stream >> isHabituationOrderRandomized_; settings.setHabituationRandomized(isHabituationOrderRandomized_);
	stream >> habituationRandomizeMethod; settings.setHabituationRandomizeMethod(habituationRandomizeMethod);
	stream >> testTestOrder_; settings.setTestOrder(testTestOrder_);
	stream >> isTestOrderRandomized_; settings.setTestRandomized(isTestOrderRandomized_);
	stream >> testRandomizeMethod; settings.setTestRandomizeMethod(testRandomizeMethod);
	return stream;
}


} // namespace Habit
