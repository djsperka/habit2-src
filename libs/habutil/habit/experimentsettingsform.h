#ifndef EXPERIMENTSETTINGSFORM_H
#define EXPERIMENTSETTINGSFORM_H

#include "experimentsettings.h"

#include <QtGui/QDialog>

class QComboBox;

namespace GUILib
{
    class TreeWizard;
}

namespace GUILib
{
    class ExperimentSettingsForm;
	class MonitorSettingsForm;
	class ControlBarOptionsForm;
	class DesignSetupForm;
	class HabituationSetupForm;
	class StimulusDisplayForm;
	class AttentionSetupForm;
	class HabituationStimuliForm;
	class HLookSettingsWidget;
	class HPhaseSettingsWidget;
}

class GUILib::ExperimentSettingsForm : public QDialog
{
    Q_OBJECT

public:
	static const QString NEW_EXPERIMENT;
	static const QString CLONE_EXPERIMENT;

public:
    ExperimentSettingsForm(const Habit::ExperimentSettings& experimentSettings, QWidget* w = 0, bool bViewOnly=false);
    ~ExperimentSettingsForm();

public:
	Habit::ExperimentSettings getConfigurationObject();
	void setConfigurationObject(const Habit::ExperimentSettings&);
	void initialize();

private:
	void initializeAvailableExperimentSettings();
	void initializeForViewOnly();
	void setControlsActive(bool isActive);
	bool validateMonitorSettings();
	bool validateHabituationSettings();
	bool isMediaExists(Habit::StimulusSettings& stimulus);
	void createComponents();
	void makeConnections();
	void doLayout();

private slots:
	void onExperimentChoose(const QString&);
	void onCancel();
	void onDone();

signals:
	void initializationRequest(const Habit::ExperimentSettings& es);
	
private:
	bool bViewOnly_;	// if true, the ExperimentSettings in constructor are to be viewed, not saved, experiment combo not populated or active.
    GUILib::TreeWizard *treeWizard_;
    Habit::ExperimentSettings experimentSettings_;

	GUILib::MonitorSettingsForm* monitorSettingsForm_;
	GUILib::ControlBarOptionsForm* controlBarOptionsForm_;
	GUILib::HLookSettingsWidget* lookSettingsForm_;
	GUILib::HPhaseSettingsWidget* pretestPhaseSettingsForm_;
	GUILib::HPhaseSettingsWidget* habituationPhaseSettingsForm_;
	GUILib::HPhaseSettingsWidget* testPhaseSettingsForm_;
	GUILib::HabituationSetupForm* habituationSetupForm_;
	GUILib::StimulusDisplayForm* stimulusDisplayForm_;
	GUILib::AttentionSetupForm* attentionSetup_;
	GUILib::HabituationStimuliForm* preTestStimuliForm_;
	GUILib::HabituationStimuliForm* habituationStimuliForm_;
	GUILib::HabituationStimuliForm* testStimuliForm_;
};

#endif // EXPERIMENTSETTINGSFORM_H
