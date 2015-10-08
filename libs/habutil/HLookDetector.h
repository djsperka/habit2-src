/*
 *  HLookDetector.h
 *  habutil
 *
 *  Created by Oakes Lab on 7/26/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HLOOKDETECTOR_H_
#define _HLOOKDETECTOR_H_

#include "HLooker.h"


// Look detector is an abstract interface class for managing the entity that 
// detects when subject is looking at something. Subclasses should implement 
// specific methods for determining whether looks occur, either using keypad
// input or (FUTURE) an eye tracking device. 
//
// The LookDetector is the only interface through which habit gets info on
// looking behavior. The LookDetector has two types of 'looks' that it 
// detects: attention looks and 'real looks'. Attention looks occur when the 
// subject actually begins looking at a target (as determined by the experimenter).
// An attention look is just a time, not a duration. 'Real' looks occur according
// to the rules for lookaways, etc., controlled here by parameters in the
// constructor. Real looks have a start and end time - a duration.
//
// Each type of look detection can be turned on and off. It is the responsibility 
// of subclasses to implement the method of input to the Looker, e.g. keypad or 
// eye tracker etc. For example, the KeypadLookDetector installs an event 
// filter and looks at input from the keypad. The constructor is supplied with 
// a QObject that will receive the keypad events (presumably a dialog that must
// be in focus). It interprets key events and when a transition is found 
// it is added to the Looker. 
// 
// The subclass must also implement the attention() signal - see below. 
//
// The HLooker base class emits the look() signal. 
//
// SIGNAL(attention()) - signals when the attention getter has done its job
// and got the attention of the subject. This SIGNAL is picked up by the 
// AGRunning state to indicate it can transition to the stim. SUBCLASSES MUST
// IMPLEMENT THIS SIGNAL. 
//
// SIGNAL(look()) - signals that a look (as defined by the exp settings)
// has occurred. The full look() signal includes an HLook argument that 
// gives complete information on the look (duration, what was looked at).
// The HLooker base class emits this signal, but subclasses must feed it 
// transitions (see HLooker::addTrans()). 
//
// Subclasses should implement agLookEnabled and lookEnabled. These are called
// when look detection is activated - any initialization or prep (e.g. install
// event filter) should occur then. 

class HLookDetector: public HLooker
{
	Q_OBJECT
	
private:
	bool m_bAGLookEnabled;
	bool m_bLookEnabled;
	bool m_bLookTransPending;
	int m_iLookTransPendingType;
protected:
	virtual void agLookEnabled(bool enabled) = 0;
	virtual void lookEnabled(bool enabled) = 0;
signals:
	void attention();
	void lookDetectorEnabled();
	void lookDetectorDisabled();
public:
	HLookDetector(int minlooktime_ms, int minlookawaytime_ms, int maxlookawaytime_ms, int maxaccumlooktime_ms, HEventLog& log)
		: HLooker(minlooktime_ms, minlookawaytime_ms, maxlookawaytime_ms, maxaccumlooktime_ms, log)
		, m_bAGLookEnabled(false)
		, m_bLookEnabled(false)
		, m_bLookTransPending(false)
		, m_iLookTransPendingType(-1) {};
	virtual ~HLookDetector() {};
	virtual void enableAGLook();
	virtual void enableLook();
	virtual void disable();
	bool isAGLookEnabled() { return m_bAGLookEnabled; };
	bool isLookEnabled() { return 	m_bLookEnabled; };
	void pendingTrans(bool bPending, const HLookTrans& type = HLookTrans::UnknownLookTrans);
};
	
#endif
