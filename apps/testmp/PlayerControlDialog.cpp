/*
 *  PlayerControlDialog.cpp
 *  myp
 *
 *  Created by Oakes Lab on 5/17/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlayerControlDialog.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextStream>
#include "HabitVideoImagePlayer.h"
#include "HabitAudioPlayer.h"


PlayerControlDialog::PlayerControlDialog(QWidget* w) : QDialog(w)
{
	// Create button
	pbtnCreate = new QPushButton("Create", this);

	// spinbox
	pspinboxScreen = new QSpinBox(this);
	pspinboxScreen->setMaximum(4);	

	// show button et al
	pbtnShow = new QPushButton("Show", this);
	pspinboxShow = new QSpinBox(this);
	pspinboxShow->setMaximum(6);
	
	// connections
	connect(pbtnCreate, SIGNAL(clicked()), this, SLOT(btnCreateClicked()));
	connect(pbtnShow, SIGNAL(clicked()), this, SLOT(btnShowClicked()));

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QHBoxLayout* layout1 = new QHBoxLayout;
	layout1->addWidget(pbtnCreate);
	layout1->addWidget(pspinboxScreen);

	QHBoxLayout* layout2 = new QHBoxLayout;
	layout2->addWidget(pbtnShow);
	layout2->addWidget(pspinboxShow);
	
	mainLayout->addLayout(layout1);
	mainLayout->addLayout(layout2);

}

PlayerControlDialog::~PlayerControlDialog()
{
}

void PlayerControlDialog::btnCreateClicked()
{
	int iScreen = 0;
	QRect rect;
	QString filename("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov");
	QTextStream out(stdout);
	
	// create media manager
	pmm = new HabitMediaManager();
	
	// What screens to show it on?
	iScreen = pspinboxScreen->value();

	if (iScreen == 1 || iScreen == 3)
	{
		HabitVideoImagePlayer* player1 = new HabitVideoImagePlayer(1, this);
		rect = QApplication::desktop()->screenGeometry(1);
		player1->setGeometry(rect);
		player1->move(rect.x(), rect.y());
		
		player1->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov", true);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov");
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/mainecoon.jpg");
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg");
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/multicolor_box_bubble.mov");
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg");
		
		out << (HabitPlayer&)(*player1) << endl;
		
		pmm->addPlayer(player1);
	}
	
	if (iScreen == 2 || iScreen == 3)
	{
		//player2 = new MyPlayer(this);
		HabitVideoImagePlayer* player2 = new HabitVideoImagePlayer(2, this);
		rect = QApplication::desktop()->screenGeometry(2);
		player2->setGeometry(rect);
		player2->move(rect.x(), rect.y());
		player2->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov");
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg");
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/multicolor_box_bubble.mov", true);
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov", true);
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/mainecoon.jpg");
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg");

		out << (HabitPlayer&)(*player2) << endl;
		
		pmm->addPlayer(player2);
	}
	
	if (iScreen == 4)
	{
		HabitVideoImagePlayer* player1 = new HabitVideoImagePlayer(1, this);
		rect = QApplication::desktop()->screenGeometry(1);
		player1->setGeometry(rect);
		player1->move(rect.x(), rect.y());
		
		player1->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov", true);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov");
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/mainecoon.jpg");
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg");
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/multicolor_box_bubble.mov");
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg");
		
		out << (HabitPlayer&)(*player1) << endl;
		
		pmm->addPlayer(player1);

		HabitAudioPlayer* player3 = new HabitAudioPlayer(0, this);
		player3->setGeometry(QRect(0, 0, 0, 0));
		player3->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov", true);
		player3->addStimulus("");
		player3->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/dog2.wav", true);
		player3->addStimulus("");
		player3->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/2minbeeping copy.aiff");
		player3->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg");

		out << (HabitPlayer&)(*player3) << endl;
		
		pmm->addPlayer(player3);
	}
		
	
	connect(this, SIGNAL(playStim(int)), pmm, SLOT(stim(int)));
	connect(pmm, SIGNAL(started()), this, SLOT(stimStarted()));
	
	// disable create button and spinbox
	pbtnCreate->setEnabled(false);
	pspinboxScreen->setEnabled(false);
	
}

void PlayerControlDialog::btnShowClicked()
{
	int which = pspinboxShow->value();
	emit playStim(which);
}


void PlayerControlDialog::btnStopClicked()
{
	// TODO
}

void PlayerControlDialog::stimStarted()
{
	qDebug() << "Got stim started ";
}


