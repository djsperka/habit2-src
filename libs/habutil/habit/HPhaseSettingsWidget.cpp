/*
 * HPhaseSettingsWidget.cpp
 *
 *  Created on: Feb 28, 2014
 *      Author: Oakeslab
 */

#include "HPhaseSettingsWidget.h"
#include "ui_HPhaseSettingsForm.h"
using namespace GUILib;
using namespace Habit;

HPhaseSettingsWidget::HPhaseSettingsWidget(QWidget *parent)
: QWidget(parent)
, ui(new Ui::HPhaseSettingsForm)
, m_settings()
{
	ui->setupUi(this);
	setValidators();
	initialize();
}

HPhaseSettingsWidget::HPhaseSettingsWidget(const HPhaseSettings& settings, QWidget *parent)
: QWidget(parent)
, ui(new Ui::HPhaseSettingsForm)
, m_settings(settings)
{
	ui->setupUi(this);
	setValidators();
	initialize();
}

void HPhaseSettingsWidget::setValidators()
{
	ui->lineeditAccumulatedLookTime->setValidator(new QIntValidator(0, 9999999, this));
	ui->lineeditContinuousLookAwayTime->setValidator(new QIntValidator(0, 9999999, this));
	ui->lineeditMaxStimulusTime->setValidator(new QIntValidator(0, 9999999, this));
	ui->lineeditMaxInitialTimeNotLooking->setValidator(new QIntValidator(0, 9999999, this));
}


void HPhaseSettingsWidget::initialize()
{
	ui->gbxEnabled->setChecked(m_settings.getIsEnabled());
	ui->spinboxNTrials->setValue(m_settings.getNTrials());
	ui->cbxUseLookSettings->setChecked(m_settings.getUseLookingCriteria());
	ui->rbSingleCompleteLook->setChecked(m_settings.getIsSingleLook());
	ui->rbAccumulatedLookTime->setChecked(m_settings.getIsMaxAccumulatedLookTime());
	ui->lineeditAccumulatedLookTime->setText(QString("%1").arg(m_settings.getMaxAccumulatedLookTime()));
	ui->cbxContinuousLookAwayTime->setChecked(m_settings.getIsMaxLookAwayTime());
	ui->lineeditContinuousLookAwayTime->setText(QString("%1").arg(m_settings.getMaxLookAwayTime()));
	ui->cbxRepeatTrialInattentive->setChecked(m_settings.getRepeatTrialOnMaxLookAwayTime());
	ui->cbxMaxStimulusTime->setChecked(m_settings.getIsMaxStimulusTime());
	ui->lineeditMaxStimulusTime->setText(QString("%1").arg(m_settings.getMaxStimulusTime()));
	ui->rbMeasuredFromStimulusOnset->setChecked(m_settings.getMeasureStimulusTimeFromOnset());
	ui->rbMeasuredFromInitialLooking->setChecked(m_settings.getMeasureStimulusTimeFromLooking());
	ui->cbxMaxInitialTimeNotLooking->setChecked(m_settings.getIsMaxNoLookTime());
	ui->lineeditMaxInitialTimeNotLooking->setText(QString("%1").arg(m_settings.getMaxNoLookTime()));

	// Sometimes the signals from the checkboxes are not sent on initialization, leaving the radio buttons and
	// lineedits in the wrong state. Force everything to work here. CAUTION: this assumes that the ui has the same
	// connections!

	ui->rbSingleCompleteLook->setEnabled(m_settings.getUseLookingCriteria());
	ui->rbAccumulatedLookTime->setEnabled(m_settings.getUseLookingCriteria());
	ui->lineeditAccumulatedLookTime->setEnabled(m_settings.getUseLookingCriteria());

	ui->lineeditMaxStimulusTime->setEnabled(m_settings.getIsMaxStimulusTime());
	ui->rbMeasuredFromInitialLooking->setEnabled(m_settings.getIsMaxStimulusTime());
	ui->rbMeasuredFromStimulusOnset->setEnabled(m_settings.getIsMaxStimulusTime());

	ui->lineeditContinuousLookAwayTime->setEnabled(m_settings.getIsMaxLookAwayTime());

	ui->lineeditMaxInitialTimeNotLooking->setEnabled(m_settings.getIsMaxNoLookTime());
}

const HPhaseSettings& HPhaseSettingsWidget::getHPhaseSettings()
{
	bool b;

	// Update m_settings with the current contents of all the little checkboxes etc.

	m_settings.setIsEnabled(ui->gbxEnabled->isChecked());
	m_settings.setNTrials(ui->spinboxNTrials->value());
	m_settings.setUseLookingCriteria(ui->cbxUseLookSettings->isChecked());
	m_settings.setIsSingleLook(ui->rbSingleCompleteLook->isChecked());
	m_settings.setIsMaxAccumulatedLookTime(ui->rbAccumulatedLookTime->isChecked());
	m_settings.setMaxAccumulatedLookTime(ui->lineeditAccumulatedLookTime->text().toUInt(&b));
	m_settings.setIsMaxLookAwayTime(ui->cbxContinuousLookAwayTime->isChecked());
	m_settings.setMaxLookAwayTime(ui->lineeditContinuousLookAwayTime->text().toUInt(&b));
	m_settings.setRepeatTrialOnMaxLookAwayTime((ui->cbxRepeatTrialInattentive->isChecked()));
	m_settings.setIsMaxStimulusTime(ui->cbxMaxStimulusTime->isChecked());
	m_settings.setMaxStimulusTime(ui->lineeditMaxStimulusTime->text().toUInt(&b));
	m_settings.setMeasureStimulusTimeFromOnset(ui->rbMeasuredFromStimulusOnset->isChecked());
	m_settings.setMeasureStimulusTimeFromLooking(ui->rbMeasuredFromInitialLooking->isChecked());
	m_settings.setIsMaxNoLookTime(ui->cbxMaxInitialTimeNotLooking->isChecked());
	m_settings.setMaxNoLookTime(ui->lineeditMaxInitialTimeNotLooking->text().toUInt(&b));

	return m_settings;
}

