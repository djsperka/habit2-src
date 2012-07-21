/*
 *  TestLookDialog.cpp
 *  testlook
 *
 *  Created by Oakes Lab on 6/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestTrialDialog.h"
#include <QTextStream>
#include <QIntValidator>
#include <QApplication>
#include "HStateMachine.h"
#include "HabitVideoImagePlayer.h"

TestTrialDialog::TestTrialDialog(QWidget* parent) : QDialog(parent), m_bGoClicked(false)
{
	m_time.start();
	pushbuttonGo = new QPushButton();
	pushbuttonGo->setText("Go");
	listWidget = new QListWidget(this);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(createTrialSettingsGroup());
	layout->addWidget(pushbuttonGo);
	layout->addWidget(listWidget);

	setLayout(layout);
	
	connect(pushbuttonGo, SIGNAL(clicked()), this, SLOT(goClicked()));
	QCoreApplication::instance()->installEventFilter(this);
};

QGroupBox* TestTrialDialog::createTrialSettingsGroup()
{
	QGroupBox* group = new QGroupBox("Trial Settings");
	
	// Trial settings group box is the widget returned. 
	// This widget will have a VBox layout.....
	
	QVBoxLayout* vbox = new QVBoxLayout;
	
	// withing the vbox there will be two checkboxes....
	checkboxFixed = new QCheckBox("Fixed");
	checkboxAG = new QCheckBox("AG");

	// lineedit item for trial length
	QLabel* label = new QLabel("max trial length(ms)");
	lineeditMaxMS = new QLineEdit("5000");
	QHBoxLayout* hlayoutMax = new QHBoxLayout;
	hlayoutMax->addWidget(label);
	hlayoutMax->addWidget(lineeditMaxMS);
	
	// lineedit item for min look time
	label = new QLabel("Min look time ms");
	lineeditLook = new QLineEdit("1000");
	lineeditLook->setValidator(new QIntValidator(1, 10000));
	QHBoxLayout* hlayoutLook = new QHBoxLayout;
	hlayoutLook->addWidget(label);
	hlayoutLook->addWidget(lineeditLook);
	
	// lineedit item for min look away time
	label = new QLabel("min look away time ms");
	lineeditLookAway = new QLineEdit("250");
	lineeditLookAway->setValidator(new QIntValidator(0, 10000));
	QHBoxLayout* hlayoutLookAway = new QHBoxLayout;
	hlayoutLookAway->addWidget(label);
	hlayoutLookAway->addWidget(lineeditLookAway);
	
	
	vbox->addWidget(checkboxFixed);
	vbox->addWidget(checkboxAG);
	vbox->addLayout(hlayoutMax);
	vbox->addLayout(hlayoutLook);
	vbox->addLayout(hlayoutLookAway);
	group->setLayout(vbox);
	
	return group;
}

void TestTrialDialog::goClicked()
{
	bool bValue;
	qDebug() << "goClicked()" << lineeditLook->text().toInt(&bValue) <<  " " << lineeditLookAway->text().toInt(&bValue) << endl;
	pushbuttonGo->setDisabled(true);
	lineeditLook->setDisabled(true);
	lineeditLookAway->setDisabled(true);
	m_bGoClicked = true;

	// create looker object and connect it to our own slot. 
	// note that this connection isn't needed for the trial class to function - we do it to display
	// looks signals only. 
	
	looker = new HLooker(lineeditLook->text().toInt(&bValue), lineeditLookAway->text().toInt(&bValue));
	connect(looker, SIGNAL(look(HLook)), this, SLOT(gotLook(HLook)));

	// create media manager
	HabitMediaManager* pmm = createMediaManager();
	
	// Now create a trial and state machine
	HStateMachine* machine = new HStateMachine();
	trial = new HTrial(this, pmm, looker, lineeditMaxMS->text().toInt(&bValue), checkboxFixed->isChecked(), checkboxAG->isChecked());
	machine->addState(trial);
	QFinalState* sFinal = new QFinalState;
	machine->addState(sFinal);
	trial->addTransition(sFinal);
	machine->setInitialState(trial);
	machine->start();
	
	return;
};

HabitMediaManager* TestTrialDialog::createMediaManager()
{
	// create media manager
	HabitMediaManager* pmm = new HabitMediaManager();

	HabitVideoImagePlayer* player1 = new HabitVideoImagePlayer(1, this);
	QRect rect = QApplication::desktop()->screenGeometry(1);
	player1->setGeometry(rect);
	player1->move(rect.x(), rect.y());
		
	player1->addAG("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/Green AG Movie bell.mov", true);
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/crank_nutchop_crumble.mov");
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/mainecoon.jpg");
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/husky.jpg");
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/multicolor_box_bubble.mov");
	player1->addStimulus("/Users/Oakeslab/Desktop/New Habit Testing Stimuli/van.jpg");
		
	pmm->addPlayer(player1);

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
	
	pmm->addPlayer(player2);
	
	return pmm;
}


void TestTrialDialog::gotLook(HLook l)
{
	QString s;
	QTextStream out(&s);
	out << l;
	qDebug() << "Got look signal " << s;
	new QListWidgetItem(s, listWidget);
}

bool TestTrialDialog::eventFilter(QObject *obj, QEvent *event)
{
	int t = m_time.elapsed();
	QKeyEvent *keyEvent;
	bool bVal = false;
	if (m_bGoClicked) 
	{
		switch (event->type()) 
		{
			case QEvent::KeyPress:
				keyEvent = static_cast<QKeyEvent*>(event);
				if (keyEvent->isAutoRepeat())
				{
					bVal = true;
					//qDebug() << "Ignore auto-repeat " << keyEvent->key();
				}
				else 
				{
					qDebug() << "Ate key press " << keyEvent->key();
					switch (keyEvent->key()) 
					{
						case Qt::Key_4:
							looker->addTrans(NoneLeft, t);
							bVal = true;
							break;
						case Qt::Key_6:
							looker->addTrans(NoneRight, t);
							bVal = true;
							break;
						default:
							break;
					}
				}
				break;
			case QEvent::KeyRelease:
				keyEvent = static_cast<QKeyEvent*>(event);
				qDebug() << "Ate key release " << keyEvent->key();
				switch (keyEvent->key()) {
					case Qt::Key_4:
						looker->addTrans(LeftNone, t);
						bVal = true;
						break;
					case Qt::Key_6:
						looker->addTrans(RightNone, t);
						bVal = true;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	else 
	{
		// pass the event on to the parent class
		bVal = QDialog::eventFilter(obj, event);
	}
	return bVal;
}