#ifndef HABITUATIONSETUPFORM_H
#define HABITUATIONSETUPFORM_H

#include "habituationsettings.h"

#ifndef USING_QT5
#include <QtGui>
#else
#include <QtWidgets>
#endif

namespace GUILib {

class HabituationSetupForm : public QWidget
{
	Q_OBJECT

public:
	static const QString FIXED_NUMBER_OF_TRIALS;
	static const QString CRITERION_TO_END; 
	static const QString TOTAL_LOOKING_TIME_TO_END;
	static const QString FIXED_WINDOW;
	static const QString SLIDING_WINDOW;

public:
	HabituationSetupForm(const Habit::HabituationSettings& settings, QWidget* w = 0);
	~HabituationSetupForm();

private:
	void createComponents();
	void setLabelsFont();
	void makeConnections();
	void doLayout();
	void doBasisLayout();
	void doPercentLayout();
	void doWindowLayout();
	void doLookLengthLayout();

private slots:
	void onCurrentIndexChanged(int);

public:
	Habit::HabituationSettings getConfigurationObject();
	void setConfigurationObject(const Habit::HabituationSettings&);
	void initialize();

private:
	QLabel* mainTitle_;
	QGroupBox* habituationGroup_;
	QComboBox* habituationCombo_;
	QGroupBox* basisGroup_;
	QRadioButton* basisRadio1_;
	QRadioButton* basisRadio2_;
	QGroupBox* percentGroup_;
	QLineEdit* percentEdit_;
	QLabel* percentLabel_;
	QGroupBox* windowMainGroup_;
	QGroupBox* windowSizeGroup_;
	QGroupBox* windowTypeGroup_;
	QLineEdit* windowSizeEdit_;
	QLabel* windowSizeLabel_;
	QComboBox* windowTypeCombo_;
	QVBoxLayout* habituationLayout_;
	Habit::HabituationSettings settings_;
	QGroupBox* totalLookLengthToEndGroup_;
	QLineEdit* totalLookLengthEdit_;
	QLabel* totalLookLengthLabel_;
};
} //namespace GUILIB

#endif // HABITUATIONSETUPFORM_H
