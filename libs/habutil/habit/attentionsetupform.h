#ifndef ATTENTIONSETUPFORM_H
#define ATTENTIONSETUPFORM_H

#include "attentiongettersettings.h"
#include "stimulussettings.h"
#include "stimulusdisplayinfo.h"
#include "HStimulusSettingsWidget.h"
#include "HStimulusPreviewWidget.h"
#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif


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
	void toggleNoISI(bool checked);
	void toggleUseAG(bool checked);
	void toggleUseISI(bool checked);
	void toggleSoundOnlyAG(bool checked);

private:
	Habit::AttentionGetterSettings m_agSettings;
	Habit::StimulusDisplayInfo m_stimulusDisplayInfo;
	QGroupBox *m_pStimulusGroup;
	HStimulusSettingsWidget* m_pStimulusSettingsWidget;
	HStimulusPreviewWidget* m_pStimulusPreviewWidget;
	QRadioButton *m_prbNoISI;
	QRadioButton *m_prbUseAG;
	QRadioButton *m_prbUseISI;
	QRadioButton *m_prbSoundOnlyAG;
	QLineEdit *m_plineeditISI;
};
}
#endif // ATTENTIONSETUPFORM_H
