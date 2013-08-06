#ifndef RESULTSVIEWEROPTIONS_H
#define RESULTSVIEWEROPTIONS_H

#include "resultviewersettings.h"

#include <QDialog>

class QCheckBox;
class QListWidget;
class QGroupBox;
class QLabel;
class QPushButton;

namespace GUILib {

class ResultsViewerOptions : public QDialog
{
	Q_OBJECT

public:
	ResultsViewerOptions(QWidget* w = 0);
	~ResultsViewerOptions();

	Habit::ResultViewerSettings getConfigurationObject();
private:
	void createComponents();
	void createExperimentHeaderSection();
	void createReliabilityInfoSection();
	void createPhasesSection();
	void createExperimentSummarySection();
	void createTrialInfoSection();
	void createControlButtons();
	void makeConnections();
	void doLayout();
	void doExperimentHeaderLayout();
	void doReliabilityInfoLayout();
	void doPhasesLayout();
	void doExperimentSummaryLayout();
	void doTrialInformationLayout();
	void doMainLayout();
	void initializeValuesFromDB();
	void writeValuesToDB();

private slots:
	void on_cancel();
	void on_ok();
	
private:
	QCheckBox* subjectName;
	QCheckBox* birthDate;
	QCheckBox* testDate;
	QCheckBox* observer;
	QCheckBox* condition;
	QCheckBox* cellNo;
	QCheckBox* comments;

	QCheckBox* ongFileName;
	QCheckBox* relDate;
	QCheckBox* relObserver;
	QCheckBox* relComments;

	QCheckBox* pretest;
	QCheckBox* habituation;
	QCheckBox* test;

	QCheckBox* experimentSummary;
	QCheckBox* lookTimeline;

	QCheckBox* attentionLatencies;
	QCheckBox* attentionOnTime;
	QCheckBox* leftMonitorLookTime;
	QCheckBox* rightMonitorLookTime;
	QCheckBox* centerMonitorLookTime;
	QCheckBox* totalLookTime;
	QCheckBox* trialDurations;
	QCheckBox* numberLeftLooks;
	QCheckBox* numberCenterLooks;
	QCheckBox* numberRightLooks;
	QCheckBox* totalNumberLooks;
	QCheckBox* leftMonitorFilenames;
	QCheckBox* centerMonitorFilenames;
	QCheckBox* rightMonitorFilenames;
	QCheckBox* independentSoundFilenames;
	QCheckBox* stimulusNames;

	QGroupBox* experimentHeaderGroup;
	QGroupBox* reliabilityInfoGroup;
	QGroupBox* phasesGroup;
	QGroupBox* experimentSummaryGroup;
	QGroupBox* trialInfoGroup;

	QListWidget* listWidget;

	QPushButton* cancel;
	QPushButton* ok;
};

} // namespace GUILib

#endif // RESULTSVIEWEROPTIONS_H
