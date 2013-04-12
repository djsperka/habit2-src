#ifndef SUBJECTINFORMATION_H
#define SUBJECTINFORMATION_H

#include "subjectsettings.h"

#include <QtGui/QDialog>

class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QDateTimeEdit;
class QVBoxLayout;
class QGridLayout;

namespace GUILib {

class SubjectInformation : public QDialog
{
	Q_OBJECT

public:
	SubjectInformation(const Habit::SubjectSettings& settings = Habit::SubjectSettings(), QWidget* w = 0);
	~SubjectInformation();

private:
	void createComponents();
	void setAlignment();
	void createLabelsSection();
	void createEditsSection();
	void makeConnections();
	void doLayout();
	void doLabelsLayout();
	void initialize();
	bool validate();

public:
	void setConfigurationObject(const Habit::SubjectSettings& settings);
	Habit::SubjectSettings getConfigurationObject();

private slots:
	void onClearSubjectInfo();
	void onCancelClick();
	void onOkClick();

private:
	Habit::SubjectSettings settings_;
	QLabel* name_;
	QLabel* birthdate_;
	QLabel* testdate_;
	QLabel* condition_;
	QLabel* observer_;
	QLabel* cellNo_;
	QLabel* comments_;

	QLineEdit* nameEdit_;
	QDateTimeEdit* birthdateEdit_;
	QDateTimeEdit* testdateEdit_;
	QLineEdit* conditionEdit_;
	QLineEdit* observerEdit_;
	QLineEdit* cellNoEdit_;
	QTextEdit* commentsEdit_;

	QPushButton* clearSubjectInfo_;
	QPushButton* cancelButton_;
	QPushButton* okButton_;

	QVBoxLayout* mainLayout_;
	QGridLayout* grid_;	
};

}

#endif // SUBJECTINFORMATION_H
