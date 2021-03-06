/*
 *  HKeypadLookDetector.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 8/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HKeypadLookDetector.h"
#include "HElapsedTimer.h"
#include <QKeyEvent>

HKeypadLookDetector::HKeypadLookDetector(HEventLog& log, QWidget* pdialog, bool bUseLeft, bool bUseCenter, bool bUseRight)
: HLookDetector(log)
, m_bUseLeft(bUseLeft)
, m_bUseCenter(bUseCenter)
, m_bUseRight(bUseRight)
{
	setDialog(pdialog);
};

HKeypadLookDetector::~HKeypadLookDetector()
{
	// Make sure to delete this BEFORE deleting the dialog!!!
	m_pdialog->releaseKeyboard();
	m_pdialog->removeEventFilter(this);
	//qDebug() << "HKeypadLookDetector::~HKeypadLookDetector(): Event filter removed.";
};

void HKeypadLookDetector::setDialog(QWidget *pdialog)
{
	m_pdialog = pdialog;
	if (m_pdialog)
	{
		m_pdialog->grabKeyboard();
		m_pdialog->installEventFilter(this);
		//qDebug() << "HKeypadLookDetector: Event filter installed.";
	}
	else
	{
		qDebug() << "HKeypadLookDetector: No event filter installed - expecting testing input.";
	}
}

bool HKeypadLookDetector::eventFilter(QObject *obj, QEvent *event)
{
	Q_UNUSED(obj);
	int t = HElapsedTimer::elapsed();
	QKeyEvent *keyEvent;
	bool bVal = false;
	
	switch (event->type()) 
	{
		case QEvent::KeyPress:
		{
			keyEvent = static_cast<QKeyEvent*>(event);
			//qDebug() << "KeyPress(" << keyEvent->text() << ") repeat? " << keyEvent->isAutoRepeat();
			if (keyEvent->isAutoRepeat())
			{
				bVal = true;
			}
			else 
			{
				switch (keyEvent->key()) 
				{
					case Qt::Key_Enter:
					case Qt::Key_Space:
					{
						if (isAGLookEnabled())
						{
							emit attention();
							bVal = true;
						}
						break;
					}
					case Qt::Key_4:
					{
						if (m_bUseLeft)
						{
							if (isLookEnabled())
							{
								addTrans(HLookTrans::NoneLeft, t);
							}
							else
							{
								pendingTrans(true, HLookTrans::NoneLeft);
							}
							bVal = true;
						}
						break;
					}
					case Qt::Key_5:
					{
						if (m_bUseCenter)
						{
							if (isLookEnabled())
							{
								addTrans(HLookTrans::NoneCenter, t);
							}
							else
							{
								pendingTrans(true, HLookTrans::NoneCenter);
							}
							bVal = true;
						}
						break;
					}
					case Qt::Key_6:
					{
						if (m_bUseRight)
						{
							if (isLookEnabled())
							{
								addTrans(HLookTrans::NoneRight, t);
							}
							else
							{
								pendingTrans(true, HLookTrans::NoneRight);
							}
							bVal = true;
						}
						break;
					}
					default:
						break;
				}
			}
			break;
		}
		case QEvent::KeyRelease:
		{
			keyEvent = static_cast<QKeyEvent*>(event);
			//qDebug() << "KeyRelease(" << keyEvent->text() << ") repeat? " << keyEvent->isAutoRepeat();

			// On Windows, it seems that when holding the key down we get repeated events in this
			// type of sequence (from log file on windows)
			// 
			//06 : 58 : 28 Debug : KeyPress("5") repeat ? false
			//06 : 58 : 28 Debug : KeyRelease("5") repeat ? true
			//06 : 58 : 28 Debug : KeyPress("5") repeat ? true
			//06 : 58 : 28 Debug : KeyRelease("5") repeat ? true
			// ===CUT===
			//06 : 58 : 30 Debug : KeyRelease("5") repeat ? false
			//
			// The last one - with autorepeat=FALSE, appears to be the one. 
			// On Mac, we get no "KeyRelease" events with AutoRepeat=true. 
			// Apparently on Windows, the held-key-press is translated as a series
			// of "KeyPress (repeat=true)"/"KeyRelease (repeat=true)" events. 
			if (keyEvent->isAutoRepeat())
			{
				bVal = true;
			}
			else
			{
				switch (keyEvent->key())
				{
				case Qt::Key_Enter:
				case Qt::Key_Space:
				{
					if (isAGLookEnabled())
					{
						// just eat the key release in this case. Action was taken on key press. 
						bVal = true;
					}
					break;
				}
				case Qt::Key_4:
				{
					if (m_bUseLeft)
					{
						if (isLookEnabled())
						{
							addTrans(HLookTrans::LeftNone, t);
						}
						else
						{
							pendingTrans(false);
						}
						bVal = true;
					}
					break;
				}
				case Qt::Key_5:
				{
					if (m_bUseCenter)
					{
						if (isLookEnabled())
						{
							addTrans(HLookTrans::CenterNone, t);
						}
						else
						{
							pendingTrans(false);
						}
						bVal = true;
					}
					break;
				}
				case Qt::Key_6:
				{
					if (m_bUseRight)
					{
						if (isLookEnabled())
						{
							addTrans(HLookTrans::RightNone, t);
						}
						else
						{
							pendingTrans(false);
						}
						bVal = true;
					}
					break;
				}
				default:
					break;
				}
			}
			break;
		}
		default:
			break;
	}
	return bVal;
};
