/*
 *  hLookEventFilter.h
 *  habutil
 *
 *  Created by Oakes Lab on 7/25/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _HLOOKEVENTFILTER_H_
#define _HLOOKEVENTFILTER_H_

#include <QKeyEvent>
#include "HLooker.h"

class HLookEventFilter: public QObject
{
	Q_OBJECT

public:
	HLookEventFilter(HLooker* plooker, const QTime& clock, bool useLeft, bool useCenter, bool useRight) : m_plooker(plooker), m_time(clock), m_bUseLeft(useLeft), m_bUseCenter(useCenter), m_bUseRight(useRight) {};
	~HLookEventFilter() {};
private:
	HLooker* m_plooker;
	const QTime& m_time;
	bool m_bUseLeft;
	bool m_bUseCenter;
	bool m_bUseRight;
protected:
	bool eventFilter(QObject *obj, QEvent *event)
	{
		Q_UNUSED(obj);
		int t = m_time.elapsed();
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
						case Qt::Key_4:
						{
							if (m_bUseLeft)
							{
								m_plooker->addTrans(NoneLeft, t);
								bVal = true;
							}
							break;
						}
						case Qt::Key_5:
						{
							if (m_bUseCenter)
							{
								m_plooker->addTrans(NoneCenter, t);
								bVal = true;
							}
							break;
						}
						case Qt::Key_6:
						{
							if (m_bUseRight)
							{
								m_plooker->addTrans(NoneRight, t);
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
					case Qt::Key_4:
					{
						if (m_bUseLeft)
						{
							m_plooker->addTrans(LeftNone, t);
							bVal = true;
						}
						break;
					}
					case Qt::Key_5:
					{
						if (m_bUseCenter)
						{
							m_plooker->addTrans(CenterNone, t);
							bVal = true;
						}
						break;
					}
					case Qt::Key_6:
					{
						if (m_bUseRight)
						{
							m_plooker->addTrans(RightNone, t);
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
	
};

#endif