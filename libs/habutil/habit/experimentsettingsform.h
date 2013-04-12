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
}

class GUILib::ExperimentSettingsForm : public QDialog
{
    Q_OBJECT

public:
	static const QString NEW_EXPERIMENT;
	static const QString CLONE_EXPERIMENT;

public:
    ExperimentSettingsForm(const Habit::ExperimentSettings& experimentSettings, QWidget* w = 0);
    ~ExperimentSettingsForm();

public:
	Habit::ExperimentSettings getConfigurationObject();
	void setConfigurationObject(const Habit::ExperimentSettings&);
	void initialize();

private:
	void initializeAvailableExperimentSettings();
	void setControlsActive(bool isActive);
	bool validateMonitorSettings();
	bool validateHabituationSettings();
	bool isMediaExists(Habit::StimulusSettings& stimulus);

private slots:
	void onExperimentChoose(const QString&);
	void onCancel();
	void onDone();

signals:
	void initializationRequest(const Habit::ExperimentSettings& es);
	
private:
    //QComboBox* experimentBox_;
    GUILib::TreeWizard *treeWizard_;
    Habit::ExperimentSettings experimentSettings_;

	GUILib::MonitorSettingsForm* monitorSettingsForm_;
	GUILib::ControlBarOptionsForm* controlBarOptionsForm_;
	GUILib::DesignSetupForm* designSetupForm_;
	GUILib::HabituationSetupForm* habituationSetupForm_;
	GUILib::StimulusDisplayForm* stimulusDisplayForm_;
	GUILib::AttentionSetupForm* attentionSetup_;
	GUILib::HabituationStimuliForm* preTestStimuliForm_;
	GUILib::HabituationStimuliForm* habituationStimuliForm_;
	GUILib::HabituationStimuliForm* testStimuliForm_;
};

#endif // EXPERIMENTSETTINGSFORM_H
