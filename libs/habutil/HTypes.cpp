/*
 *  HTypes.cpp
 *  habutil
 *
 *  Created by Oakes Lab on 2/26/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "HTypes.h"

const HPhaseType HPhaseType::UnknownPhase(-1, "UnknownPhase");
const HPhaseType HPhaseType::AttentionGetter(0, "AttentionGetter");
const HPhaseType HPhaseType::PreTest(1, "PreTest");
const HPhaseType HPhaseType::Habituation(2, "Habituation");
const HPhaseType HPhaseType::Test(3, "Test");

const HPhaseType& getPhaseType(int i)
{
	switch (i)
	{
		case 0: return HPhaseType::AttentionGetter; break;
		case 1: return HPhaseType::PreTest; break;
		case 2: return HPhaseType::Habituation; break;
		case 3: return HPhaseType::Test; break;
		default: return HPhaseType::UnknownPhase; break;
	}
	return HPhaseType::UnknownPhase; 
};

const HLookTrans HLookTrans::UnknownLookTrans(0, "UnknownLookTrans");
const HLookTrans HLookTrans::NoneLeft(1, "NoneLeft");
const HLookTrans HLookTrans::LeftNone(2, "LeftNone");
const HLookTrans HLookTrans::NoneCenter(3, "NoneCenter");
const HLookTrans HLookTrans::CenterNone(4, "CenterNone");
const HLookTrans HLookTrans::NoneRight(5, "NoneRight");
const HLookTrans HLookTrans::RightNone(6, "RightNone");
const HLookTrans HLookTrans::NoneNone(7, "NoneNone");

const HLookTrans& getLookTransType(int i)
{
	switch(i)
	{
		case 0: return HLookTrans::UnknownLookTrans; break;
		case 1: return HLookTrans::NoneLeft; break;
		case 2: return HLookTrans::LeftNone; break;
		case 3: return HLookTrans::NoneCenter; break;
		case 4: return HLookTrans::CenterNone; break;
		case 5: return HLookTrans::NoneRight; break;
		case 6: return HLookTrans::RightNone; break;
		case 7: return HLookTrans::NoneNone; break;
		default: return HLookTrans::UnknownLookTrans; break;
	}
	return HLookTrans::UnknownLookTrans;
};

const HLookDirection HLookDirection::UnknownLookDirection(0, "UnknownLookDirection");
const HLookDirection HLookDirection::NoLook(1, "NoLook");
const HLookDirection HLookDirection::LookLeft(2, "LookLeft");
const HLookDirection HLookDirection::LookRight(3, "LookRight");
const HLookDirection HLookDirection::LookCenter(4, "LookCenter");

const HLookDirection& getLookDirection(int i)
{
	switch (i)
	{
		case 0: return HLookDirection::UnknownLookDirection; break;
		case 1: return HLookDirection::NoLook; break;
		case 2: return HLookDirection::LookLeft; break;
		case 3: return HLookDirection::LookRight; break;
		case 4: return HLookDirection::LookCenter; break;
		default: return HLookDirection::UnknownLookDirection; break;
	}
	return HLookDirection::UnknownLookDirection;
};


