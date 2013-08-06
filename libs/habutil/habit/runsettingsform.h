#ifndef RUNSETTINGSFORM_H
#define RUNSETTINGSFORM_H

#include "experimentsettings.h"
#include "subjectsettings.h"
#include "runsettings.h"
#include "subjectinformationwidget.h"

#include <QDialog>
#include <QStackedWidget>

class QLabel;
class QComboBox;
class QGroupBox;
class QTextEdit;
class QListWidget;
class QListWidgetItem;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QString;

namespace GUILib {

class HabituationStimuliTable;

class RunSettingsForm : public QDialog
{
	Q_OBJECT

public:
	static const QString SUBJECT_INFORMATION;
	static const QString HABITUATION_ORDER;
	static const QString PRETEST_ORDER;
	static const QString TEST_ORDER;

	static const QString TEST_RUN;
	static const QString NEW_RUN;

public:
	RunSettingsForm(QWidget* w);
	~RunSettingsForm();

	Habit::SubjectSettings getSubjectSettings() { return subjectSettings_; }
	Habit::RunSettings getRunSettings() { return runSettings_; }
private:
	void createComponents();
	void makeConnections();
	void doLayout();
	void doRunTypeLayout();
	void doSettingsLayout();
	void doStimuliLayout();
	void doButtonsLayout();
	void initializeSettingsList();
	void initializeExperimentsBox();
	void initializeRunTypeCombo();
	void initializeRunSettings(const QString&);
	void initializeStimuliTable(const QString&);
	void initializeSubjects();
	void setControlsEnabled(bool isEnabled, bool isSubjInfoEnabled);
	bool isInformationCorrect();

private slots:
	void onDone();
	void onOrderActivation(QListWidgetItem*);
	void onExperimentActivation(const QString& experimentName);
	void onSubjectInformationEdit();
	void onRunTypeActivation(const QString& runType);
	void onSubjectChosen(const QString&);

private:
	QGroupBox* runTypeGroup_;
	QComboBox* runTypeCombo_;
	QLabel* detailedLabel_;	
	QGroupBox* settingsGroup_;
	QLabel* settingsLabel_;
	QListWidget* settingsList_;
	HabituationStimuliTable* stimuliTable_;
	QLabel* stimuliOrderLabel_;
	QTextEdit* stimuliOrderEdit_;
	QGroupBox* randomizeGroup_;
	QVBoxLayout* mainLayout_;
	QHBoxLayout* horizontalLayout_;
	QComboBox* randomizeOrderCombo_;
	QLabel* experimentsChooserLabel_;
	QComboBox* experimentBox_;
	QLabel* subjectChooserLabel_;
	QComboBox* subjectChooser_;
	QStackedWidget* stackedWgt_;
	SubjectInformationWidget* subjectInfoWgt_;
	QWidget* stimuliContainer_;

	QPushButton* cancelButton_;
	QPushButton* doneButton_;
	//QPushButton* editSubjectInformation_;
	Habit::ExperimentSettings activeExperiment_;
	Habit::SubjectSettings subjectSettings_;
	Habit::RunSettings runSettings_;
	QString currentTestOrder_;
};

} // namespace GUILIB

#endif // RUNSETTINGSFORM_H
