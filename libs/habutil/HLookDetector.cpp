/*
 *  HLookDetector.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 7/26/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HLookDetector.h"
#include "HElapsedTimer.h"
#include <QCoreApplication>

void HLookDetector::enableAGLook()
{
	if (m_bLookEnabled)
	{
		m_bLookEnabled = false;
		lookEnabled(false);
	}
	if (!m_bAGLookEnabled)
	{
		m_bAGLookEnabled = true;
		agLookEnabled(true);
	}
	return;
};

void HLookDetector::enableLook()
{
	if (m_bAGLookEnabled)
	{
		m_bAGLookEnabled = false;
		agLookEnabled(false);
	}
	if (!m_bLookEnabled)
	{
		m_bLookEnabled = true;
		lookEnabled(true);
	}
	start();	// start state machine
	QCoreApplication::processEvents(0);
	qDebug() << "HLookDetector::enableLook() { start() } " << this->isRunning();
	return;
};

void HLookDetector::disable()
{
	if (m_bLookEnabled)
	{
		m_bLookEnabled = false;
		lookEnabled(false);
		stopLooker(HElapsedTimer::elapsed());		// stop state machine
	}
	if (m_bAGLookEnabled)
	{
		m_bAGLookEnabled = false;
		agLookEnabled(false);
	}
	return;
};
