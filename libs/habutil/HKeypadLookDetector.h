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
#include <QWidget>

class HKeypadLookDetector: public HLookDetector
{
	Q_OBJECT
	
public:
	HKeypadLookDetector(HEventLog& log, QWidget* pdialog=NULL, bool bUseLeft=true, bool bUseCenter=true, bool bUseRight=true);
	virtual ~HKeypadLookDetector();
	bool eventFilter(QObject *obj, QEvent *event);
	void setDialog(QWidget *pdialog);
protected:
	virtual void agLookEnabled(bool enabled) { Q_UNUSED(enabled); };
	virtual void lookEnabled(bool enabled) { Q_UNUSED(enabled); };
private:
	QWidget* m_pdialog;
	bool m_bUseLeft;
	bool m_bUseCenter;
	bool m_bUseRight;
};

#endif
