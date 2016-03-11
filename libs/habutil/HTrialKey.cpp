/*
 * HTrialKey.cpp
 *
 *  Created on: Mar 10, 2016
 *      Author: Oakeslab
 */

#include <HTrialKey.h>

HTrialKey::HTrialKey()
: m_phase("")
, m_trial(0)
, m_repeat(0)
{}

HTrialKey::HTrialKey(const HTrialKey& k)
: m_phase(k.phase())
, m_trial(k.trial())
, m_repeat(k.repeat())
{}

HTrialKey::~HTrialKey()
{}

HTrialKey::HTrialKey(const QString& phase, int trial, int repeat)
: m_phase(phase)
, m_trial(trial)
, m_repeat(repeat)
{}

void HTrialKey::setPhase(const QString& phase) { m_phase = phase; };

void HTrialKey::setTrial(int trial) { m_trial = trial; };

void HTrialKey::setRepeat(int repeat) { m_repeat = repeat; };

const QString& HTrialKey::phase() const { return m_phase; };
int HTrialKey::trial() const { return m_trial; };
int HTrialKey::repeat() const { return m_repeat; };

HTrialKey& HTrialKey::operator=(const HTrialKey& rhs)
{
	if (this == &rhs) return *this;
	setPhase(rhs.phase());
	setTrial(rhs.trial());
	setRepeat(rhs.repeat());
	return *this;
}

bool HTrialKey::operator<(const HTrialKey& rhs)
{
	bool b;

	if (m_phase < rhs.phase()) return true;
	if (m_phase > rhs.phase()) return false;

	// if we make it here then the phases are equal - check trial number
	if (m_trial < rhs.trial()) return true;
	if (m_trial > rhs.trial()) return false;

	// if we make it here, then phase and trial are equal - check repeat number
	if (m_repeat < rhs.repeat()) return true;

	// if we make it here, then has to be false
	return false;
}

