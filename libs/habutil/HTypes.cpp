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

bool operator==(const HPhaseType& lhs, const HPhaseType& rhs)
{
	return lhs.number() == rhs.number();
};

const HLookTrans HLookTrans::UnknownLookTrans(-1, "UnknownLookTrans", false);
const HLookTrans HLookTrans::NoneLeft(0, "NoneLeft", true);
const HLookTrans HLookTrans::LeftNone(1, "LeftNone", false);
const HLookTrans HLookTrans::NoneCenter(2, "NoneCenter", true);
const HLookTrans HLookTrans::CenterNone(3, "CenterNone", false);
const HLookTrans HLookTrans::NoneRight(4, "NoneRight", true);
const HLookTrans HLookTrans::RightNone(5, "RightNone", false);
const HLookTrans HLookTrans::NoneNone(6, "NoneNone", false);

const HLookTrans& getLookTrans(int i)
{
	switch(i)
	{
		case -1: return HLookTrans::UnknownLookTrans; break;
		case 0: return HLookTrans::NoneLeft; break;
		case 1: return HLookTrans::LeftNone; break;
		case 2: return HLookTrans::NoneCenter; break;
		case 3: return HLookTrans::CenterNone; break;
		case 4: return HLookTrans::NoneRight; break;
		case 5: return HLookTrans::RightNone; break;
		case 6: return HLookTrans::NoneNone; break;
		default: return HLookTrans::UnknownLookTrans; break;
	}
	return HLookTrans::UnknownLookTrans;
};

const HLookDirection HLookDirection::UnknownLookDirection(-1, "UnknownLookDirection");
const HLookDirection HLookDirection::LookAway(0, "LookAway");
const HLookDirection HLookDirection::LookLeft(1, "LookLeft");
const HLookDirection HLookDirection::LookRight(2, "LookRight");
const HLookDirection HLookDirection::LookCenter(3, "LookCenter");

const HLookDirection& getLookDirection(int i)
{
	switch (i)
	{
		case -1: return HLookDirection::UnknownLookDirection; break;
		case 0: return HLookDirection::LookAway; break;
		case 1: return HLookDirection::LookLeft; break;
		case 2: return HLookDirection::LookRight; break;
		case 3: return HLookDirection::LookCenter; break;
		default: return HLookDirection::UnknownLookDirection; break;
	}
	return HLookDirection::UnknownLookDirection;
};

bool operator==(const HLookDirection& lhs, const HLookDirection& rhs)
{
	return (lhs.number() == rhs.number());
}

bool operator!=(const HLookDirection& lhs, const HLookDirection& rhs)
{
	return !(lhs == rhs);
}

bool operator==(const HLookTrans& lhs, const HLookTrans& rhs)
{
	return (lhs.number() == rhs.number());
}

const HLookDirection& directionTo(const HLookTrans& type)
{
	if (type == HLookTrans::NoneLeft) return HLookDirection::LookLeft;
	else if (type == HLookTrans::NoneCenter) return HLookDirection::LookCenter;
	else if (type == HLookTrans::NoneRight) return HLookDirection::LookRight;
	else if (type == HLookTrans::LeftNone || type == HLookTrans::CenterNone || type == HLookTrans::RightNone) return HLookDirection::LookAway;
	return HLookDirection::UnknownLookDirection;
}




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
};

bool operator==(const HHabituationType& lhs, const HHabituationType& rhs)
{
	return lhs.number() == rhs.number();
};

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
	for (unsigned int i=0; i<sizeof(HHabituationType::A)/sizeof(HHabituationType*) && result==&HHabituationType::HHabituationTypeUnknown; i++)
		if (number_value == HHabituationType::A[i]->number()) result = HHabituationType::A[i];
	return *result;
};



const HCriterionWindowType HCriterionWindowType::HCriterionWindowUnknown(-1, "CriterionWindowUnknown", "Unknown window type");
const HCriterionWindowType HCriterionWindowType::HCriterionWindowFixed(0, "CriterionWindowFixed", "Fixed window");
const HCriterionWindowType HCriterionWindowType::HCriterionWindowSliding(1, "CriterionWindowSliding", "Sliding window");

// The contents of this array are used to populate the radio buttons. 
const HCriterionWindowType* HCriterionWindowType::A[2] = 
{
	&HCriterionWindowType::HCriterionWindowFixed, 
	&HCriterionWindowType::HCriterionWindowSliding
};

bool operator==(const HCriterionWindowType& lhs, const HCriterionWindowType& rhs)
{
	return lhs.number() == rhs.number();
};


const HCriterionWindowType& getCriterionWindowType(int number_value)
{
	using ::HCriterionWindowType;
	const HCriterionWindowType* result = &HCriterionWindowType::HCriterionWindowUnknown;
	for (unsigned int i=0; i<sizeof(HCriterionWindowType::A)/sizeof(HCriterionWindowType*) && result==&HCriterionWindowType::HCriterionWindowUnknown; i++)
		if (number_value == HCriterionWindowType::A[i]->number()) result = HCriterionWindowType::A[i];
	return *result;
}





const HCriterionBasisType HCriterionBasisType::HCriterionBasisUnknown(-1, "CriterionBasisUnknown", "Unknown");
const HCriterionBasisType HCriterionBasisType::HCriterionBasisFirstN(0, "CriterionBasisFirstN", "Base criterion on first n trials");
const HCriterionBasisType HCriterionBasisType::HCriterionBasisLongestN(1, "CriterionBasisLongestN", "Base criterion on longest n trials");

// The contents of this array are used to populate the radio buttons. 
const HCriterionBasisType* HCriterionBasisType::A[2] = 
{
	&HCriterionBasisType::HCriterionBasisFirstN, 
	&HCriterionBasisType::HCriterionBasisLongestN
};

bool operator==(const HCriterionBasisType& lhs, const HCriterionBasisType& rhs)
{
	return (lhs.number() == rhs.number());
};

const HCriterionBasisType& getCriterionBasisType(int number_value)
{
	const HCriterionBasisType* result = &HCriterionBasisType::HCriterionBasisUnknown;
	for (unsigned int i=0; i<sizeof(HCriterionBasisType::A)/sizeof(HCriterionBasisType*) && result==&HCriterionBasisType::HCriterionBasisUnknown; i++)
		if (number_value == HCriterionBasisType::A[i]->number()) result = HCriterionBasisType::A[i];
	return *result;
}


const HTrialCompletionType HTrialCompletionType::HTrialCompletionUnknown(-1, "TrialCompletionUnknown", "Unknown");
const HTrialCompletionType HTrialCompletionType::HTrialCompletionFixedLength(0, "TrialCompletionFixedLength", "Fixed Length");
const HTrialCompletionType HTrialCompletionType::HTrialCompletionSubjectControlled(1, "TrialCompletionSubjectControlled", "Subject Controlled");

// The contents of this array are used to populate the radio buttons.
const HTrialCompletionType* HTrialCompletionType::A[2] =
{
	&HTrialCompletionType::HTrialCompletionFixedLength,
	&HTrialCompletionType::HTrialCompletionSubjectControlled
};

bool operator==(const HTrialCompletionType& lhs, const HTrialCompletionType& rhs)
{
	return (lhs.number() == rhs.number());
};

const HTrialCompletionType& getTrialCompletionType(int number_value)
{
	const HTrialCompletionType* result = &HTrialCompletionType::HTrialCompletionUnknown;
	for (unsigned int i=0; i<sizeof(HTrialCompletionType::A)/sizeof(HTrialCompletionType*) && result==&HTrialCompletionType::HTrialCompletionUnknown; i++)
		if (number_value == HTrialCompletionType::A[i]->number()) result = HTrialCompletionType::A[i];
	return *result;
};



const HPresentationStyle HPresentationStyle::HPresentationStyleUnknown(-1, "PresentationStyleUnknown", "Unknown");
const HPresentationStyle HPresentationStyle::HPresentationStyleMonitorDefined(0, "PresentationStyleMonitorDefined", "Monitor Defined");
const HPresentationStyle HPresentationStyle::HPresentationStyleSingleMovable(1, "PresentationStyleSingleMovable", "Single Movable");

// The contents of this array are used to populate the radio buttons.
const HPresentationStyle* HPresentationStyle::A[2] =
{
	&HPresentationStyle::HPresentationStyleMonitorDefined,
	&HPresentationStyle::HPresentationStyleSingleMovable
};

bool operator==(const HPresentationStyle& lhs, const HPresentationStyle& rhs)
{
	return (lhs.number() == rhs.number());
};

const HPresentationStyle& getPresentationStyle(int number_value)
{
	const HPresentationStyle* result = &HPresentationStyle::HPresentationStyleUnknown;
	for (unsigned int i=0; i<sizeof(HPresentationStyle::A)/sizeof(HPresentationStyle*) && result==&HPresentationStyle::HPresentationStyleUnknown; i++)
		if (number_value == HPresentationStyle::A[i]->number()) result = HPresentationStyle::A[i];
	return *result;
};


const HDisplayType HDisplayType::HDisplayTypeUnknown(-1, "DisplayTypeUnknown", "Unknown");
const HDisplayType HDisplayType::HDisplayTypeFullScreen(0, "DisplayTypeFullScreen", "Full Screen");
const HDisplayType HDisplayType::HDisplayTypeOriginalSize(1, "DisplayTypeOriginalSize", "Original Size");

// The contents of this array are used to populate the radio buttons.
const HDisplayType* HDisplayType::A[2] =
{
	&HDisplayType::HDisplayTypeFullScreen,
	&HDisplayType::HDisplayTypeOriginalSize
};

bool operator==(const HDisplayType& lhs, const HDisplayType& rhs)
{
	return (lhs.number() == rhs.number());
};

const HDisplayType& getDisplayType(int number_value)
{
	const HDisplayType* result = &HDisplayType::HDisplayTypeUnknown;
	for (unsigned int i=0; i<sizeof(HDisplayType::A)/sizeof(HDisplayType*) && result==&HDisplayType::HDisplayTypeUnknown; i++)
		if (number_value == HDisplayType::A[i]->number()) result = HDisplayType::A[i];
	return *result;
};



const HResultsType HResultsType::HResultsTypeUnknown(-1, "Unknown Results Type");
const HResultsType HResultsType::HResultsTypeOriginalRun(0, "Original Run");
const HResultsType HResultsType::HResultsTypeTestRun(1, "Test Run");
const HResultsType HResultsType::HResultsTypeReliabilityRun(2, "Reliability Run");

const HResultsType* HResultsType::A[3] =
{
	&HResultsType::HResultsTypeOriginalRun,
	&HResultsType::HResultsTypeTestRun,
	&HResultsType::HResultsTypeReliabilityRun
};

bool operator==(const HResultsType& lhs, const HResultsType& rhs)
{
	return (lhs.number() == rhs.number());
}

const HResultsType& getResultsType(int number_value)
{
	const HResultsType* result = &HResultsType::HResultsTypeUnknown;
	for (unsigned int i=0; i<sizeof(HResultsType::A)/sizeof(HResultsType *) && result==&HResultsType::HResultsTypeUnknown; i++)
		if (number_value == HResultsType::A[i]->number()) result = HResultsType::A[i];
	return *result;
};


const HLookDetectorType HLookDetectorType::UnknownLookDetector(-1, "UnknownLookDetector", "Unknown Type");
const HLookDetectorType HLookDetectorType::Keypad(0, "Keypad", "Keypad (456Enter)");
const HLookDetectorType HLookDetectorType::EyeTracker(1, "EyeTracker", "Eye Tracker");
const HLookDetectorType* HLookDetectorType::A[2] =
{
	&HLookDetectorType::Keypad,
	&HLookDetectorType::EyeTracker
};

bool operator==(const HLookDetectorType& lhs, const HLookDetectorType& rhs)
{
	return (lhs.number() == rhs.number());
};

const HLookDetectorType& getLookDetectorType(int number_value)
{
	const HLookDetectorType* result = &HLookDetectorType::UnknownLookDetector;
	for (unsigned int i=0; i<sizeof(HLookDetectorType::A)/sizeof(HLookDetectorType *) && result==&HLookDetectorType::UnknownLookDetector; i++)
		if (number_value == HLookDetectorType::A[i]->number()) result = HLookDetectorType::A[i];
	return *result;
};

