#ifndef EXPERIMENTRESULTSFORM_H
#define EXPERIMENTRESULTSFORM_H

#include "subjectsettings.h"
#include "runsettings.h"
#include "outputgenerator.h"

#include <QtGui/QDialog>

class QGroupBox;
class QLabel;
class QWidget;
class QPushButton;
class QVBoxLayout;

namespace GUILib {

class ExperimentResultsForm : public QDialog
{
	Q_OBJECT

public:
	ExperimentResultsForm(const Habit::SubjectSettings& ss, const Habit::RunSettings& rs, QWidget* w = 0);
	~ExperimentResultsForm();

private:
	void createComponents();
	void createSubjectInfoSection();
	void createSummeryOfResultsSection();
	void makeConnections();
	void doLayout();
	void doSubjectInfoLayout();
	void doSummaryOfResultsLayout();
	void doButtonsLayout();
	void setAlignment();
	void setAlignmentToSubjectInfo();
	void setAlignmentToSummaryResults();
	void initialize();

private slots:
	void editSubjectInfo();
	void exportMultipleResults();
	void onOk();
	void onViewDetailedResults();

private:
	Habit::SubjectSettings subjectSettings_;
	Habit::RunSettings runSettings_;
	QGroupBox* subjectInformation_;
	QLabel* name_;
	QLabel* birthDate_;
	QLabel* testDate_;
	QLabel* observer_;
	QLabel* condition_;
	QLabel* cellNo_;
	QLabel* comments_;
	
	QLabel* nameValue_;
	QLabel* birthDateValue_;
	QLabel* testDateValue_;
	QLabel* observerValue_;
	QLabel* conditionValue_;
	QLabel* cellNoValue_;
	QLabel* commentsValue_;
	QPushButton* editInfo_;
	QPushButton* viewDetailedResults_;
	QPushButton* exportResults_;
	QPushButton* exportMultipleResults_;

	QGroupBox* summaryOfResults_;
	QLabel* experimentName_;
	QLabel* settingsFile_;
	QLabel* resultsFile_;
	QLabel* runType_;
	QLabel* trialType_;
	QLabel* habituation_;
	QLabel* habituationResult_;

	QLabel* experimentNameValue_;
	QLabel* settingsFileValue_;
	QLabel* resultsFileValue_;
	QLabel* runTypeValue_;
	QLabel* trialTypeValue_;
	QLabel* habituationValue_;
	QLabel* habituationResultValue_;

	QPushButton*  cancelButton_;
	QPushButton*  okButton_;

	QVBoxLayout* mainLayout_;

};

} // namespace GUILib

#endif // EXPERIMENTRESULTSFORM_H
