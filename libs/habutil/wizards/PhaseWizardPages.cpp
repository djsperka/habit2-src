/*
 * PhaseWizardPages.cpp
 *
 *  Created on: Jul 12, 2016
 *      Author: dan
 */

#include "PhaseWizardPages.h"
#include "ui_PhaseWPFirst.h"
#include "ui_PhaseWPFamPref.h"
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

static const QString sreWordWithSpaces("((\\w)|(\\w[ '_-\\w]*\\w))");
static const QRegExp reWordWithSpaces(sreWordWithSpaces);

static const int pageFirst = 1;
static const int pageFamPref = 2;
static const int pageHabituation = 3;
static const int pageHabit1 = 4;
static const int pageHabit2 = 5;

namespace GUILib
{
	PhaseWizard::PhaseWizard(const QStringList& phaseNames, QWidget *parent)
	: QWizard(parent)
	{
		setWizardStyle(QWizard::MacStyle);
		setPixmap(QWizard::BackgroundPixmap, QPixmap(":/resources/logo/habit-large.png").scaled(256, 256));
		setPage(pageFirst, (wpFirst = new PhaseWPFirst(phaseNames, this)));
		setPage(pageFamPref, (wpFamPref = new PhaseWPFamPref(this)));
		setPage(pageHabituation, (wpHabituation = new PhaseWPHabituation(this)));
		setPage(pageHabit1, (wpHabit1 = new PhaseWPHabit1(this)));
		//setPage(pageHabit1, new PhaseWPHabit1(this));
		setPage(pageHabit2, createHabit2Page());
		//setPage(pageHabit2, new PhaseWPHabit2(this));
	}

#if 0
	QWizardPage* PhaseWizard::createHabit1Page()
	{
		QWizardPage* page = new QWizardPage;
		page->setTitle("Habituation by looking time reduction");
		page->setSubTitle("Specify the maximum number of trials, window type, percentage, and window size.");
		GUILib::HHabituationCriteriaWidget* pc = new GUILib::HHabituationCriteriaWidget();
		pc->setCurrentIndex(1);	// habituation criteria page
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(pc);
		page->setLayout(vbox);
		return page;
	}
#endif

	QWizardPage* PhaseWizard::createHabit2Page()
	{
		QWizardPage* page = new QWizardPage;
		page->setTitle("Habituation by total looking time summed over all trials");
		page->setSubTitle("Specify the required looking time and maximum number of trials.");
		GUILib::HHabituationCriteriaWidget* pc = new GUILib::HHabituationCriteriaWidget();
		pc->setCurrentIndex(2);	// total looking page
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->addWidget(pc);
		page->setLayout(vbox);
		return page;
	}

	Habit::HPhaseSettings PhaseWizard::getHPhaseSettings()
	{
		Habit::HPhaseSettings ps;
		ps.setIsEnabled(true);
		ps.setName(wpFirst->getName());
		if (wpFirst->getIsFamPrefType())
		{
			if (wpFamPref->isMaxAccumulatedLookTime())
			{
				ps.setUseLookingCriteria(true);
				ps.setIsMaxAccumulatedLookTime(true);
				ps.setIsSingleLook(false);
				ps.setMaxAccumulatedLookTime(wpFamPref->getMaxStimulusTime());
			}
			else
			{
				ps.setUseLookingCriteria(false);
				ps.setIsMaxAccumulatedLookTime(false);
				ps.setIsSingleLook(false);
			}

			if (wpFamPref->isMaxStimulusTime())
			{
				ps.setIsMaxStimulusTime(true);
				ps.setMaxStimulusTime(wpFamPref->getMaxAccumulatedLookTime());
				ps.setMeasureStimulusTimeFromOnset(wpFamPref->getMeasureStimulusTimeFromOnset());
				ps.setMeasureStimulusTimeFromLooking(wpFamPref->getMeasureStimulusTimeFromLooking());
			}
			else
			{
				ps.setIsMaxStimulusTime(false);
			}

			Habit::HabituationSettings habituationSettings;
			habituationSettings.setHabituationType(HHabituationType::HHabituationTypeFixedN);
			habituationSettings.setNTrials(wpFamPref->getNTrials());
			ps.setHabituationSettings(habituationSettings);

		}

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
		ui->lePhaseName->setValidator(new QRegExpValidator(reWordWithSpaces));

		// register field for completeness
		registerField("PhaseName*", ui->lePhaseName);

	}

	int PhaseWPFirst::nextId() const
	{
		if (ui->rbFamiliarizationPreference->isChecked())
			return pageFamPref;
		else
			return pageHabituation;
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

	PhaseWPFamPref::PhaseWPFamPref(QWidget* parent)
	: QWizardPage(parent)
	, ui(new Ui::PhaseWPFamPref)
	{
		ui->setupUi(this);
		ui->leMaxStimulusTime->setValidator(new QIntValidator(1, 1000000));
		ui->leMinLookingTime->setValidator(new QIntValidator(1, 1000000));
	}

	bool PhaseWPFamPref::validatePage()
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


	bool PhaseWPFamPref::isMaxAccumulatedLookTime()
	{
		return ui->gbMinLookingTime->isChecked();
	}

	int PhaseWPFamPref::getMaxAccumulatedLookTime()
	{
		return ui->leMinLookingTime->text().toInt();
	}

	bool PhaseWPFamPref::isMaxStimulusTime()
	{
		return ui->gbMaxStimulusTime->isChecked();
	}

	int PhaseWPFamPref::getMaxStimulusTime()
	{
		return ui->leMaxStimulusTime->text().toInt();
	}

	bool PhaseWPFamPref::getMeasureStimulusTimeFromOnset()
	{
		return ui->rbMeasuredFromStimulusOnset->isChecked();
	}

	bool PhaseWPFamPref::getMeasureStimulusTimeFromLooking()
	{
		return ui->rbMeasuredFromInitialLooking->isChecked();
	}

	int PhaseWPFamPref::getNTrials()
	{
		return ui->sbNTrials->value();
	}

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

#if 0
	PhaseWPHabit1::PhaseWPHabit1(QWidget *parent)
	: QWizardPage(parent)
	, ui(new Ui::PhaseWPHabit1)
	{
		ui->setupUi(this);

		// populate window type combo
		QStringList items;
		items << HCriterionWindowType::HCriterionWindowFixed.label() << HCriterionWindowType::HCriterionWindowSliding.label();
		ui->comboWindowType->addItems(items);

		// default selection is first window
		ui->rbUseFirstWindow->setChecked(true);

		// validator for min basis value
		ui->lineeditMinBasisValue->setValidator(new QIntValidator(1, 1000000));
	}

	PhaseWPHabit2::PhaseWPHabit2(QWidget *parent)
	: QWizardPage(parent)
	, ui(new Ui::PhaseWPHabit2)
	{
		ui->setupUi(this);
		ui->leTotalLookingTime->setValidator(new QIntValidator(1, 1000000));
	}
#endif

}


