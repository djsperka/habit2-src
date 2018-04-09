/*
 * HIntertrialIntervalSettingsWidget.cpp
 *
 *  Created on: Apr 6, 2018
 *      Author: dan
 */

#include <HIntertrialIntervalSettingsWidget.h>
#include "ui_HIntertrialIntervalSettingsForm.h"
#include "HStimulusSettingsEditor.h"




GUILib::HIntertrialIntervalSettingsWidget::HIntertrialIntervalSettingsWidget(const Habit::AttentionGetterSettings& ags, const Habit::StimulusDisplayInfo& sdi, QWidget *parent)
: QWidget(parent)
, m_agsOriginal(ags)
, m_sdiOriginal(sdi)
, m_ss(ags.getAttentionGetterStimulus())
, m_ssSaved(ags.getAttentionGetterStimulus())
, ui(new Ui::HIntertrialIntervalSettingsForm)
{
	ui->setupUi(this);
	ui->leFixedTimeMS->setValidator(new QIntValidator(1, 1000000));
	ui->leFixedTimeMS->setText(QString("%1").arg(ags.getFixedISIMS()));
	if (ags.isAttentionGetterUsed() || ags.isFixedISI() || ags.isSoundOnly())
	{
		ui->rbUseIntertrialInterval->setChecked(true);
		ui->gbStimulus->setEnabled(true);
		if (ags.isAttentionGetterUsed())
		{
			ui->rbStimulus->setChecked(true);
		}
		else if (ags.isFixedISI())
		{
			ui->rbFixedTimeInterval->setChecked(true);
		}
		else
		{
			ui->rbSound->setChecked(true);
		}
	}
	else
	{
		ui->rbNoIntertrialInterval->setChecked(true);
		ui->gbStimulus->setEnabled(true);
	}

	// connection to edit stimulus
	connect(ui->pbEditStimulus, SIGNAL(clicked()), this, SLOT(editButtonClicked()));

	// connect "use stimulus as ag" button
	connect(ui->rbStimulus, SIGNAL(toggled(bool)), this, SLOT(rbStimulusToggled(bool)));

	// connect sound-only button
	connect(ui->rbSound, SIGNAL(toggled(bool)), this, SLOT(rbSoundToggled(bool)));

	// connect bkgd-only button
	connect(ui->rbBackground, SIGNAL(toggled(bool)), this, SLOT(rbBackgroundToggled(bool)));

}

GUILib::HIntertrialIntervalSettingsWidget::~HIntertrialIntervalSettingsWidget()
{
	delete ui;
}


void GUILib::HIntertrialIntervalSettingsWidget::rbSoundToggled(bool checked)
{
	if (checked)
	{
		// save the current settings, then force all the displayed stim to be background color
		m_ssSaved = m_ss;
		qDebug() << "Saving stimulus settings:";
		qDebug() << m_ssSaved;
		m_ss.getLeftStimulusInfo().setIsBackground(true);
		m_ss.getLeftStimulusInfo().setColor(m_sdiOriginal.getBackGroundColor());
		m_ss.getCenterStimulusInfo().setIsBackground(true);
		m_ss.getCenterStimulusInfo().setColor(m_sdiOriginal.getBackGroundColor());
		m_ss.getRightStimulusInfo().setIsBackground(true);
		m_ss.getRightStimulusInfo().setColor(m_sdiOriginal.getBackGroundColor());
		qDebug() << "Updated stimulus settings (sound only)";
		qDebug() << m_ss;
		editButtonClicked();
	}
	else
	{
		// restore original
		m_ss = m_ssSaved;
	}
}

void GUILib::HIntertrialIntervalSettingsWidget::rbBackgroundToggled(bool checked)
{
	if (checked)
	{
		m_ssSaved = m_ss;
		m_ss.getLeftStimulusInfo().setIsBackground(true);
		m_ss.getLeftStimulusInfo().setColor(m_sdiOriginal.getBackGroundColor());
		m_ss.getCenterStimulusInfo().setIsBackground(true);
		m_ss.getCenterStimulusInfo().setColor(m_sdiOriginal.getBackGroundColor());
		m_ss.getRightStimulusInfo().setIsBackground(true);
		m_ss.getRightStimulusInfo().setColor(m_sdiOriginal.getBackGroundColor());
		emit stimulusSettingsChanged();
	}
}

void GUILib::HIntertrialIntervalSettingsWidget::rbStimulusToggled(bool checked)
{
	if (checked)
	{
		m_ss = m_ssSaved;
		editButtonClicked();
	}
}


bool GUILib::HIntertrialIntervalSettingsWidget::isSoundOnlyAG()
{
	return (ui->rbUseIntertrialInterval->isChecked() && ui->rbSound->isChecked());
}

Habit::AttentionGetterSettings GUILib::HIntertrialIntervalSettingsWidget::getAttentionGetterSettings()
{
	Habit::AttentionGetterSettings ags;
	ags.setId(m_agsOriginal.getId());
	ags.setAttentionGetterStimulus(m_ss);
	if (ui->rbNoIntertrialInterval->isChecked())
	{
		ags.setIsNoISI(true);
		ags.setIsFixedISI(false);
		ags.setFixedISIMS(ui->leFixedTimeMS->text().toInt());
		ags.setIsSoundOnly(false);
		ags.setUseAttentionGetter(false);
	}
	else
	{
		if (ui->rbFixedTimeInterval->isChecked())
		{
			ags.setIsNoISI(false);
			ags.setIsFixedISI(true);
			ags.setFixedISIMS(ui->leFixedTimeMS->text().toInt());
			ags.setIsSoundOnly(false);
			ags.setUseAttentionGetter(false);
		}
		else
		{
			if (ui->rbStimulus->isChecked())
			{
				ags.setIsNoISI(false);
				ags.setIsFixedISI(false);
				ags.setFixedISIMS(ui->leFixedTimeMS->text().toInt());
				ags.setIsSoundOnly(false);
				ags.setUseAttentionGetter(true);
			}
			else if (ui->rbBackground->isChecked())
			{
				ags.setIsNoISI(false);
				ags.setIsFixedISI(false);
				ags.setFixedISIMS(ui->leFixedTimeMS->text().toInt());
				ags.setIsSoundOnly(false);
				ags.setUseAttentionGetter(true);		// looks like 'useStimulus' option, but stimulus settings forced to be bkgd here
			}
			else if (ui->rbSound->isChecked())
			{
				ags.setIsNoISI(false);
				ags.setIsFixedISI(false);
				ags.setFixedISIMS(ui->leFixedTimeMS->text().toInt());
				ags.setIsSoundOnly(true);
				ags.setUseAttentionGetter(false);
			}
		}
	}
	return ags;
}


void GUILib::HIntertrialIntervalSettingsWidget::editButtonClicked()
{
	GUILib::HStimulusSettingsEditor *pEditor = NULL;

	// choose editor based on current ISI selection
	if (ui->rbStimulus->isChecked() || ui->rbFixedTimeInterval->isChecked())
	{
		pEditor = new GUILib::HStimulusSettingsEditor(m_ss, m_sdiOriginal);
	}
	else if (ui->rbSound->isChecked())
	{
		pEditor = new GUILib::HStimulusSettingsEditor(m_ss, HPlayerPositionType::Sound);
	}
	else
	{
		qCritical() << "AttentionSetupForm::editButtonClicked() - unhandled configuration!";
		Q_ASSERT(true);
		return;
	}

	if (pEditor->exec() == QDialog::Accepted)
	{
		Habit::StimulusSettings ss = pEditor->getStimulusSettings();
		if (ss !=  m_ss)
		{
			m_ss = ss;
			qDebug() << "dialog accepted, set ss in m_ss:";
			qDebug() << m_ss;
			emit stimulusSettingsChanged();
		}
	}
}


