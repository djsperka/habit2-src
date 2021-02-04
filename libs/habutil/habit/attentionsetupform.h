#ifndef ATTENTIONSETUPFORM_H
#define ATTENTIONSETUPFORM_H

#include "attentiongettersettings.h"
#include "stimulussettings.h"
#include "stimulusdisplayinfo.h"
#include "HStimulusSettingsWidget.h"
#include "HStimulusPreviewWidget.h"
#include "HIntertrialIntervalSettingsWidget.h"
#include "HGMM.h"
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
	void initialize();
	void updatePreviewWidget();
public:
	Habit::AttentionGetterSettings getAttentionGetterSettings();

public slots:
	void stimulusDisplayInfoChanged(const Habit::StimulusDisplayInfo& info);
	void stimulusSettingsChanged();

private:
	Habit::AttentionGetterSettings m_agSettings;
	Habit::StimulusSettings m_ssCurrent;	// as they are modified during usage of this form, the latest is kept here. Used in getAttentionGetterSettings
	Habit::StimulusDisplayInfo m_stimulusDisplayInfo;
	HIntertrialIntervalSettingsWidget *m_pIntertrialIntervalSettingsWidget;
	HStimulusPreviewWidget* m_pStimulusPreviewWidget;
	HGMM* m_phgmm;
};
}
#endif // ATTENTIONSETUPFORM_H
