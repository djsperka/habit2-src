#ifndef ATTENTIONSETUPFORM_H
#define ATTENTIONSETUPFORM_H

#include "attentiongettersettings.h"
#include "stimulussettings.h"
#include "stimulusdisplayinfo.h"
#include "HStimulusSettingsWidget.h"
#include "HStimulusPreviewWidget.h"
#include <QtGui/QWidget>

class QGroupBox;
class QLabel;
class QPushButton;
class QVBoxLayout;

namespace GUILib {

class StimulusSettingsEditorDialog;

class AttentionSetupForm : public QWidget
{
	Q_OBJECT

public:
	AttentionSetupForm(const Habit::AttentionGetterSettings& settings, const Habit::StimulusDisplayInfo& info, QWidget* w = 0);
	~AttentionSetupForm();

private:
	void createComponents();
	void setLabelsFont();
	void makeConnections();
	void doLayout();

public:
	Habit::AttentionGetterSettings getConfigurationObject();
	void setConfigurationObject(const Habit::AttentionGetterSettings&);
	void initialize();
	void setStimulusLayoutType(const HStimulusLayoutType& layoutType);

private slots:
	//void onModifyClick();
	void stimulusLayoutTypeChanged(int);
	void stimulusSettingsChanged();

private:
	QGroupBox* attentionGroup_;
	QLabel* stimulusName_;
	QPushButton* modifyButton_;
	QVBoxLayout* mainLayout_;
	Habit::AttentionGetterSettings settings_;
	//const HStimulusLayoutType* m_pLayoutType;
	Habit::StimulusDisplayInfo m_stimulusDisplayInfo;
	HStimulusSettingsWidget* stimulusSettingsWidget_;
	HStimulusPreviewWidget* stimulusPreviewWidget_;
};
}
#endif // ATTENTIONSETUPFORM_H
