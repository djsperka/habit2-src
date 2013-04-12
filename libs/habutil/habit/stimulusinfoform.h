#ifndef STIMULUSINFOFORM_H
#define STIMULUSINFOFORM_H

#include "stimulusinfo.h"

#include <QtGui/QWidget>

class QLabel;
class QGroupBox;
class QCheckBox;
class QFileDialog;
class QSlider;
class QPushButton;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;

namespace GUILib {

class StimulusInfoForm : public QWidget
{
	Q_OBJECT

public:
	StimulusInfoForm(const Habit::StimulusInfo& stim, const QString& pos, bool isIndep = false, QWidget* w = 0);
	~StimulusInfoForm();

private:
	void createComponents();
	void doFileChooserLayout();
	void doIndependentSoundLayout();
	void doMonitorStimulusLayout();
	void doLayout();
	void makeConnections();

public:
	Habit::StimulusInfo getConfigurationObject();
	bool isGroupEnabled();
	void setGroupEnabled(bool);
	void initialize(const Habit::StimulusInfo& stimulusInfo);

private slots:
	void onFileChooserClick();

public:
	void setScreenshot(const QString& path);

signals:
	void formSettingsChanged();


private:
	static QString defaultDir;
	Habit::StimulusInfo stimulusInfo_;
	QString stimulusPositionName_;
	bool isIndependentStimulus_;
	QGroupBox* stimulusInfoGroup_;
	QLabel* viewLabel_;
	QGroupBox* fileGroup_;
	QLabel* fileLabel_;
	QPushButton* fileChooser_;
	QCheckBox* loopPlayback_;
	QVBoxLayout* mainLayout_;
};

} // namespace GUILib

#endif // STIMULUSINFOFORM_H