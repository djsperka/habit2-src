
#ifndef HABIT_SUBJECT_SETTINGS_H
#define HABIT_SUBJECT_SETTINGS_H

#include <QString>
#include <QDateTime>
#include <QDataStream>

namespace Habit {

class SubjectSettings
{
public:
	SubjectSettings();
	~SubjectSettings();

public:
	int getId() const;
	void setId(int id);
	QString getSubjectName() const;
	void setSubjectName(const QString& name);
	QDateTime getBirthDate() const;
	void setBirthDate(const QDateTime& bd);
	QDateTime getTestDate() const;
	void setTestDate(const QDateTime& td);
	QString getCondition() const;
	void setCondition(const QString& s);
	QString getObserver() const;
	void setObserver(const QString& s);
	QString getCellNumber() const;
	void setCellNumber(const QString& s);
	QString getComments() const;
	void setComments(const QString& s);
	bool isOk();

private:
	int id_;
	QString subjectName_;
	QDateTime subjectBirthDate_;
	QDateTime testDate_;
	QString condition_;
	QString observer_;
	QString cellNumber_;
	QString comments_;
};

QDataStream & operator << (QDataStream& stream, SubjectSettings settings);
QDataStream & operator >> (QDataStream& stream, SubjectSettings& settings);
bool operator==(const SubjectSettings& lhs, const SubjectSettings& rhs);


} // namespace Habit

#endif // HABIT_SUBJECT_SETTINGS_H
