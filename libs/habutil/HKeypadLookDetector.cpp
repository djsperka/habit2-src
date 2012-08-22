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
							emit attention();
							bVal = true;
						}
						break;
					}
					case Qt::Key_4:
					{
						if (isLookEnabled() && m_bUseLeft)
						{
							addTrans(NoneLeft, t);
							bVal = true;
						}
						break;
					}
					case Qt::Key_5:
					{
						if (isLookEnabled() && m_bUseCenter)
						{
							addTrans(NoneCenter, t);
							bVal = true;
						}
						break;
					}
					case Qt::Key_6:
					{
						if (isLookEnabled() && m_bUseRight)
						{
							addTrans(NoneRight, t);
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
						addTrans(LeftNone, t);
						bVal = true;
					}
					break;
				}
				case Qt::Key_5:
				{
					if (isLookEnabled() && m_bUseCenter)
					{
						addTrans(CenterNone, t);
						bVal = true;
					}
					break;
				}
				case Qt::Key_6:
				{
					if (isLookEnabled() && m_bUseRight)
					{
						addTrans(RightNone, t);
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
