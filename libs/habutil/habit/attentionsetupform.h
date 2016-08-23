#ifndef ATTENTIONSETUPFORM_H
#define ATTENTIONSETUPFORM_H

#include "attentiongettersettings.h"
#include "stimulussettings.h"
#include "stimulusdisplayinfo.h"
#include "HStimulusSettingsWidget.h"
#include "HStimulusPreviewWidget.h"
#include <QtGui>

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
	void components();
	void connections();

public:
	Habit::AttentionGetterSettings getConfigurationObject();
	void setConfigurationObject(const Habit::AttentionGetterSettings&);
	void initialize();
	//void setStimulusLayoutType(const HStimulusLayoutType& layoutType);

private slots:
	void stimulusLayoutTypeChanged(int);
	void stimulusSettingsChanged();

private:
	Habit::AttentionGetterSettings m_agSettings;
	Habit::StimulusDisplayInfo m_stimulusDisplayInfo;
	HStimulusSettingsWidget* m_pStimulusSettingsWidget;
	HStimulusPreviewWidget* m_pStimulusPreviewWidget;
	QRadioButton *m_prbNoISI;
	QRadioButton *m_prbUseAG;
	QRadioButton *m_prbUseISI;
	QLineEdit *m_plineeditISI;
};
}
#endif // ATTENTIONSETUPFORM_H
