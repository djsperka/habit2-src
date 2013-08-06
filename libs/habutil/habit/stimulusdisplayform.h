#ifndef STIMULUSDISPLAYFORM_H
#define STIMULUSDISPLAYFORM_H

#include "stimulusdisplayinfo.h"

#ifndef USING_QT5
#include <QtGuit>
#else
#include <QtWidgets>
#endif

namespace GUILib {

class StimulusDisplayForm : public QWidget
{
	Q_OBJECT

public:
	static const QString MONITOR_DEFINED; 
	static const QString SINGLE_MOVABLE;

	static const QString FULL_SCREEN;
	static const QString ORIGINAL_SIZE;

public:
	StimulusDisplayForm(const Habit::StimulusDisplayInfo& settings, QWidget* w = 0);
	~StimulusDisplayForm();

private:
	void createComponents();
	void setLabelsFont();
	void makeConnections();
	void doLayout();
	void doPresentationLayout();
	void doColorLayout();

public:
	Habit::StimulusDisplayInfo getConfigurationObject();
	void setConfigurationObject(const Habit::StimulusDisplayInfo&);
	void initialize();

private slots:
	void onColorChooserClick();
	void onDisplayComboIndexChanged(const QString&);
	
private:
	 QLabel* mainTitle_;
	 QGroupBox* presentationGroup_;
	 QComboBox* presentationCombo_;
	 QGroupBox* displayGroup_;
	 QComboBox* displayCombo_;
	 QCheckBox* displayCheck_;
	 QGroupBox* colorGroup_;
	 QPushButton* colorButton_;
	 QVBoxLayout* mainLayout_; 
	 Habit::StimulusDisplayInfo settings_;
};

}
#endif // STIMULUSDISPLAYFORM_H
