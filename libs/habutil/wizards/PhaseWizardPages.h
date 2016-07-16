/*
 * PhaseWPFirst.h
 *
 *  Created on: Jul 12, 2016
 *      Author: dan
 */

#ifndef PHASEWPFIRST_H_
#define PHASEWPFIRST_H_

#include <QWizard>
#include <QWizardPage>
#include <QStringList>

namespace Ui
{
	class PhaseWPFirst;					// id = 1
	class PhaseWPFamPref; 				// id = 2
	class PhaseWPHabituation;			// id = 3
	class PhaseWPHabit1;				// id = 4
	class PhaseWPHabit2;				// id = 5
}

namespace Habit
{
	class HPhaseSettings;
}

namespace GUILib
{
	// forward declaratins
	class HHabituationCriteriaWidget;
	class PhaseWPFirst;
	class PhaseWPFamPref;
	class PhaseWPHabituation;
	class PhaseWPHabit1;
	class PhaseWPHabit2;

	class PhaseWizard: public QWizard
	{
		Q_OBJECT

	public:
		PhaseWizard(const QStringList& phaseNames, QWidget *parent = 0);
		~PhaseWizard() {};
		//QWizardPage* createHabit1Page();
		QWizardPage* createHabit2Page();
		Habit::HPhaseSettings getHPhaseSettings();

	private:
		GUILib::PhaseWPFirst* wpFirst;
		GUILib::PhaseWPFamPref* wpFamPref;
		GUILib::PhaseWPHabituation* wpHabituation;
		GUILib::PhaseWPHabit1* wpHabit1;
		GUILib::PhaseWPHabit2* wpHabit2;
	};

	class PhaseWPFirst : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPFirst(const QStringList& phaseNames, QWidget* w = 0);
		~PhaseWPFirst() {};
		int nextId() const;
		bool validatePage();
		QString getName();
		bool getIsFamPrefType();	// this will set habituation type to FixedN
		bool getIsHabType();		// this means habituation type is criterion or totalLookTime

	private:
		Ui::PhaseWPFirst *ui;
		QStringList m_phaseNames;

	};

	class PhaseWPFamPref : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPFamPref(QWidget* w = 0);
		~PhaseWPFamPref() {};
		int nextId() const { return -1; } // final button activated
		bool validatePage();
		bool isMaxAccumulatedLookTime();
		int getMaxAccumulatedLookTime();
		bool isMaxStimulusTime();
		int getMaxStimulusTime();
		bool getMeasureStimulusTimeFromOnset();
		bool getMeasureStimulusTimeFromLooking();
		int getNTrials();

	private:
		Ui::PhaseWPFamPref *ui;

	};

	class PhaseWPHabituation : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPHabituation(QWidget* w = 0);
		~PhaseWPHabituation() {};
		int nextId() const;

	private:
		Ui::PhaseWPHabituation *ui;

	};

	class PhaseWPHabit1 : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPHabit1(QWidget* w = 0);
		~PhaseWPHabit1() {};
		int nextId() const { return -1; } // final button activated

	private:
		Ui::PhaseWPHabit1 *ui;
		GUILib::HHabituationCriteriaWidget *m_pCriteriaWidget;
	};

	class PhaseWPHabit2 : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPHabit2(QWidget* w = 0);
		~PhaseWPHabit2() {};
		int nextId() const { return -1; } // final button activated

	private:
		Ui::PhaseWPHabit2 *ui;

	};


};



#endif /* PHASEWPFIRST_H_ */
