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

HKeypadLookDetector::HKeypadLookDetector(HEventLog& log, QWidget* pdialog, int minlooktime_ms, int minlookawaytime_ms, int maxlookawaytime_ms, int maxaccumlooktime_ms, bool bUseLeft, bool bUseCenter, bool bUseRight)
: HLookDetector(minlooktime_ms, minlookawaytime_ms, maxlookawaytime_ms, maxaccumlooktime_ms, log)
, m_pdialog(pdialog)
, m_bUseLeft(bUseLeft)
, m_bUseCenter(bUseCenter)
, m_bUseRight(bUseRight)
{
	if (m_pdialog)
	{
		m_pdialog->grabKeyboard();
		m_pdialog->installEventFilter(this);
		qDebug() << "HKeypadLookDetector: Event filter installed.";
	}
	else
	{
		qDebug() << "HKeypadLookDetector: No event filter installed - expecting testing input.";
	}
};																																							  

HKeypadLookDetector::~HKeypadLookDetector() 
{ 
	m_pdialog->releaseKeyboard();
	m_pdialog->removeEventFilter(this); 
	qDebug() << "HKeypadLookDetector: Event filter removed.";
};



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
								qDebug() << "Key_5 down: addTrans";
								addTrans(HLookTrans::NoneCenter, t);
							}
							else
							{
								qDebug() << "Key_5 down: pendingTrans";
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
			break;
		}
		default:
			break;
	}
	return bVal;
};
