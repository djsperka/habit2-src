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
const HPhaseType HPhaseType::PreTest(1, "PreTest");
const HPhaseType HPhaseType::Habituation(2, "Habituation");
const HPhaseType HPhaseType::Test(3, "Test");

const HPhaseType& getPhaseType(int i)
{
	switch (i)
	{
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



const HStimContext HStimContext::UnknownContext(0, "UnknownContext");
const HStimContext HStimContext::PreTestPhase(1, "PreTestStim");
const HStimContext HStimContext::HabituationPhase(2, "HabituationStim");
const HStimContext HStimContext::TestPhase(3, "TestStim");
const HStimContext HStimContext::AttentionGetter(4, "AttentionGetter");

const HStimContext& getStimContext(int i)
{
	switch (i)
	{
		case 0: return HStimContext::UnknownContext; break;
		case 1: return HStimContext::PreTestPhase; break;
		case 2: return HStimContext::HabituationPhase; break;
		case 3: return HStimContext::TestPhase; break;
		case 4: return HStimContext::AttentionGetter; break;
		default: return HStimContext::UnknownContext; break;
	}
	return HStimContext::UnknownContext;
};

bool operator==(const HStimContext& lhs, const HStimContext& rhs)
{
	return lhs.number() == rhs.number();
}

bool operator==(const HHabituationType& lhs, const HHabituationType& rhs)
{
	return lhs.number() == rhs.number();
}

const HHabituationType HHabituationType::HHabituationTypeUnknown(-1, "HabituationTypeUnknown", "Unknown");
const HHabituationType HHabituationType::HHabituationTypeFixedN(0, "HabituationTypeFixedN", "Fixed Number");
const HHabituationType HHabituationType::HHabituationTypeCriterion(1, "HabituationTypeCriterion", "Criterion to End");
const HHabituationType HHabituationType::HHabituationTypeTotalLookingTime(2, "HabituationTypeTotalLookingTime", "Total Looking");

// This array is used in handling the QComboBox for habituation type in the HabituationSetupForm
// The combo box items are created by iterating through this array, and the 'number() value for 
// is used to persist the selection in the database. That means you MUST NOT CHANGE THE number() OF
// ITEMS IN THIS ARRAY! New items may be added, and old items removed, but new, unique values for 
// number() should be used. See HabituationSetupForm. 
// 
// Whenever changing elements of this array, make certain that the size of this array is set 
// here and in the declaration (see HTypes.h).

const HHabituationType *HHabituationType::A[3] = 
{ 
	&HHabituationType::HHabituationTypeFixedN, 
	&HHabituationType::HHabituationTypeCriterion,
	&HHabituationType::HHabituationTypeTotalLookingTime
};

const HHabituationType& getHabituationType(int number_value)
{
	const HHabituationType* result = &HHabituationType::HHabituationTypeUnknown;
	for (int i=0; i<sizeof(HHabituationType::A)/sizeof(HHabituationType*) && result==&HHabituationType::HHabituationTypeUnknown; i++)
		if (number_value == HHabituationType::A[i]->number()) result = HHabituationType::A[i];
	return *result;
}