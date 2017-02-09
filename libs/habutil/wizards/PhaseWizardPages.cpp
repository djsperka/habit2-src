/*
 * PhaseWizardPages.cpp
 *
 *  Created on: Jul 12, 2016
 *      Author: dan
 */

#include "PhaseWizardPages.h"
#include "ui_PhaseWPFirst.h"
#include "ui_PhaseWPFamPref.h"
#include "ui_PhaseWPTrialSettings.h"
#include "ui_PhaseWPTrial.h"
#include "ui_PhaseWPHabituation.h"
#include "ui_PhaseWPHabit1.h"
#include "ui_PhaseWPHabit2.h"

#include <QRegExpValidator>
#include <QIntValidator>
#include <QPixmap>
#include <QMessageBox>
#include <QStringList>
#include <QVBoxLayout>
#include "HTypes.h"
#include "HHabituationCriteriaWidget.h"
#include "HPhaseSettings.h"
#include "habituationsettings.h"
#include "HNameValidator.h"

static const QString sreWordWithSpaces("((\\w)|(\\w[ '_-\\w]*\\w))");
static const QRegExp reWordWithSpaces(sreWordWithSpaces);

static const int pageFirst = 1;
static const int pageTrialSettings = 2;
static const int pageTrial = 2;
static const int pageHabit0 = 4;
static const int pageHabit1 = 5;
static const int pageHabit2 = 6;

namespace GUILib
{
	PhaseWizard::PhaseWizard(const QStringList& phaseNames, QWidget *parent)
	: QWizard(parent)
	{
		setWizardStyle(QWizard::MacStyle);
		setPixmap(QWizard::BackgroundPixmap, QPixmap(":/resources/logo/habit-large.png").scaled(256, 256));
		setPage(pageFirst, (wpFirst = new PhaseWPFirst(phaseNames, this)));
		//setPage(pageTrialSettings, (wpTrialSettings = new PhaseWPTrialSettings(this)));
		setPage(pageTrial, (wpTrial = new PhaseWPTrial(this)));
		setPage(pageHabit0, wpHabit0 = new PhaseWPHabit0(this));
		setPage(pageHabit1, wpHabit1 = new PhaseWPHabit1(this));
		setPage(pageHabit2, wpHabit2 = new PhaseWPHabit2(this));
	}


	Habit::HPhaseSettings PhaseWizard::getHPhaseSettings()
	{
		Habit::HPhaseSettings ps;
		ps.setIsEnabled(true);

		ps.setName(field("PhaseName").toString());
		if (field("isMaxAccumulatedLookTime").toBool())
		{
			ps.setUseLookingCriteria(true);
			ps.setIsMaxAccumulatedLookTime(true);
			ps.setIsSingleLook(false);
			ps.setMaxAccumulatedLookTime(field("MaxAccumulatedLookTime").toInt());
		}
		else
		{
			ps.setUseLookingCriteria(false);
			ps.setIsMaxAccumulatedLookTime(false);
			ps.setIsSingleLook(false);
		}

		if (field("isMaxStimulusTime").toBool())
		{
			ps.setIsMaxStimulusTime(true);
			ps.setMaxStimulusTime(field("MaxStimulusTime").toInt());
			ps.setMeasureStimulusTimeFromOnset(field("isMeasuredFromStimulusOnset").toBool());
			ps.setMeasureStimulusTimeFromLooking(field("isMeasuredFromInitialLooking").toBool());
		}
		else
		{
			ps.setIsMaxStimulusTime(false);
		}

		if (field("FamPref").toBool())
		{
			ps.setHabituationSettings(wpHabit0->getHabituationSettings());
		}
		else
		{
			if (field("HabReduction").toBool())
			{
				ps.setHabituationSettings(wpHabit1->getHabituationSettings());
			}
			else
			{
				ps.setHabituationSettings(wpHabit2->getHabituationSettings());
			}
		}
		return ps;
	}



	PhaseWPFirst::PhaseWPFirst(const QStringList& phaseNames, QWidget *parent)
	: QWizardPage(parent)
	, ui(new Ui::PhaseWPFirst)
	, m_phaseNames(phaseNames)
	{
		ui->setupUi(this);

		// default selection is familiarization/preference
		ui->rbFamiliarizationPreference->setChecked(true);

		// validator for phase name
		ui->lePhaseName->setValidator(new HNameValidator());

		// connect rbHabituation with gbHabituation enabled state
		connect(ui->rbHabituation, SIGNAL(toggled(bool)), ui->gbHabituationType, SLOT(setEnabled(bool)));

		// register field for completeness
		registerField("PhaseName*", ui->lePhaseName);
		registerField("FamPref", ui->rbFamiliarizationPreference);
		registerField("Habituation", ui->rbHabituation);
		registerField("HabReduction", ui->rbHabReduction);
		registerField("HabAccum", ui->rbHabAccum);

		ui->rbFamiliarizationPreference->setChecked(true);
		ui->rbHabReduction->setChecked(true);
		ui->gbHabituationType->setEnabled(false);

	}

	int PhaseWPFirst::nextId() const
	{
		return pageTrialSettings;
	}

	bool PhaseWPFirst::validatePage()
	{
		bool b = true;

		// check phase name against existing names
		if (ui->lePhaseName->text().trimmed().isEmpty())
		{
			b = false;
			QMessageBox::warning(this, "Enter phase name", "Please enter a phase name.");
		}
		else if (m_phaseNames.contains(ui->lePhaseName->text()))
		{
			b = false;
			QMessageBox::warning(this, "Duplicate phase name", "The phase name you entered is already used. Please enter a new phase name.");
		}
		return b;
	}

	QString PhaseWPFirst::getName()
	{
		return ui->lePhaseName->text().trimmed();
	}

	bool PhaseWPFirst::getIsFamPrefType()
	{
		return ui->rbFamiliarizationPreference->isChecked();
	}

	bool PhaseWPFirst::getIsHabType()
	{
		return ui->rbHabituation->isChecked();
	}

	PhaseWPTrial::PhaseWPTrial(QWidget* parent)
	: QWizardPage(parent)
	, ui(new Ui::PhaseWPTrial)
	{
		ui->setupUi(this);
		ui->leMaxStimulusTime->setValidator(new QIntValidator(1, 1000000));
		ui->leMaxAccumulatedLookTime->setValidator(new QIntValidator(1, 1000000));

		// register fields
		registerField("isMaxAccumulatedLookTime", ui->cbMaxAccumulatedLookTime);
		registerField("MaxAccumulatedLookTime", ui->leMaxAccumulatedLookTime);
		registerField("isMaxStimulusTime", ui->cbMaxStimulusTime);
		registerField("MaxStimulusTime", ui->leMaxStimulusTime);
		registerField("isMeasuredFromStimulusOnset", ui->rbMeasuredFromStimulusOnset);
		registerField("isMeasuredFromInitialLooking", ui->rbMeasuredFromInitialLooking);
	}

	int PhaseWPTrial::nextId() const
	{
		if (field("FamPref").toBool()) return -1;
		else
		{
			if (field("HabReduction").toBool())
				return pageHabit1;
			else
				return pageHabit2;
		}
	}

	void PhaseWPTrial::initializePage()
	{
		// In habituation type trials, hide the "Number of Trials" spinbox
		if (field("Habituation").toBool())
		{
			ui->frameNTrials->hide();
		}
		else
		{
			ui->frameNTrials->show();
		}
	}

	bool PhaseWPTrial::validatePage()
	{
		bool b = true;
		//if (!ui->gbMaxStimulusTime->isChecked() && !ui->gbMinLookingTime->isChecked()) b = false;
		if (!(field("isMaxStimulusTime").toBool()) && !(field("isMaxAccumulatedLookTime").toBool())) b = false;
		else
		{
			// One of the two is checked.
			//if (ui->gbMinLookingTime->isChecked() && !ui->gbMaxStimulusTime->isChecked())
			if ((field("isMaxAccumulatedLookTime").toBool()) && !(field("isMaxStimulusTime").toBool()))
			{
				b = (QMessageBox::Yes == QMessageBox::warning(this, "Are you sure?", "You have specified minimum looking time, but you have not specified a maximum stimulus time. If your subject does not look at the stimulus, your trial(s) may never end! Are you sure you want to create a new phase without specifying a maximum stimulus time?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No));
			}
		}
		return b;
	}


#if 0
	//dddddd
	PhaseWPTrialSettings::PhaseWPTrialSettings(QWidget* parent)
	: QWizardPage(parent)
	, ui(new Ui::PhaseWPTrialSettings)
	{
		ui->setupUi(this);
		ui->leMaxStimulusTime->setValidator(new QIntValidator(1, 1000000));
		ui->leMinLookingTime->setValidator(new QIntValidator(1, 1000000));

		// register fields

	}

	int PhaseWPTrialSettings::nextId() const
	{
		if (field("FamPref").toBool()) return pageHabit0;
		else
		{
			if (field("HabReduction").toBool())
				return pageHabit1;
			else
				return pageHabit2;
		}
	}
	bool PhaseWPTrialSettings::validatePage()
	{
		bool b = true;
		if (!ui->gbMaxStimulusTime->isChecked() && !ui->gbMinLookingTime->isChecked()) b = false;
		else
		{
			// One of the two is checked.
			if (ui->gbMinLookingTime->isChecked() && !ui->gbMaxStimulusTime->isChecked())
			{
				b = (QMessageBox::Yes == QMessageBox::warning(this, "Are you sure?", "You have specified minimum looking time, but you have not specified a maximum stimulus time. If your subject does not look at the stimulus, your trial(s) may never end! Are you sure you want to create a new phase without specifying a maximum stimulus time?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No));
			}
		}
		return b;
	}


	bool PhaseWPTrialSettings::isMaxAccumulatedLookTime()
	{
		return ui->gbMinLookingTime->isChecked();
	}

	int PhaseWPTrialSettings::getMaxAccumulatedLookTime()
	{
		return ui->leMinLookingTime->text().toInt();
	}

	bool PhaseWPTrialSettings::isMaxStimulusTime()
	{
		return ui->gbMaxStimulusTime->isChecked();
	}

	int PhaseWPTrialSettings::getMaxStimulusTime()
	{
		return ui->leMaxStimulusTime->text().toInt();
	}

	bool PhaseWPTrialSettings::getMeasureStimulusTimeFromOnset()
	{
		return ui->rbMeasuredFromStimulusOnset->isChecked();
	}

	bool PhaseWPTrialSettings::getMeasureStimulusTimeFromLooking()
	{
		return ui->rbMeasuredFromInitialLooking->isChecked();
	}
#endif

	PhaseWPHabituation::PhaseWPHabituation(QWidget *parent)
	: QWizardPage(parent)
	, ui(new Ui::PhaseWPHabituation)
	{
		ui->setupUi(this);
	}


	int PhaseWPHabituation::nextId() const
	{
		if (ui->rbHabReduction->isChecked())
			return pageHabit1;
		else
			return pageHabit2;
	}


	PhaseWPHabit0::PhaseWPHabit0(QWidget *p)
	: QWizardPage(p)
	{
		setTitle("Familiarization/Preference type phase");
		setSubTitle("Specify the number of trials for this phase.");
		m_pCriteriaWidget = new GUILib::HHabituationCriteriaWidget();
		m_pCriteriaWidget->setCurrentIndex(0);
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(m_pCriteriaWidget);
		setLayout(vbox);
	}

	Habit::HabituationSettings PhaseWPHabit0::getHabituationSettings()
	{
		Habit::HabituationSettings hs(HHabituationType::HHabituationTypeFixedN);
		hs.setNTrials(m_pCriteriaWidget->getNTrials());
		hs.setTotalLookLengthToEnd(m_pCriteriaWidget->getTotalLookLength());
		hs.setCriterionSettings(m_pCriteriaWidget->getCriterionSettings());
		return hs;
	}

	PhaseWPHabit1::PhaseWPHabit1(QWidget *p)
	: QWizardPage(p)
	{
		setTitle("Habituation by looking time reduction");
		setSubTitle("Specify the maximum number of trials, window type, percentage, and window size.");
		m_pCriteriaWidget = new GUILib::HHabituationCriteriaWidget();
		m_pCriteriaWidget->setCurrentIndex(1);	// habituation criteria page
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(m_pCriteriaWidget);
		setLayout(vbox);
	}

	Habit::HabituationSettings PhaseWPHabit1::getHabituationSettings()
	{
		Habit::HabituationSettings hs(HHabituationType::HHabituationTypeCriterion);
		hs.setNTrials(m_pCriteriaWidget->getNTrials());
		hs.setTotalLookLengthToEnd(m_pCriteriaWidget->getTotalLookLength());
		hs.setCriterionSettings(m_pCriteriaWidget->getCriterionSettings());
		return hs;
	}

	PhaseWPHabit2::PhaseWPHabit2(QWidget *p)
	: QWizardPage(p)
	{
		setTitle("Habituation by cumulative looking time");
		setSubTitle("Specify the maximum number of trials, and total looking time required for habituation.");
		m_pCriteriaWidget = new GUILib::HHabituationCriteriaWidget();
		m_pCriteriaWidget->setCurrentIndex(2);	// habituation criteria page
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(m_pCriteriaWidget);
		setLayout(vbox);
	}

	Habit::HabituationSettings PhaseWPHabit2::getHabituationSettings()
	{
		Habit::HabituationSettings hs(HHabituationType::HHabituationTypeTotalLookingTime);
		hs.setNTrials(m_pCriteriaWidget->getNTrials());
		hs.setTotalLookLengthToEnd(m_pCriteriaWidget->getTotalLookLength());
		hs.setCriterionSettings(m_pCriteriaWidget->getCriterionSettings());
		return hs;
	}

}


