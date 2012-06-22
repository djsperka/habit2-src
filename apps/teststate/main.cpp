/*
 *  main.cpp
 *  hstate
 *
 *  Created by Oakes Lab on 6/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


/*
 *  main.cpp
 *  state1
 *
 *  Created by Oakes Lab on 6/5/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#include <QObject>
#include <QtGui>
#include <QFinalState>
#include <QEvent>
#include <QEventTransition>
#include "HStateMachine.h"
#include "HStartTrialState.h"
#include "HAGState.h"
#include "MediaPlayerDummy.h"
#include "HEvents.h"
#include "HReadyTransition.h"
#include "HStimRequestState.h"


int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QDialog *dlg = new QDialog();
	QPushButton* button = new QPushButton();
	QVBoxLayout *layout = new QVBoxLayout(dlg);
	QLabel *label = new QLabel();
	QLabel *labelLook = new QLabel();
	layout->addWidget(button);
	layout->addWidget(label);
	layout->addWidget(labelLook);
	
	// Create media player

	MediaPlayerDummy* mpd = new MediaPlayerDummy();	
	
	// Create state machine
	
	HStateMachine* machine = new HStateMachine();
	
	// Create all states

	HStartTrialState* stateStart = new HStartTrialState();
	HAGState* stateAG = new HAGState();
	HState* stateAGRunning = new HState("stateAGRunning");
	HState* stateStimRequest = new HStimRequestState();
	HState* stateStimRunning = new HState("stateStimRunning");
	QFinalState* stateFinal = new QFinalState();

	// add states to machine
	
	machine->addState(stateStart);
	machine->addState(stateAG);
	machine->addState(stateAGRunning);
	machine->addState(stateStimRequest);
	machine->addState(stateStimRunning);
	machine->addState(stateFinal);
	
	// make label (see above) display current state
	stateStart->assignProperty(label, "text", "state: Start");
	stateAG->assignProperty(label, "text", "state: AG");
	stateAGRunning->assignProperty(label, "text", "state: AGRunning");
	stateStimRequest->assignProperty(label, "text", "state: stimRequest");
	stateStimRunning->assignProperty(label, "text", "state: stimRunning");
	
	
	// Set up transitions.

	HAGTrialTransition* transHAG = new HAGTrialTransition();
	transHAG->setTargetState(stateAG);
	stateStart->addTransition(transHAG);
	stateAG->addTransition(mpd, SIGNAL(agStarted()), stateAGRunning);
	HReadyTransition* transReady = new HReadyTransition(dlg);
	transReady->setTargetState(stateStimRequest);
	stateAGRunning->addTransition(transReady);

	// transitions within stateStimLook
	stateStimRequest->addTransition(mpd, SIGNAL(stimStarted()), stateStimRunning);
	
	
	
	
	// initial state
	
	machine->setInitialState(stateStart);

	// connections
	
	QObject::connect(stateAG, SIGNAL(playAG()), mpd, SLOT(play_attention_getter()));
	QObject::connect(stateStimRequest, SIGNAL(playStim()), mpd, SLOT(play_stim()));

	// start machine 
	
	machine->start();	
	
	dlg->show();
	return app.exec();
}
