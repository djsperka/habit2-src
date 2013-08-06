#ifndef DESIGNSETUPFORM_H
#define DESIGNSETUPFORM_H

#include "designsettings.h"

#ifndef USING_QT5
#include <QtGui>
#else
#include <QtWidgets>
#endif


namespace GUILib {

class DesignSetupForm : public QWidget
{
	Q_OBJECT

public:
	static const QString SUBJECT_CONTROLLED;
	static const QString FIXED_LENGTH;

public:
	DesignSetupForm(const Habit::DesignSettings& settings, QWidget *w = 0);
	~DesignSetupForm();

private:
	void createComponents();
	void createTrialType();
	void createTrialLength();
	void createLookAwayTimes();
	void createNumberOfTrials();
	void setLabelsFont();
	void setValidators();
	void makeConnections();
	void doLayout();
	void doTrialTypeLayout();
	void doTrialLengthLayout();
	void doLookAwayTimeLayout();
	void doNumberOfTrialsLayout();

public:
	Habit::DesignSettings getConfigurationObject();
	void setConfigurationObject(const Habit::DesignSettings&);
	void initialize();

private:
	QGroupBox* trialGroup_;
	QGroupBox* trialPretestGroup_;
	QGroupBox* trialHabituationGroup_;
	QGroupBox* trialTestGroup_;
	QComboBox* pretestCombo_;
	QComboBox* habituationCombo_;
	QComboBox* testCombo_;
	QGroupBox* trialLengthGroup_;
	QGroupBox* pretestGroup_;
	QGroupBox* habituationGroup_;
	QGroupBox* testGroup_;
	QGroupBox* lookTimesGroup_;
	QGroupBox* minLookTimeGroup_;
	QGroupBox* minLookAwayTimeGroup_;
	QGroupBox* minNoLookTimeGroup_;
	QLineEdit* pretestEdit_;
	QLineEdit* habituationEdit_;
	QLineEdit* testEdit_;
	QLineEdit* minLookTimeEdit_;
	QLineEdit* minLookAwayTimeEdit_;
	QLineEdit* minNoLookTimeEdit_;
	QGroupBox* numberOfTrialsGroup_;
	QGroupBox* pretestTrialsGroup_;
	QGroupBox* habituationTrialsGroup_;
	QGroupBox* testTrialsGroup_;
	QLineEdit* pretestTrialsEdit_;
	QLineEdit* habituationTrialsEdit_;
	QLineEdit* testTrialsEdit_;
	Habit::DesignSettings settings_;
};

} // namespace GUILib

#endif // DESIGNSETUPFORM_H
