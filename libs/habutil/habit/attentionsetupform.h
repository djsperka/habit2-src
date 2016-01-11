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
	AttentionSetupForm(const Habit::AttentionGetterSettings& settings, const HStimulusLayoutType& layoutType = HStimulusLayoutType::HStimulusLayoutSingle, QWidget* w = 0);
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
	//void onColorChooserClick();
	void onModifyClick();
	void stimulusLayoutTypeChanged(int);

private:
	//QLabel* mainTitle_;
	QGroupBox* attentionGroup_;
	//QGroupBox* colorGroup_;
	QLabel* stimulusName_;
	QPushButton* modifyButton_;
	//QPushButton* colorButton_;
	QVBoxLayout* mainLayout_;
	Habit::AttentionGetterSettings settings_;
	const HStimulusLayoutType* m_pLayoutType;
//	StimulusSettingsEditorDialog* dlg_;
};
}
#endif // ATTENTIONSETUPFORM_H
