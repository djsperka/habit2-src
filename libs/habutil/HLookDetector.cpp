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

void HLookDetector::pendingTrans(bool bPending, const HLookTrans& type)
{
	m_bLookTransPending = bPending;
	if (m_bLookTransPending)
	{
		m_iLookTransPendingType = type.number();
		qDebug() << "pendingTrans: " << type.name();
	}
	else
	{
		m_iLookTransPendingType = HLookTrans::UnknownLookTrans.number();
		qDebug() << "pendingTrans: FALSE";
	}
}


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
	log().append(new HAGLookEnabledEvent(HElapsedTimer::elapsed()));
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
	qDebug() << "call start()";
	start();	// start state machine
	qDebug() << "state machine started, calling process Events(0)";
	QCoreApplication::processEvents(0);
	qDebug() << "append log";
	log().append(new HLookEnabledEvent(HElapsedTimer::elapsed()));
	if (m_bLookTransPending)
	{
		qDebug() << "Add pending look trans: " << getLookTrans(m_iLookTransPendingType).name();
		addTrans(getLookTrans(m_iLookTransPendingType), HElapsedTimer::elapsed());
		pendingTrans(false);
	}
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
	log().append(new HLookDisabledEvent(HElapsedTimer::elapsed()));
	return;
};
