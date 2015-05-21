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
#include <QTextStream>
#include "HVideoImagePlayer.h"
#include "HAudioPlayer.h"


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
	pmm = new HMediaManager();
	
	// What screens to show it on?
	iScreen = pspinboxScreen->value();

	if (iScreen == 1 || iScreen == 3)
	{
		HVideoImagePlayer* player1 = new HVideoImagePlayer(1, this);
		
		player1->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov", 10, true);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov", 10);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/mainecoon.jpg", 0);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg", 0);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/multicolor_box_bubble.mov", 10);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg", 0);
		
		out << (HPlayer&)(*player1) << endl;
		
		pmm->addPlayer(player1, 1);
	}
	
	if (iScreen == 2 || iScreen == 3)
	{
		//player2 = new MyPlayer(this);
		HVideoImagePlayer* player2 = new HVideoImagePlayer(2, this);
		player2->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov", 10);
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg", 0);
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/multicolor_box_bubble.mov", 10, true);
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov", 10, true);
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/mainecoon.jpg", 0);
		player2->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg", 0);

		out << (HPlayer&)(*player2) << endl;
		
		pmm->addPlayer(player2, 2);
	}
	
	if (iScreen == 4)
	{
		HVideoImagePlayer* player1 = new HVideoImagePlayer(1, this);
		
		player1->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov", 10, true);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov", 10);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/mainecoon.jpg", 0);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg", 0);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/multicolor_box_bubble.mov", 10);
		player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg", 0);
		
		out << (HPlayer&)(*player1) << endl;
		
		pmm->addPlayer(player1, 1);

		HAudioPlayer* player3 = new HAudioPlayer(0, this);
		player3->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov", 10, true);
		player3->addStimulus("");
		player3->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/dog2.wav", 10, true);
		player3->addStimulus("");
		player3->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/2minbeeping copy.aiff", 10);
		player3->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg", 0);

		out << (HPlayer&)(*player3) << endl;
		
		pmm->addPlayer(player3);
	}
		
	
	connect(this, SIGNAL(playStim(int)), pmm, SLOT(stim(int)));
	connect(pmm, SIGNAL(stimStarted()), this, SLOT(stimStarted()));
	
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


