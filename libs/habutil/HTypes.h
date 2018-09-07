/*
 *  HTypes.h
 *  habutil
 *
 *  Created by Oakes Lab on 2/26/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HTYPES_H_
#define _HTYPES_H_

#include <QString>

class HPhaseType
{
public:
	static const HPhaseType UnknownPhase;
	static const HPhaseType PreTest;
	static const HPhaseType Habituation;
	static const HPhaseType Test;
	int number() const { return m_t; }
	QString name() const { return m_s; }
	friend bool operator==(const HPhaseType& lhs, const HPhaseType& rhs);
private:
	explicit HPhaseType(int t, const char *name): m_t(t), m_s(name) {}
	int m_t;
	QString m_s;
};


class HLookDirection
{
public:
	static const HLookDirection LookAway;
	static const HLookDirection LookLeft;
	static const HLookDirection LookRight;
	static const HLookDirection LookCenter;
	static const HLookDirection UnknownLookDirection;
	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	bool isToStim() const { return m_bStim; }

private:
	explicit HLookDirection(int t, bool bStim, const char *name): m_t(t), m_bStim(bStim), m_s(name) {}
	int m_t;
	bool m_bStim;
	QString m_s;
};


class HLookTrans
{
public:
	static const HLookTrans NoneLeft;
	static const HLookTrans LeftNone;
	static const HLookTrans NoneCenter;
	static const HLookTrans CenterNone;
	static const HLookTrans NoneRight;
	static const HLookTrans RightNone;
	static const HLookTrans NoneNone;
	static const HLookTrans UnknownLookTrans;
	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	bool isTransToLook() const { return m_bToLook; };
private:
	explicit HLookTrans(int t, const char *name, bool toLook): m_t(t), m_s(name), m_bToLook(toLook) {}
	int m_t;
	QString m_s;
	bool m_bToLook;
};




const HLookDirection& directionTo(const HLookTrans& type);
const HLookDirection& directionFrom(const HLookTrans& type);
const HPhaseType& getPhaseType(int i);
const HPhaseType& getPhaseType(const QString& sPhase);
const HLookTrans& getLookTrans(int i);
const HLookDirection& getLookDirection(int i);
bool operator==(const HLookDirection& lhs, const HLookDirection& rhs);
bool operator!=(const HLookDirection& lhs, const HLookDirection& rhs);
bool operator==(const HLookTrans& lhs, const HLookTrans& rhs);
bool operator<(const HPhaseType& lhs, const HPhaseType& rhs);

// Stim context refers to when stim is to be played. 

class HStimContext
{
public:
	static const HStimContext UnknownContext;
	static const HStimContext PreTestPhase;
	static const HStimContext HabituationPhase;
	static const HStimContext TestPhase;
	static const HStimContext AttentionGetter;	// Not really a phase, but makes this class useful for stim types as well as phase. 
	static const HStimContext Preview;			// Used with previewer

	HStimContext() : m_t(-1), m_s("") {};
	HStimContext(const HStimContext& c): m_t(c.number()), m_s(c.name()) {};
	HStimContext& operator=(const HStimContext& rhs);

	int number() const { return m_t; }
	const QString& name() const { return m_s; }

	friend bool operator==(const HStimContext& lhs, const HStimContext& rhs);
	
private:
	explicit HStimContext(int t, const char *name): m_t(t), m_s(name) {}
	int m_t;
	QString m_s;
};

bool operator<(const HStimContext& lhs, const HStimContext& rhs);

const HStimContext& getStimContext(int i);

class HHabituationType
{
public:
	static const HHabituationType HHabituationTypeUnknown;
	static const HHabituationType HHabituationTypeFixedN;
	static const HHabituationType HHabituationTypeCriterion;
	static const HHabituationType HHabituationTypeTotalLookingTime;
	
	// This array is used in handling the ComboBox in HabituationSetupForm
	// See HTypes.cpp.
	static const HHabituationType *A[3];

	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	const QString& label() const { return m_l; }
	friend bool operator==(const HHabituationType& lhs, const HHabituationType& rhs);
	
private:
	explicit HHabituationType(int t, const char *name, const char *label): m_t(t), m_s(name), m_l(label) {}
	int m_t;
	QString m_s;
	QString m_l;
};

const HHabituationType& getHabituationType(int number_value);


class HCriterionWindowType
{
public:
	static const HCriterionWindowType HCriterionWindowUnknown;
	static const HCriterionWindowType HCriterionWindowFixed;
	static const HCriterionWindowType HCriterionWindowSliding;
	
	// The contents of this array are used to populate the radio buttons. 
	static const HCriterionWindowType* A[2];
	
	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	const QString& label() const { return m_l; }
	friend bool operator==(const HCriterionWindowType& lhs, const HCriterionWindowType& rhs);
	
private:
	explicit HCriterionWindowType(int t, const char* name, const char* label): m_t(t), m_s(name), m_l(label) {};
	int m_t;
	const QString m_s;
	const QString m_l;
};

const HCriterionWindowType& getCriterionWindowType(int number_value);


class HCriterionBasisType
{
public:
	static const HCriterionBasisType HCriterionBasisUnknown;
	static const HCriterionBasisType HCriterionBasisFirstN;
	static const HCriterionBasisType HCriterionBasisLongestN;
	
	// The contents of this array are used to populate the radio buttons. 
	static const HCriterionBasisType* A[2];
	
	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	const QString& label() const { return m_l; }
	friend bool operator==(const HCriterionBasisType& lhs, const HCriterionBasisType& rhs);
	
private:
	explicit HCriterionBasisType(int t, const char* name, const char* label): m_t(t), m_s(name), m_l(label) {};
	int m_t;
	const QString m_s;
	const QString m_l;
};

const HCriterionBasisType& getCriterionBasisType(int number_value);


class HTrialCompletionType
{
public:
	static const HTrialCompletionType HTrialCompletionUnknown;
	static const HTrialCompletionType HTrialCompletionFixedLength;
	static const HTrialCompletionType HTrialCompletionSubjectControlled;

	// The contents of this array are used to populate the radio buttons.
	static const HTrialCompletionType* A[2];

	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	const QString& label() const { return m_l; }

private:
	explicit HTrialCompletionType(int t, const char* name, const char* label): m_t(t), m_s(name), m_l(label) {};
	int m_t;
	const QString m_s;
	const QString m_l;
};

bool operator==(const HTrialCompletionType& lhs, const HTrialCompletionType& rhs);

const HTrialCompletionType& getTrialCompletionType(int number_value);



//======



class HPresentationStyle
{
public:
	static const HPresentationStyle HPresentationStyleUnknown;
	static const HPresentationStyle HPresentationStyleMonitorDefined;
	static const HPresentationStyle HPresentationStyleSingleMovable;

	// The contents of this array are used to populate the radio buttons.
	static const HPresentationStyle* A[2];

	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	const QString& label() const { return m_l; }

private:
	explicit HPresentationStyle(int t, const char* name, const char* label): m_t(t), m_s(name), m_l(label) {};
	int m_t;
	const QString m_s;
	const QString m_l;
};

bool operator==(const HPresentationStyle& lhs, const HPresentationStyle& rhs);

const HPresentationStyle& getPresentationStyle(int number_value);


class HDisplayType
{
public:
	static const HDisplayType HDisplayTypeUnknown;
	static const HDisplayType HDisplayTypeFullScreen;
	static const HDisplayType HDisplayTypeOriginalSize;

	// The contents of this array are used to populate the radio buttons.
	static const HDisplayType* A[2];

	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	const QString& label() const { return m_l; }

private:
	explicit HDisplayType(int t, const char* name, const char* label): m_t(t), m_s(name), m_l(label) {};
	int m_t;
	const QString m_s;
	const QString m_l;
};

bool operator==(const HDisplayType& lhs, const HDisplayType& rhs);
const HDisplayType& getDisplayType(int number_value);


class HStimulusLayoutType
{
public:
	static const HStimulusLayoutType HStimulusLayoutUnknown;
	static const HStimulusLayoutType HStimulusLayoutSingle;
	static const HStimulusLayoutType HStimulusLayoutLeftRight;
	static const HStimulusLayoutType HStimulusLayoutTriple;

	// The contents of this array are used to populate the radio buttons.
	static const HStimulusLayoutType* A[3];

	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	const QString& label() const { return m_l; }

private:
	explicit HStimulusLayoutType(int t, const char* name, const char* label): m_t(t), m_s(name), m_l(label) {};
	int m_t;
	const QString m_s;
	const QString m_l;
};

bool operator==(const HStimulusLayoutType& lhs, const HStimulusLayoutType& rhs);
const HStimulusLayoutType& getStimulusLayoutType(int number_value);




class HResultsType
{
public:
	static const HResultsType HResultsTypeOriginalRun;
	static const HResultsType HResultsTypeTestRun;
	static const HResultsType HResultsTypeReliabilityRun;
	static const HResultsType HResultsTypeUnknown;

	static const HResultsType* A[3];

	int number() const { return m_t; };
	const QString& name() const { return m_s; };

private:
	explicit HResultsType(int t, const char* name): m_t(t), m_s(name) {};
	int m_t;
	const QString m_s;
};

bool operator==(const HResultsType& lhs, const HResultsType& rhs);
const HResultsType& getResultsType(int number_value);


class HLookDetectorType
{
public:
	static const HLookDetectorType UnknownLookDetector;
	static const HLookDetectorType Keypad;
	static const HLookDetectorType TestFile;
	static const HLookDetectorType EyeTracker;

	static const HLookDetectorType* A[2];

	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	const QString& label() const { return m_l; }
private:
	explicit HLookDetectorType(int t, const char* name, const char* label): m_t(t), m_s(name), m_l(label) {};
	int m_t;
	const QString m_s;
	const QString m_l;
};

bool operator==(const HLookDetectorType& lhs, const HLookDetectorType& rhs);
const HLookDetectorType& getLookDetectorType(int number_value);


class HRandomizationType
{
public:
	static const HRandomizationType UnknownRandomizationType;
	static const HRandomizationType Simple;
	static const HRandomizationType Block;

	static const HRandomizationType* A[2];

	int number() const { return m_t; };
	const QString& name() const { return m_s; };
	const QString& label() const { return m_l; };
private:
	explicit HRandomizationType(int t, const char *name, const char* label): m_t(t), m_s(name), m_l(label) {};
	int m_t;
	const QString m_s;
	const QString m_l;
};

bool operator==(const HRandomizationType& lhs, const HRandomizationType& rhs);
bool operator!=(const HRandomizationType& lhs, const HRandomizationType& rhs);
const HRandomizationType& getRandomizationType(int number_value);

class HPlayerPositionType
{
public:
	static const HPlayerPositionType UnknownPlayerPositionType;
	static const HPlayerPositionType Control;
	static const HPlayerPositionType Left;
	static const HPlayerPositionType Center;
	static const HPlayerPositionType Right;
	static const HPlayerPositionType Sound;

	static const HPlayerPositionType* A[5];

	HPlayerPositionType() : m_t(-1), m_s("") {};
	HPlayerPositionType(const HPlayerPositionType& ppt): m_t(ppt.number()), m_s(ppt.name()) {};
	HPlayerPositionType& operator=(const HPlayerPositionType& rhs);

	int number() const { return m_t; };
	QString name() const { return m_s; };
private:
	explicit HPlayerPositionType(int t, const char *name): m_t(t), m_s(name) {};
	int m_t;
	QString m_s;
};

bool operator<(const HPlayerPositionType& lhs, const HPlayerPositionType& rhs);
bool operator==(const HPlayerPositionType& lhs, const HPlayerPositionType& rhs);
bool operator!=(const HPlayerPositionType& lhs, const HPlayerPositionType& rhs);
const HPlayerPositionType& getPlayerPositionType(int number_value);





class HMediaContainerType
{
public:
	static const HMediaContainerType UnknownContainerType;
	static const HMediaContainerType Audio;
	static const HMediaContainerType Video;
	static const HMediaContainerType AudioVideo;
	static const HMediaContainerType Image;

	static const HMediaContainerType* A[4];

	HMediaContainerType() : m_t(-1), m_s("") {};
	HMediaContainerType(const HMediaContainerType& mct): m_t(mct.number()), m_s(mct.name()) {};
	HMediaContainerType& operator=(const HMediaContainerType& rhs);

	int number() const { return m_t; };
	QString name() const { return m_s; };
private:
	explicit HMediaContainerType(int t, const char *name): m_t(t), m_s(name) {};
	int m_t;
	QString m_s;
};

bool operator==(const HMediaContainerType& lhs, const HMediaContainerType& rhs);
bool operator!=(const HMediaContainerType& lhs, const HMediaContainerType& rhs);
const HMediaContainerType& getMediaContainerType(int number_value);



#endif



