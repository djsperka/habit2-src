/*
 *  HTypes.h
 *  habutil
 *
 *  Created by Oakes Lab on 2/26/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include <QString>

class HPhaseType
{
public:
	static const HPhaseType UnknownPhase;
	static const HPhaseType PreTest;
	static const HPhaseType Habituation;
	static const HPhaseType Test;
	static const HPhaseType AttentionGetter;	// Not really a phase, but makes this class useful for stim types as well as phase. 
	int number() const { return m_t; }
	const QString& name() const { return m_s; }
	
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



