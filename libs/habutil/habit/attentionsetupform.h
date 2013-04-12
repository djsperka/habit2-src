#ifndef ATTENTIONSETUPFORM_H
#define ATTENTIONSETUPFORM_H

#include "attentiongettersettings.h"
#include "stimulussettings.h"

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
	AttentionSetupForm(const Habit::AttentionGetterSettings& settings, QWidget* w = 0);
	~AttentionSetupForm();

private:
	void createComponents();
	void setLabelsFont();
	void makeConnections();
	void doLayout();
	void doAttentionLayout();

public:
	Habit::AttentionGetterSettings getConfigurationObject();
	void setConfigurationObject(const Habit::AttentionGetterSettings&);
	void initialize();

private slots:
	void onColorChooserClick();
	void onConfigurationChange(const Habit::StimulusSettings& stimulus);
	void onModifyClick();

private:
	QLabel* mainTitle_;
	QGroupBox* attentionGroup_;
	QGroupBox* colorGroup_;
	QLabel* stimulusName_;
	QPushButton* modifyButton_;
	QPushButton* colorButton_; 
	QVBoxLayout* mainLayout_;
	Habit::AttentionGetterSettings settings_;
	StimulusSettingsEditorDialog* dlg_;
};
}
#endif // ATTENTIONSETUPFORM_H
