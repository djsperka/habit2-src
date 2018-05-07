/*
 *  HStateMachine.h
 *  hstate
 *
 *  Created by Oakes Lab on 6/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HSTATE_MACHINE_H
#define _HSTATE_MACHINE_H

#include <QStateMachine>
#include <QtDebug>
#include "HExperiment.h"

class HStateMachine: public QStateMachine
{
	Q_OBJECT

	const HExperiment& m_expt;
	public:

	HStateMachine(const HExperiment &expt);
	~HStateMachine();
	const HExperiment& experiment() const { return m_expt; };
};

#endif
