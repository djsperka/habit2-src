/*
 *  HStateMachine.cpp
 *  hstate
 *
 *  Created by Oakes Lab on 6/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HStateMachine.h"

HStateMachine::HStateMachine(const HExperiment& expt)
: QStateMachine()
, m_expt(expt)
{
}

HStateMachine::~HStateMachine()
{
};
