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
#include <vector>

class HPhaseType
{
public:
	static const HPhaseType UnknownPhase;
	static const HPhaseType PreTest;
	static const HPhaseType Habituation;
	static const HPhaseType Test;
	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	friend bool operator==(const HPhaseType& lhs, const HPhaseType& rhs);
private:
	explicit HPhaseType(int t, const char *name): m_t(t), m_s(name) {}
	int m_t;
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
	
private:
	explicit HLookTrans(int t, const char *name): m_t(t), m_s(name) {}
	int m_t;
	QString m_s;
};



class HLookDirection
{
public:
	static const HLookDirection NoLook;
	static const HLookDirection LookLeft;
	static const HLookDirection LookRight;
	static const HLookDirection LookCenter;
	static const HLookDirection UnknownLookDirection;
	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	
private:
	explicit HLookDirection(int t, const char *name): m_t(t), m_s(name) {}
	int m_t;
	QString m_s;
};

	
const HPhaseType& getPhaseType(int i);
const HLookTrans& getLookTransType(int i);
const HLookDirection& getLookDirection(int i);


// Stim context refers to when stim is to be played. 

class HStimContext
{
public:
	static const HStimContext UnknownContext;
	static const HStimContext PreTestPhase;
	static const HStimContext HabituationPhase;
	static const HStimContext TestPhase;
	static const HStimContext AttentionGetter;	// Not really a phase, but makes this class useful for stim types as well as phase. 
	int number() const { return m_t; }
	const QString& name() const { return m_s; }

	friend bool operator==(const HStimContext& lhs, const HStimContext& rhs);
	
private:
	explicit HStimContext(int t, const char *name): m_t(t), m_s(name) {}
	int m_t;
	QString m_s;
};

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
	const QString& m_s;
	const QString& m_l;
};

const HCriterionBasisType& getCriterionBasisType(int number_value);


#endif


