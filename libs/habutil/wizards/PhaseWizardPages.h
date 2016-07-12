/*
 * PhaseWPFirst.h
 *
 *  Created on: Jul 12, 2016
 *      Author: dan
 */

#ifndef PHASEWPFIRST_H_
#define PHASEWPFIRST_H_

#include <QWizardPage>

namespace Ui
{
	class PhaseWPFirst;
	class PhaseWPFamiliarizationSelect;
	class PhaseWPMinLooking;
	class PhaseWPHabit1;
	class PhaseWPHabit2;
	class PhaseWPFixedStim;
}


namespace GUILib
{
	class PhaseWPFirst : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPFirst(QWidget* w = 0);
		~PhaseWPFirst();

	private:
		Ui::PhaseWPFirst *ui;

	};

	class PhaseWPFamiliarizationSelect : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPFamiliarizationSelect(QWidget* w = 0);
		~PhaseWPFamiliarizationSelect();

	private:
		Ui::PhaseWPFamiliarizationSelect *ui;

	};

	class PhaseWPMinLooking : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPMinLooking(QWidget* w = 0);
		~PhaseWPMinLooking();

	private:
		Ui::PhaseWPMinLooking *ui;

	};

	class PhaseWPHabit1 : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPHabit1(QWidget* w = 0);
		~PhaseWPHabit1();

	private:
		Ui::PhaseWPHabit1 *ui;

	};

	class PhaseWPHabit2 : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPHabit2(QWidget* w = 0);
		~PhaseWPHabit2();

	private:
		Ui::PhaseWPHabit2 *ui;

	};

	class PhaseWPFixedStim : public QWizardPage
	{
		Q_OBJECT

	public:
		PhaseWPFixedStim(QWidget* w = 0);
		~PhaseWPFixedStim();

	private:
		Ui::PhaseWPFixedStim *ui;

	};


};



#endif /* PHASEWPFIRST_H_ */
