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


HKeypadLookDetector::HKeypadLookDetector(int minlooktime_ms, int minlookawaytime_ms, int maxlookawaytime_ms, int maxaccumlooktime_ms, HEventLog& log, QWidget* pdialog, bool bUseLeft, bool bUseCenter, bool bUseRight)
: HLookDetector(minlooktime_ms, minlookawaytime_ms, maxlookawaytime_ms, maxaccumlooktime_ms, log)
, m_pdialog(pdialog)
, m_bUseLeft(bUseLeft)
, m_bUseCenter(bUseCenter)
, m_bUseRight(bUseRight)
{
	qDebug() << "HKeypadLookDetector: grab keyboard and install self.";
	m_pdialog->grabKeyboard();
	m_pdialog->installEventFilter(this);
};																																							  

HKeypadLookDetector::~HKeypadLookDetector() 
{ 
	qDebug() << "HKeypadLookDetector: release keyboard and remove self.";
	m_pdialog->releaseKeyboard();
	m_pdialog->removeEventFilter(this); 
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
					{
						if (isAGLookEnabled())
						{
							qDebug() << "Emit attention() signal...";
							emit attention();
							bVal = true;
						}
						break;
					}
					case Qt::Key_4:
					{
						if (isLookEnabled() && m_bUseLeft)
						{
							addTrans(HLookTrans::NoneLeft, t);
							bVal = true;
						}
						break;
					}
					case Qt::Key_5:
					{
						if (isLookEnabled() && m_bUseCenter)
						{
							addTrans(HLookTrans::NoneCenter, t);
							bVal = true;
						}
						break;
					}
					case Qt::Key_6:
					{
						if (isLookEnabled() && m_bUseRight)
						{
							addTrans(HLookTrans::NoneRight, t);
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
					if (isLookEnabled() && m_bUseLeft)
					{
						addTrans(HLookTrans::LeftNone, t);
						bVal = true;
					}
					break;
				}
				case Qt::Key_5:
				{
					if (isLookEnabled() && m_bUseCenter)
					{
						addTrans(HLookTrans::CenterNone, t);
						bVal = true;
					}
					break;
				}
				case Qt::Key_6:
				{
					if (isLookEnabled() && m_bUseRight)
					{
						addTrans(HLookTrans::RightNone, t);
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
