/*
 *  HKeypadLookDetector.h
 *  habutil
 *
 *  Created by Oakes Lab on 8/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HKEYPADLOOKDETECTOR_H_
#define _HKEYPADLOOKDETECTOR_H_

#include "HLookDetector.h"
#include <QEvent>

class HKeypadLookDetector: public HLookDetector
{
	Q_OBJECT
	
public:
	HKeypadLookDetector(int minlooktime_ms, int minlookawaytime_ms, QObject* pdialog, bool bUseLeft, bool bUseCenter, bool bUseRight) : HLookDetector(minlooktime_ms, minlookawaytime_ms), m_pdialog(pdialog), m_bUseLeft(bUseLeft), m_bUseCenter(bUseCenter), m_bUseRight(bUseRight)
	{
		m_pdialog->installEventFilter(this);
	};																																							  
	virtual ~HKeypadLookDetector() { m_pdialog->removeEventFilter(this); };
	bool eventFilter(QObject *obj, QEvent *event);
protected:
	virtual void agLookEnabled(bool enabled) { Q_UNUSED(enabled); };
	virtual void lookEnabled(bool enabled) { Q_UNUSED(enabled); };
private:
	QObject* m_pdialog;
	bool m_bUseLeft;
	bool m_bUseCenter;
	bool m_bUseRight;
};

#endif