/*
 *  ControlDialog.h
 *
 */

#include <QtGui>
#include <QTimer>
#include "HLookDetector.h"
#include "HEventLog.h"
#include "HLook.h"
#include "HPhaseCriteria.h"
#include "habituationsetupform.h"
#include "HLookSettingsWidget.h"

class TestKPDialog : public QDialog
{
	Q_OBJECT

public:
	TestKPDialog();
	~TestKPDialog();

private:
	QGroupBox *m_pgbUseTestLD;
	QCheckBox *m_pcbUsePhaseCriteria;

	GUILib::HLookSettingsWidget* m_pLookSettingsWidget;

#if 0
	QLineEdit *m_pleMinLookTime;
	QLineEdit *m_pleMinLookAwayTime;
	QLineEdit *m_pleMaxLookAwayTime;
	QLineEdit *m_pleMaxAccumulatedLookTime;
#endif

	HLookDetector *m_pld;
	HEventLog m_eventLog;
	HPhaseCriteria* m_phaseCriteria;
	Habit::HabituationSettings m_habituationSettings;
	bool m_bUsePhaseCriteria;
	int m_ntrials;

protected slots:
	void pbtnEditCriteriaClicked();
	void pbtnGoClicked();
	void pbtnEnableClicked();
	void pbtnDisableClicked();

	void gotLook(HLook l);
	void gotLookStarted();
	void gotLookPending();
	void gotLookAborted();
	void gotMaxLookAwayTime();
	void gotMaxAccumulatedLookTime();
};

class EditCriteriaDialog: public QDialog
{
	Q_OBJECT

public:
	EditCriteriaDialog(const Habit::HabituationSettings& settings);
	~EditCriteriaDialog() {};
	Habit::HabituationSettings getConfigurationObject() { return m_form->getConfigurationObject(); };

private:
	GUILib::HabituationSetupForm* m_form;
};
