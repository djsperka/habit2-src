/*
 *  HLookTransition.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef HLOOKTRANSITION_H
#define HLOOKTRANSITION_H

#include <QtGui>

class HLookTransition : public QEventTransition
{
	Q_OBJECT
	
public:

	HLookTransition() : QEventTransition() {};
	~HLookTransition() {};
#if 0
	HLookTransition(Window *window) :
	QEventTransition(window, QEvent::KeyPress) {};
#endif
	
protected:
	bool eventTest(QEvent* e) 
	{ 
		Q_UNUSED(e);
		return true; 
	};
	void onTransition(QEvent* e) 
	{
		Q_UNUSED(e);
	};

#if 0
	bool eventTest(QEvent *event) {
		if (event->type() == QEvent::StateMachineWrapped &&
			static_cast<QStateMachine::WrappedEvent *>(event)->event()->type() == QEvent::KeyPress) {
			QEvent *wrappedEvent = static_cast<QStateMachine::WrappedEvent *>(event)->event();
			
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(wrappedEvent);
			int key = keyEvent->key();
			
			return key == Qt::Key_2 || key == Qt::Key_8 || key == Qt::Key_6 ||
			key == Qt::Key_4;
		}
		return false;
	}
	
	void onTransition(QEvent *event) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(
													   static_cast<QStateMachine::WrappedEvent *>(event)->event());
		
		int key = keyEvent->key();
		switch (key) {
			case Qt::Key_4:
				window->movePlayer(Window::Left);
				break;
			case Qt::Key_8:
				window->movePlayer(Window::Up);
				break;
			case Qt::Key_6:
				window->movePlayer(Window::Right);
				break;
			case Qt::Key_2:
				window->movePlayer(Window::Down);
				break;
			default:
				;
		}
	}
	
private:
	Window *window;
#endif
};

#endif