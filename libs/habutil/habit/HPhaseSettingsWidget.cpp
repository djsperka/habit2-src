/*
 * HPhaseSettingsWidget.cpp
 *
 *  Created on: Feb 28, 2014
 *      Author: Oakeslab
 */

#include "HPhaseSettingsWidget.h"
using namespace GUILib;
using namespace Habit;

HPhaseSettingsWidget::HPhaseSettingsWidget()
: m_settings()
{
	createComponents();
	setValidators();
	makeConnections();
	doLayout();
	initialize();
}

HPhaseSettingsWidget::HPhaseSettingsWidget(const HPhaseSettings& settings)
: m_settings(settings)
{
	createComponents();
	setValidators();
	makeConnections();
	doLayout();
	initialize();
}

void HPhaseSettingsWidget::createComponents()
{
	QFont f;
	f.setPointSize(10);
	setFont(f);

	QFontMetrics fm(f);
	int pixelsWide = 7*fm.averageCharWidth();

	m_pgb = new QGroupBox(QString("%1 Phase").arg(m_settings.getPhaseType().name()));
	//m_pgb->setCheckable(true);
	m_pgbEndTrialCriteria = new QGroupBox("Criteria for ending trials");
	m_pNTrials = new QSpinBox();
	m_pNTrials->setRange(1, 999);
	m_pcbEnabled = new QCheckBox("Enable this phase");

	m_pgbEndTrialCriteria = new QGroupBox("Criteria to end trials");

	m_pcbUseLookingCriteria = new QCheckBox("Use looking criteria");
	m_prbSingleCompleteLook = new QRadioButton("Single complete look");
	m_prbAccumulatedLooking = new QRadioButton("Accumulated look time");
	m_pbgLookingCriteria = new QButtonGroup();
	m_pbgLookingCriteria->addButton(m_prbSingleCompleteLook);
	m_pbgLookingCriteria->addButton(m_prbAccumulatedLooking);
	m_peditAccumulatedLookTime = new QLineEdit();
	m_peditAccumulatedLookTime->setMaxLength(7);
	m_peditAccumulatedLookTime->setFixedWidth(pixelsWide);
	m_peditAccumulatedLookTime->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	m_pcbMaxLookAwayTime = new QCheckBox("Max look away time");
	m_peditMaxLookAwayTime = new QLineEdit();
	m_peditMaxLookAwayTime->setMaxLength(7);
	m_peditMaxLookAwayTime->setFixedWidth(pixelsWide);
//	m_peditMaxLookAwayTime->setFixedHeight(pixelsHigh);
	m_peditMaxLookAwayTime->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	m_pcbMaxStimulusTime = new QCheckBox("Max stimulus time");
	m_peditMaxStimulusTime = new QLineEdit();
	m_peditMaxStimulusTime->setMaxLength(7);
	m_peditMaxStimulusTime->setFixedWidth(pixelsWide);
//	m_peditMaxStimulusTime->setFixedWidth(pixelsHigh);
	m_peditMaxStimulusTime->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	m_prbMeasureStimulusTimeFromOnset = new QRadioButton("Measured from stimulus onset");
	m_prbMeasureStimulusTimeFromLooking = new QRadioButton("Measured from initial looking");
	m_pbgMaxStimulusTime = new QButtonGroup();
	m_pbgMaxStimulusTime->addButton(m_prbMeasureStimulusTimeFromLooking);
	m_pbgMaxStimulusTime->addButton(m_prbMeasureStimulusTimeFromOnset);

	m_pcbMaxNoLookTime = new QCheckBox("Max initial time not looking");
	m_peditMaxNoLookTime = new QLineEdit();
}

void HPhaseSettingsWidget::setValidators()
{
	m_peditAccumulatedLookTime->setValidator(new QIntValidator(0, 9999999, this));
	m_peditMaxLookAwayTime->setValidator(new QIntValidator(0, 9999999, this));
	m_peditMaxStimulusTime->setValidator(new QIntValidator(0, 9999999, this));
	m_peditMaxNoLookTime->setValidator(new QIntValidator(0, 9999999, this));
}

void HPhaseSettingsWidget::cbLookingCriteriaToggled(bool checked)
{
	m_prbSingleCompleteLook->setEnabled(checked);
	m_prbAccumulatedLooking->setEnabled(checked);
	m_peditAccumulatedLookTime->setEnabled(checked);
}

void HPhaseSettingsWidget::cbMaxStimulusTimeToggled(bool checked)
{
	m_peditMaxStimulusTime->setEnabled(checked);
	m_prbMeasureStimulusTimeFromLooking->setEnabled(checked);
	m_prbMeasureStimulusTimeFromOnset->setEnabled(checked);
}

void HPhaseSettingsWidget::cbMaxNoLookTimeToggled(bool checked)
{
	m_peditMaxNoLookTime->setEnabled(checked);
}

void HPhaseSettingsWidget::cbMaxLookAwayTimeToggled(bool checked)
{
	m_peditMaxLookAwayTime->setEnabled(checked);
}


void HPhaseSettingsWidget::cbEnabledToggled(bool checked)
{
	m_pNTrials->setEnabled(checked);
	m_pcbUseLookingCriteria->setEnabled(checked);
	m_prbSingleCompleteLook->setEnabled(checked);
	m_prbAccumulatedLooking->setEnabled(checked);
	m_peditAccumulatedLookTime->setEnabled(checked);
	m_pcbMaxLookAwayTime->setEnabled(checked);
	m_peditMaxLookAwayTime->setEnabled(checked);
	m_pcbMaxStimulusTime->setEnabled(checked);
	m_peditMaxStimulusTime->setEnabled(checked);
	m_prbMeasureStimulusTimeFromOnset->setEnabled(checked);
	m_prbMeasureStimulusTimeFromLooking->setEnabled(checked);
	m_pcbMaxNoLookTime->setEnabled(checked);
	m_peditMaxNoLookTime->setEnabled(checked);

	// re-evaluate if things are now enabled
	if (checked)
	{
		cbLookingCriteriaToggled(m_pcbUseLookingCriteria->isChecked());
		cbMaxStimulusTimeToggled(m_pcbMaxStimulusTime->isChecked());
		cbMaxNoLookTimeToggled(m_pcbMaxNoLookTime->isChecked());
		cbMaxLookAwayTimeToggled(m_pcbMaxLookAwayTime->isChecked());
	}
}

void HPhaseSettingsWidget::makeConnections()
{
	connect(m_pcbEnabled, SIGNAL(toggled(bool)), this, SLOT(cbEnabledToggled(bool)));
	connect(m_pcbUseLookingCriteria, SIGNAL(toggled(bool)), this, SLOT(cbLookingCriteriaToggled(bool)));
	connect(m_pcbMaxStimulusTime, SIGNAL(toggled(bool)), this, SLOT(cbMaxStimulusTimeToggled(bool)));
	connect(m_pcbMaxNoLookTime, SIGNAL(toggled(bool)), this, SLOT(cbMaxNoLookTimeToggled(bool)));
	connect(m_pcbMaxLookAwayTime, SIGNAL(toggled(bool)), this, SLOT(cbMaxLookAwayTimeToggled(bool)));
}

void HPhaseSettingsWidget::doLayout()
{
	// create labels. Set font in each of these as they are unaffected by
	QLabel* plabelMS1 = new QLabel("ms");
	QLabel* plabelMS2 = new QLabel("ms");
	QLabel* plabelMS3 = new QLabel("ms");
	QLabel* plabelMS4 = new QLabel("ms");
	QLabel* plabelNumberOfTrials = new QLabel("Number of trials");
	QFont f;
	f.setPointSize(10);
	plabelMS1->setFont(f);
	plabelMS2->setFont(f);
	plabelMS3->setFont(f);
	plabelMS4->setFont(f);
	plabelNumberOfTrials->setFont(f);

	// Accumulated look time
	QHBoxLayout *hboxAccumulatedLookTime = new QHBoxLayout();
	hboxAccumulatedLookTime->addWidget(m_prbAccumulatedLooking, 0, Qt::AlignLeft);
	hboxAccumulatedLookTime->addWidget(m_peditAccumulatedLookTime, 0, Qt::AlignLeft);
	hboxAccumulatedLookTime->addWidget(plabelMS1, 0, Qt::AlignLeft);
	hboxAccumulatedLookTime->addStretch();

	// Looking criteria with radio buttons side-by-side
	QVBoxLayout *vboxLooking = new QVBoxLayout();
	vboxLooking->setMargin(0);
	vboxLooking->setSpacing(0);
	vboxLooking->addWidget(m_prbSingleCompleteLook);
	vboxLooking->addLayout(hboxAccumulatedLookTime);

	// max stimulus time
	QHBoxLayout *hboxMaxStimulusTime = new QHBoxLayout();
	hboxMaxStimulusTime->addWidget(m_pcbMaxStimulusTime, 0, Qt::AlignLeft);
	hboxMaxStimulusTime->addWidget(m_peditMaxStimulusTime, 0, Qt::AlignLeft);
	hboxMaxStimulusTime->addWidget(plabelMS2, 0, Qt::AlignLeft);
	//hboxMaxStimulusTime->addStretch();

	// max stim time with radio buttons side-by-side
//	QHBoxLayout *hMaxStim = new QHBoxLayout();
	QVBoxLayout *vboxMaxStim = new QVBoxLayout();
	vboxMaxStim->addWidget(m_prbMeasureStimulusTimeFromOnset);
	vboxMaxStim->addWidget(m_prbMeasureStimulusTimeFromLooking);
//	hMaxStim->addLayout(h2);
//	hMaxStim->addLayout(vMaxStim);

	// max lookaway time
	QHBoxLayout *hboxMaxLookAway = new QHBoxLayout();
	hboxMaxLookAway->addWidget(m_pcbMaxLookAwayTime, 0, Qt::AlignLeft);
	hboxMaxLookAway->addWidget(m_peditMaxLookAwayTime, 0, Qt::AlignLeft);
	hboxMaxLookAway->addWidget(plabelMS3, 0, Qt::AlignLeft);
	hboxMaxLookAway->addStretch();

	// max lookaway time
	QHBoxLayout *hboxMaxNoLook = new QHBoxLayout();
	hboxMaxNoLook->addWidget(m_pcbMaxNoLookTime, 0, Qt::AlignLeft);
	hboxMaxNoLook->addWidget(m_peditMaxNoLookTime, 0, Qt::AlignLeft);
	hboxMaxNoLook->addWidget(plabelMS4, 0, Qt::AlignLeft);
	hboxMaxNoLook->addStretch();

	// number of trials
	QHBoxLayout *hboxNumberOfTrials = new QHBoxLayout();
	hboxNumberOfTrials->addWidget(plabelNumberOfTrials, 0, Qt::AlignLeft);
	hboxNumberOfTrials->addWidget(m_pNTrials, 0, Qt::AlignLeft);
	hboxNumberOfTrials->addStretch();

	// vbox for left-hand column
	QVBoxLayout *vboxLeftHandColumn = new QVBoxLayout();
	vboxLeftHandColumn->addWidget(m_pcbEnabled);
	vboxLeftHandColumn->addLayout(hboxNumberOfTrials);

	// Pile it all up in a single vbox
//	QVBoxLayout *v1 = new QVBoxLayout();
// number of trials	v1->addLayout(h4);

//	v1->addWidget(m_pcbUseLookingCriteria);
//	v1->addWidget(m_prbSingleCompleteLook);
//	v1->addLayout(h1);

//	v1->addLayout(hLooking);

//	v1->addLayout(h2);
//	v1->addWidget(m_prbMeasureStimulusTimeFromOnset);
//	v1->addWidget(m_prbMeasureStimulusTimeFromLooking);

//	v1->addLayout(hMaxStim);

//	v1->addLayout(h3);

	QGridLayout *grid = new QGridLayout();
	grid->addWidget(m_pcbUseLookingCriteria, 0, 0, Qt::AlignLeft);
	grid->addLayout(vboxLooking, 0, 1, Qt::AlignLeft);
	grid->addLayout(hboxMaxStimulusTime, 1, 0, Qt::AlignLeft);
	grid->addLayout(vboxMaxStim, 1, 1, Qt::AlignLeft);
	grid->addLayout(hboxMaxLookAway, 2, 0, Qt::AlignLeft);
	grid->addLayout(hboxMaxNoLook, 3, 0, Qt::AlignLeft);

	QHBoxLayout *hh = new QHBoxLayout();
//	hh->addLayout(hboxNumberOfTrials);
//	hh->addLayout(v1);
	hh->addLayout(vboxLeftHandColumn);
	hh->addLayout(grid);

	m_pgb->setLayout(hh);

	// this widget object will have a single layout with the main group box in it
	QVBoxLayout* main = new QVBoxLayout();
	main->addWidget(m_pgb);
	setLayout(main);

}

void HPhaseSettingsWidget::initialize()
{
	m_pcbEnabled->setChecked(m_settings.getIsEnabled());
	m_pNTrials->setValue(m_settings.getNTrials());
	m_pcbUseLookingCriteria->setChecked(m_settings.getUseLookingCriteria());
	m_prbSingleCompleteLook->setChecked(m_settings.getIsSingleLook());
	m_prbAccumulatedLooking->setChecked(m_settings.getIsMaxAccumulatedLookTime());
	m_peditAccumulatedLookTime->setText(QString("%1").arg(m_settings.getMaxAccumulatedLookTime()));
	m_pcbMaxLookAwayTime->setChecked(m_settings.getIsMaxLookAwayTime());
	m_peditMaxLookAwayTime->setText(QString("%1").arg(m_settings.getMaxLookAwayTime()));
	m_pcbMaxStimulusTime->setChecked(m_settings.getIsMaxStimulusTime());
	m_peditMaxStimulusTime->setText(QString("%1").arg(m_settings.getMaxStimulusTime()));
	m_prbMeasureStimulusTimeFromOnset->setChecked(m_settings.getMeasureStimulusTimeFromOnset());
	m_prbMeasureStimulusTimeFromLooking->setChecked(m_settings.getMeasureStimulusTimeFromLooking());
	m_pcbMaxNoLookTime->setChecked(m_settings.getIsMaxNoLookTime());
	m_peditMaxNoLookTime->setText(QString("%1").arg(m_settings.getMaxNoLookTime()));

	cbEnabledToggled(m_settings.getIsEnabled());
}

const HPhaseSettings& HPhaseSettingsWidget::getHPhaseSettings()
{
	bool b;

	// Update m_settings with the current contents of all the little checkboxes etc.
	m_settings.setIsEnabled(m_pcbEnabled->isChecked());
	m_settings.setNTrials(m_pNTrials->value());
	m_settings.setUseLookingCriteria(m_pcbUseLookingCriteria->isChecked());
	m_settings.setIsSingleLook(m_prbSingleCompleteLook->isChecked());
	m_settings.setIsMaxAccumulatedLookTime(m_prbAccumulatedLooking->isChecked());
	m_settings.setMaxAccumulatedLookTime(m_peditAccumulatedLookTime->text().toUInt(&b));
	m_settings.setIsMaxLookAwayTime(m_pcbMaxLookAwayTime->isChecked());
	m_settings.setMaxLookAwayTime(m_peditMaxLookAwayTime->text().toUInt(&b));
	m_settings.setIsMaxStimulusTime(m_pcbMaxStimulusTime->isChecked());
	m_settings.setMaxStimulusTime(m_peditMaxStimulusTime->text().toUInt(&b));
	m_settings.setMeasureStimulusTimeFromOnset(m_prbMeasureStimulusTimeFromOnset->isChecked());
	m_settings.setMeasureStimulusTimeFromLooking(m_prbMeasureStimulusTimeFromLooking->isChecked());
	m_settings.setIsMaxNoLookTime(m_pcbMaxNoLookTime->isChecked());
	m_settings.setMaxNoLookTime(m_peditMaxNoLookTime->text().toUInt(&b));

	return m_settings;
}

