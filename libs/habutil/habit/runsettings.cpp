#include "runsettings.h"
#include <QDebug>

Habit::RunSettings::RunSettings()
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

Habit::RunSettings::~RunSettings() {
}

int Habit::RunSettings::getId() const {
	return id_;
}

void Habit::RunSettings::setId(int id) {
	id_ = id;
}

int Habit::RunSettings::getExperimentId() const {
	return experimentId_;
}

void Habit::RunSettings::setExperimentId(int id) {
	experimentId_ = id;
}

int Habit::RunSettings::getSubjectId() const {
	return subjectId_;
}

void Habit::RunSettings::setSubjectId(int id) {
	subjectId_ = id;
}

QString Habit::RunSettings::getHabituationOrder() const {
	return habituationTestOrder_;
}

void Habit::RunSettings::setHabituationOrder(const QString& order) {
	habituationTestOrder_ = order;
}

bool Habit::RunSettings::isHabituationRandomized() const {
	return isHabituationOrderRandomized_;
}

void Habit::RunSettings::setHabituationRandomized(bool t) {
	isHabituationOrderRandomized_ = t;
}

int Habit::RunSettings::getHabituationRandomizeMethod() const {
	return habituationRandomizeMethod;
}

void Habit::RunSettings::setHabituationRandomizeMethod(int m) {
	habituationRandomizeMethod = m;
}

QString Habit::RunSettings::getPretestOrder() const {
	return pretestTestOrder_;
}

void Habit::RunSettings::setPretestOrder(const QString& order) {
	pretestTestOrder_ = order;
}

bool Habit::RunSettings::isPretestRandomized() const {
	return isPretestOrderRandomized_;
}

void Habit::RunSettings::setPretestRandomized(bool t) {
	isPretestOrderRandomized_ = t;
}

int Habit::RunSettings::getPretestRandomizeMethod() const {
	return pretestRandomizeMethod;
}

void Habit::RunSettings::setPretestRandomizeMethod(int m) {
	pretestRandomizeMethod = m;
}

QString Habit::RunSettings::getTestOrder() const {
	return testTestOrder_;
}

void Habit::RunSettings::setTestOrder(const QString& order) {
	testTestOrder_ = order;
}

bool Habit::RunSettings::isTestRandomized() const {
	return isTestOrderRandomized_;
}

void Habit::RunSettings::setTestRandomized(bool t) {
	isTestOrderRandomized_ = t;
}

int Habit::RunSettings::getTestRandomizeMethod() const {
	return testRandomizeMethod;
}

void Habit::RunSettings::setTestRandomizeMethod(int m) {
	testRandomizeMethod = m;
}

QDataStream & Habit::operator << (QDataStream& stream, Habit::RunSettings settings)
{
	stream << settings.getId();
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

QDataStream & Habit::operator >> (QDataStream& stream, Habit::RunSettings& settings)
{
	int experimentId_;
	int subjectId_;
	int id_;

	QString habituationTestOrder_;
	bool isHabituationOrderRandomized_;
	int habituationRandomizeMethod;

	QString pretestTestOrder_;
	bool isPretestOrderRandomized_;
	int pretestRandomizeMethod;

	QString testTestOrder_;
	bool isTestOrderRandomized_;
	int testRandomizeMethod;

	stream >> id_; settings.setId(id_);
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


bool Habit::operator==(const Habit::RunSettings& lhs, const Habit::RunSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getExperimentId() == rhs.getExperimentId() &&
			lhs.getSubjectId() == rhs.getSubjectId() &&
			lhs.getHabituationOrder() == rhs.getHabituationOrder() &&
			lhs.isHabituationRandomized() == rhs.isHabituationRandomized() &&
			lhs.getHabituationRandomizeMethod() == rhs.getHabituationRandomizeMethod() &&
			lhs.getPretestOrder() == rhs.getPretestOrder() &&
			lhs.isPretestRandomized() == rhs.isPretestRandomized() &&
			lhs.getPretestRandomizeMethod() == rhs.getPretestRandomizeMethod() &&
			lhs.getTestOrder() == rhs.getTestOrder() &&
			lhs.isTestRandomized() == rhs.isTestRandomized() &&
			lhs.getTestRandomizeMethod() == rhs.getTestRandomizeMethod());
}

