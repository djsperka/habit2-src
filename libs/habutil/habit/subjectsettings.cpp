#include "subjectsettings.h"

Habit::SubjectSettings::SubjectSettings()
	: id_(-1)
	, subjectName_("")
	, subjectBirthDate_()
	, testDate_()
	, condition_("")
	, observer_("")
	, cellNumber_("")
	, comments_("")
{}

Habit::SubjectSettings::~SubjectSettings() {
}

QDataStream & Habit::operator << (QDataStream& stream, Habit::SubjectSettings settings)
{
	stream << settings.getId() << settings.getSubjectName() << settings.getBirthDate() <<
			settings.getTestDate() << settings.getCondition() << settings.getObserver() <<
			settings.getCellNumber() << settings.getComments();
	return stream;
}

QDataStream & Habit::operator >> (QDataStream& stream, Habit::SubjectSettings& settings)
{
	int id;
	QString sname, condition, observer, cellNumber, comments;
	QDateTime bdate, tdate;
	stream >> id >> sname >> bdate >> tdate >> condition >> observer >> cellNumber >> comments;
	settings.setId(id);
	settings.setSubjectName(sname);
	settings.setBirthDate(bdate);
	settings.setTestDate(tdate);
	settings.setCondition(condition);
	settings.setObserver(observer);
	settings.setCellNumber(cellNumber);
	settings.setComments(comments);
	return stream;
}

bool Habit::operator==(const Habit::SubjectSettings& lhs, const Habit::SubjectSettings& rhs)
{
	return (lhs.getId() == rhs.getId() &&
			lhs.getSubjectName() == rhs.getSubjectName() &&
			lhs.getBirthDate() == rhs.getBirthDate() &&
			lhs.getTestDate() == rhs.getTestDate() &&
			lhs.getCondition() == rhs.getCondition() &&
			lhs.getObserver() == rhs.getObserver() &&
			lhs.getCellNumber() == rhs.getCellNumber() &&
			lhs.getComments() == rhs.getComments());
}



int Habit::SubjectSettings::getId() const {
	return id_;
}

void Habit::SubjectSettings::setId(int id) {
	id_ = id;
}

QString Habit::SubjectSettings::getSubjectName() const {
	return subjectName_;
}

void Habit::SubjectSettings::setSubjectName(const QString& name) {
#if 0
	if(!subjectName_.isEmpty()) {
		isNameChanged_ = true;
	}
#endif
	subjectName_ = name;
}

QDateTime Habit::SubjectSettings::getBirthDate() const {
	return subjectBirthDate_;
}

void Habit::SubjectSettings::setBirthDate(const QDateTime& bd) {
	subjectBirthDate_ = bd;
}

QDateTime Habit::SubjectSettings::getTestDate() const {
	return testDate_;
}

void Habit::SubjectSettings::setTestDate(const QDateTime& td) {
	testDate_ = td;
}

QString Habit::SubjectSettings::getCondition() const {
	return condition_;
}

void Habit::SubjectSettings::setCondition(const QString& s) {
	condition_ = s;
}

QString Habit::SubjectSettings::getObserver() const {
	return observer_;
}

void Habit::SubjectSettings::setObserver(const QString& s) {
	observer_ = s;
}

QString Habit::SubjectSettings::getCellNumber() const {
	return cellNumber_;
}

void Habit::SubjectSettings::setCellNumber(const QString& s) {
	cellNumber_ = s;
}

QString Habit::SubjectSettings::getComments() const {
	return comments_;
}

void Habit::SubjectSettings::setComments(const QString& s) {
	comments_ = s;
}

bool Habit::SubjectSettings::isOk()
{
	return !subjectName_.isEmpty() && !observer_.isEmpty();
}


