#include "subjectsettings.h"

namespace Habit {

SubjectSettings::SubjectSettings() 
	: id_(-1)
	, subjectName_("")
	, subjectBirthDate_()
	, testDate_()
	, condition_("")
	, observer_("")
	, cellNumber_("")
	, comments_("")
	, isNameChanged_(false)
{}

SubjectSettings::~SubjectSettings() {
}

int SubjectSettings::getId() const {
	return id_;
}

void SubjectSettings::setId(int id) {
	id_ = id;
}

QString SubjectSettings::getSubjectName() const {
	return subjectName_;
}

void SubjectSettings::setSubjectName(const QString& name) {
	if(!subjectName_.isEmpty()) {
		isNameChanged_ = true;
	}
	subjectName_ = name;
}

QDateTime SubjectSettings::getBirthDate() const {
	return subjectBirthDate_;
}

void SubjectSettings::setBirthDate(const QDateTime& bd) {
	subjectBirthDate_ = bd;
}

QDateTime SubjectSettings::getTestDate() const {
	return testDate_;
}

void SubjectSettings::setTestDate(const QDateTime& td) {
	testDate_ = td;
}

QString SubjectSettings::getCondition() const {
	return condition_;
}

void SubjectSettings::setCondition(const QString& s) {
	condition_ = s;
}

QString SubjectSettings::getObserver() const {
	return observer_;
}

void SubjectSettings::setObserver(const QString& s) {
	observer_ = s;
}

QString SubjectSettings::getCellNumber() const {
	return cellNumber_;
}

void SubjectSettings::setCellNumber(const QString& s) {
	cellNumber_ = s;
}

QString SubjectSettings::getComments() const {
	return comments_;
}

void SubjectSettings::setComments(const QString& s) {
	comments_ = s;
}

bool SubjectSettings::isOk()
{
	return !subjectName_.isEmpty() && !observer_.isEmpty();
}

bool SubjectSettings::isNameChanged()
{
	return isNameChanged_;
}


} // namespace Habit