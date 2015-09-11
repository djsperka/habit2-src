/*
 *  ControlDialog.cpp
 *
 */

#include "TestKPDialog.h"
#include "HKeypadLookDetector.h"
#include "HTestingLookDetector.h"
#include "HElapsedTimer.h"
#include "HPhaseCriteriaUtil.h"
#include "HLookSettings.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QTextStream>
#include <QtDebug>
#include <QIntValidator>


TestKPDialog::TestKPDialog()
: QDialog()
, m_pld(NULL)
, m_bUsePhaseCriteria(false)
, m_ntrials(1)
{
	m_pgbUseTestLD = new QGroupBox("Use Test LD");
	m_pgbUseTestLD->setCheckable(true);
	m_pgbUseTestLD->setChecked(false);
	m_pcbUsePhaseCriteria = new QCheckBox("Test Habituation Criteria");
	QPushButton *pbtnEditCriteria = new QPushButton("Edit Criteria");
	m_pLookSettingsWidget = new GUILib::HLookSettingsWidget();
#if 0
	m_pleMinLookTime = new QLineEdit;
	m_pleMinLookTime->setValidator(new QIntValidator(0, 100000, m_pleMinLookTime));
	m_pleMinLookAwayTime = new QLineEdit;
	m_pleMinLookAwayTime->setValidator(new QIntValidator(0, 100000, m_pleMinLookAwayTime));
	m_pleMaxLookAwayTime = new QLineEdit;
	m_pleMaxLookAwayTime->setValidator(new QIntValidator(0, 100000, m_pleMaxLookAwayTime));
	m_pleMaxAccumulatedLookTime = new QLineEdit;
	m_pleMaxAccumulatedLookTime->setValidator(new QIntValidator(0, 100000, m_pleMaxAccumulatedLookTime));
	QLabel* plabelMinLookTime = new QLabel("Min Look MS");
	QLabel* plabelMinLookAwayTime = new QLabel("Min LookAway MS");
	QLabel* plabelMaxLookAwayTime = new QLabel("Max LookAway MS");
	QLabel* plabelMaxAccumulatedLookTime = new QLabel("Max accum Look MS");
#endif


	QPushButton *pbtnGo = new QPushButton("Load Input & Go");
	QPushButton *pbtnEnable = new QPushButton("Enable", this);
	QPushButton *pbtnDisable = new QPushButton("Disable", this);

	QHBoxLayout *h0 = new QHBoxLayout;
	h0->addWidget(m_pcbUsePhaseCriteria);
	h0->addWidget(pbtnEditCriteria);
	QVBoxLayout *v0 = new QVBoxLayout;
	v0->addLayout(h0);
	v0->addWidget(pbtnGo);
	m_pgbUseTestLD->setLayout(v0);

#if 0
	QHBoxLayout *l1 = new QHBoxLayout;
	l1->addWidget(plabelMinLookTime);
	l1->addWidget(m_pleMinLookTime);
	QHBoxLayout *l2 = new QHBoxLayout;
	l2->addWidget(plabelMinLookAwayTime);
	l2->addWidget(m_pleMinLookAwayTime);
	QHBoxLayout *l3 = new QHBoxLayout;
	l3->addWidget(plabelMaxLookAwayTime);
	l3->addWidget(m_pleMaxLookAwayTime);
	QHBoxLayout *l4 = new QHBoxLayout;
	l4->addWidget(plabelMaxAccumulatedLookTime);
	l4->addWidget(m_pleMaxAccumulatedLookTime);
#endif

	QHBoxLayout *l5 = new QHBoxLayout;
	l5->addWidget(pbtnEnable);
	l5->addWidget(pbtnDisable);

	QVBoxLayout *v1 = new QVBoxLayout;
#if 0
	v1->addLayout(l1);
	v1->addLayout(l2);
	v1->addLayout(l3);
	v1->addLayout(l4);
#endif
	v1->addWidget(m_pLookSettingsWidget);
//	v1->addWidget(pbtnGo);
	v1->addLayout(l5);
	v1->addWidget(m_pgbUseTestLD);

	setLayout(v1);

	// Connections
	connect(m_pgbUseTestLD, SIGNAL(toggled(bool)), pbtnEnable, SLOT(setDisabled(bool)));
	connect(m_pgbUseTestLD, SIGNAL(toggled(bool)), pbtnDisable, SLOT(setDisabled(bool)));
	connect(pbtnEditCriteria, SIGNAL(clicked()), this, SLOT(pbtnEditCriteriaClicked()));
	connect(pbtnGo, SIGNAL(clicked()), this, SLOT(pbtnGoClicked()));
	connect(pbtnEnable, SIGNAL(clicked()), this, SLOT(pbtnEnableClicked()));
	connect(pbtnDisable, SIGNAL(clicked()), this, SLOT(pbtnDisableClicked()));

	pbtnEditCriteria->setEnabled(false);
}

TestKPDialog::~TestKPDialog()
{
}

void TestKPDialog::pbtnEnableClicked()
{
	if (m_pld)
	{
		qDebug() << "Enable ld.";
		m_pld->enableLook();
	}
}

void TestKPDialog::pbtnDisableClicked()
{
	if (m_pld)
	{
		qDebug() << "Disable ld.";
		m_pld->disable();
	}
}

void TestKPDialog::pbtnEditCriteriaClicked()
{
	// edit criteria here.
	EditCriteriaDialog* pEditDialog = new EditCriteriaDialog(m_habituationSettings);

	if (pEditDialog->exec() == QDialog::Accepted)
	{
		m_habituationSettings = pEditDialog->getConfigurationObject();
	}

}

void TestKPDialog::pbtnGoClicked()
{
	int minLook;
	int minLookAway;
	int maxLookAway = 0;
	int maxAccum = 0;

	QFile file;
	bool b;
	Habit::HLookSettings ls = m_pLookSettingsWidget->getHLookSettings();

#if 0
	minLook = m_pleMinLookTime->text().toInt(&b);
	minLookAway = m_pleMinLookAwayTime->text().toInt(&b);
	maxLookAway = m_pleMaxLookAwayTime->text().toInt(&b);
	maxAccum = m_pleMaxAccumulatedLookTime->text().toInt(&b);
#endif

	m_bUsePhaseCriteria = m_pcbUsePhaseCriteria->isChecked();

#if 0
	m_pleMinLookTime->setEnabled(false);
	m_pleMinLookAwayTime->setEnabled(false);
	m_pleMaxLookAwayTime->setEnabled(false);
	m_pleMaxAccumulatedLookTime->setEnabled(false);
#endif

	m_pLookSettingsWidget->setEnabled(false);

	m_phaseCriteria = createPhaseCriteria(m_habituationSettings, m_ntrials);
	if (m_pgbUseTestLD->isChecked())
	{
		qDebug() << "using test look detector";
		QString selectedFileName = QFileDialog::getOpenFileName(NULL, "Select LD testing input file", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation), "(*.txt)");
		qDebug() << "Selected db " << selectedFileName;
		QFile file(selectedFileName);
		HTestingLookDetector* pTestingLD = new HTestingLookDetector(file, ls.getMinLookTime(), ls.getMinLookAwayTime(), maxLookAway, maxAccum, m_eventLog, this, true, false, true);
		m_pld = (HLookDetector*)pTestingLD;
	}
#if 0
	else
	{
		qDebug() << "Using keypad look detector";
		HEventLog log;
		m_pld = new HKeypadLookDetector(log, this, minLook, minLookAway, maxLookAway, maxAccum, true, false, true);

	}
#endif

	connect(m_pld, SIGNAL(look(HLook)), this, SLOT(gotLook(HLook)));
	connect(m_pld, SIGNAL(lookStarted()), this, SLOT(gotLookStarted()));
	connect(m_pld, SIGNAL(lookPending()), this, SLOT(gotLookPending()));
	connect(m_pld, SIGNAL(lookAborted()), this, SLOT(gotLookAborted()));
	connect(m_pld, SIGNAL(maxLookAwayTime()), this, SLOT(gotMaxLookAwayTime()));
	connect(m_pld, SIGNAL(maxAccumulatedLookTime()), this, SLOT(gotMaxAccumulatedLookTime()));

}

void TestKPDialog::gotMaxLookAwayTime()
{
	qDebug() << "======> gotMaxLookAwayTime @ " << HElapsedTimer::elapsed();
	m_pld->disable();
	qDebug() << "======> gotMaxLookAwayTime - disable done";
}

void TestKPDialog::gotMaxAccumulatedLookTime()
{
	qDebug() << "======> gotMaxAccumulatedLookTime @ " << HElapsedTimer::elapsed();
	m_pld->disable();
	qDebug() << "======> gotMaxAccumulatedLookTime - disable done.";
}

void TestKPDialog::gotLook(HLook l)
{
	qDebug() << "======> gotLook: " << l;
}

void TestKPDialog::gotLookPending()
{
	qDebug() << "======> gotLookPending @ " << HElapsedTimer::elapsed();
}

void TestKPDialog::gotLookAborted()
{
	qDebug() << "======> gotLookAborted @ " << HElapsedTimer::elapsed();
}

void TestKPDialog::gotLookStarted()
{
	qDebug() << "======> gotLookStarted @ " << HElapsedTimer::elapsed();
}


EditCriteriaDialog::EditCriteriaDialog(const Habit::HabituationSettings& settings)
{
	m_form = new GUILib::HabituationSetupForm(settings);
	m_form->setConfigurationObject(settings);	// have to do this to get it to initialize
	QPushButton* pOK = new QPushButton("OK");
	QPushButton* pCancel = new QPushButton("Cancel");

	QVBoxLayout* v1 = new QVBoxLayout();
	v1->addWidget(m_form);

	QHBoxLayout* h1 = new QHBoxLayout();
	h1->addWidget(pCancel);
	h1->addWidget(pOK);
	v1->addLayout(h1);
	setLayout(v1);

	connect(pCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(pOK, SIGNAL(clicked()), this, SLOT(accept()));
}
