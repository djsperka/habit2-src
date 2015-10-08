/*
 * HTrialSettings.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: Oakeslab
 */

#include "HPhaseSettings.h"
#include "maindao.h"

using namespace Habit;

// version string for input/output. See operator<<, operator>>
static const QString f_sVersion18("2000018");


HPhaseSettings::HPhaseSettings(const HPhaseType& type)
: m_id(-1)
, m_bEnabled(false)
, m_pPhaseType(&type)
, m_ntrials(0)
, m_bUseLookingCriteria(true)
, m_bIsSingleLook(true)
, m_bIsMaxAccumulatedLookTime(false)
, m_uiMaxAccumulatedLookTime(0)
, m_bIsMaxLookAwayTime(false)
, m_uiMaxLookAwayTime(0)
, m_bRepeatTrialOnMaxLookAwayTime(false)
, m_bIsMaxStimulusTime(false)
, m_uiMaxStimulusTime(0)
, m_bMeasureStimulusTimeFromOnset(true)
, m_bMeasureStimulusTimeFromLooking(false)
, m_bIsMaxNoLookTime(false)
, m_uiMaxNoLookTime(0)
{};

HPhaseSettings::HPhaseSettings(const HPhaseSettings& ts)
: m_id(ts.getId())
, m_bEnabled(ts.getIsEnabled())
, m_pPhaseType(&ts.getPhaseType())
, m_ntrials(ts.getNTrials())
, m_bUseLookingCriteria(ts.getUseLookingCriteria())
, m_bIsSingleLook(ts.getIsSingleLook())
, m_bIsMaxAccumulatedLookTime(ts.getIsMaxAccumulatedLookTime())
, m_uiMaxAccumulatedLookTime(ts.getMaxAccumulatedLookTime())
, m_bIsMaxLookAwayTime(ts.getIsMaxLookAwayTime())
, m_uiMaxLookAwayTime(ts.getMaxLookAwayTime())
, m_bRepeatTrialOnMaxLookAwayTime(ts.getRepeatTrialOnMaxLookAwayTime())
, m_bIsMaxStimulusTime(ts.getIsMaxStimulusTime())
, m_uiMaxStimulusTime(ts.getMaxStimulusTime())
, m_bMeasureStimulusTimeFromOnset(ts.getMeasureStimulusTimeFromOnset())
, m_bMeasureStimulusTimeFromLooking(ts.getMeasureStimulusTimeFromLooking())
, m_bIsMaxNoLookTime(ts.getIsMaxNoLookTime())
, m_uiMaxNoLookTime(ts.getMaxNoLookTime())
{};

HPhaseSettings& HPhaseSettings::operator=(const HPhaseSettings& rhs)
{
	if (this != &rhs)
	{
		m_id = rhs.getId();
		m_bEnabled = rhs.getIsEnabled();
		this->setPhaseType(rhs.getPhaseType());
		m_ntrials = rhs.getNTrials();
		m_bUseLookingCriteria = rhs.getUseLookingCriteria();
		m_bIsSingleLook = rhs.getIsSingleLook();
		m_bIsMaxAccumulatedLookTime = rhs.getIsMaxAccumulatedLookTime();
		m_uiMaxAccumulatedLookTime = rhs.getMaxAccumulatedLookTime();
		m_bIsMaxLookAwayTime = rhs.getIsMaxLookAwayTime();
		m_uiMaxLookAwayTime = rhs.getMaxLookAwayTime();
		m_bRepeatTrialOnMaxLookAwayTime = rhs.getRepeatTrialOnMaxLookAwayTime();
		m_bIsMaxStimulusTime = rhs.getIsMaxStimulusTime();
		m_uiMaxStimulusTime = rhs.getMaxStimulusTime();
		m_bMeasureStimulusTimeFromOnset = rhs.getMeasureStimulusTimeFromOnset();
		m_bMeasureStimulusTimeFromLooking = rhs.getMeasureStimulusTimeFromLooking();
		m_bIsMaxNoLookTime = rhs.getIsMaxNoLookTime();
		m_uiMaxNoLookTime = rhs.getMaxNoLookTime();
	}
	return *this;
}

HPhaseSettings HPhaseSettings::clone()
{
	HPhaseSettings settings(*this);
	settings.setId(-1);
	return settings;
}

QDataStream & Habit::operator<< (QDataStream& stream, HPhaseSettings settings)
{
	stream	<< f_sVersion18
			<< settings.getId() << settings.getIsEnabled() << settings.getPhaseType().number() << settings.getNTrials()
			<< settings.getUseLookingCriteria() << settings.getIsSingleLook() << settings.getIsMaxAccumulatedLookTime()
			<< settings.getMaxAccumulatedLookTime() << settings.getIsMaxLookAwayTime()
			<< settings.getMaxLookAwayTime() << settings.getRepeatTrialOnMaxLookAwayTime()
			<< settings.getIsMaxStimulusTime()
			<< settings.getMaxStimulusTime() << settings.getMeasureStimulusTimeFromOnset()
			<< settings.getMeasureStimulusTimeFromLooking()
			<< settings.getIsMaxNoLookTime() << settings.getMaxNoLookTime();
	return stream;
}

QDebug Habit::operator<<(QDebug dbg, const HPhaseSettings& settings)
{
	dbg.nospace() << "Id " << settings.getId() << "Enabled " << settings.getIsEnabled() << " ptype " << settings.getPhaseType().number() << " N " << settings.getNTrials()
			<< " use look " << settings.getUseLookingCriteria() << " single " << settings.getIsSingleLook() << " maxaccum " << settings.getIsMaxAccumulatedLookTime()
			<< settings.getMaxAccumulatedLookTime() << " max lookaway " << settings.getIsMaxLookAwayTime()
			<< settings.getMaxLookAwayTime() << " repeat? " << settings.getRepeatTrialOnMaxLookAwayTime() << " max stim " << settings.getIsMaxStimulusTime()
			<< settings.getMaxStimulusTime() << " onset " << settings.getMeasureStimulusTimeFromOnset()
			<< " looking " << settings.getMeasureStimulusTimeFromLooking()
			<< " max nolook " << settings.getIsMaxNoLookTime() << settings.getMaxNoLookTime();
	return dbg.nospace();
}


QDataStream & Habit::operator>> (QDataStream& stream, HPhaseSettings& settings)
{
	int id, nphase;
	unsigned int n, maxAccum, maxLA, maxStim, maxNoLook;
	bool bEnabled, bUseLooking, bSingle, bMaxAccum, bMaxLA, bMaxStim, bMeasOnset, bMeasLook, bMaxNoLook, bRepeat;
	QString sVersion;

	// Save position in stream in case this is an old version
	qint64 pos = stream.device()->pos();
	stream >> sVersion;
	if (sVersion == f_sVersion18)
	{
		stream >> id >> bEnabled >> nphase >> n >> bUseLooking >> bSingle >> bMaxAccum >> maxAccum >> bMaxLA >> maxLA >> bRepeat >> bMaxStim >> maxStim >> bMeasOnset >> bMeasLook >> bMaxNoLook >> maxNoLook;
	}
	else
	{
		// reset stream to position before trying to read version
		stream.device()->seek(pos);

		// and now read
		stream >> id >> bEnabled >> nphase >> n >> bUseLooking >> bSingle >> bMaxAccum >> maxAccum >> bMaxLA >> maxLA >> bMaxStim >> maxStim >> bMeasOnset >> bMeasLook >> bMaxNoLook >> maxNoLook;

		// default was to NOT repeat
		bRepeat = false;
	}

	settings.setId(id);
	settings.setIsEnabled(bEnabled);
	settings.setPhaseType(getPhaseType(nphase));
	settings.setNTrials(n);
	settings.setUseLookingCriteria(bUseLooking);
	settings.setIsSingleLook(bSingle);
	settings.setIsMaxAccumulatedLookTime(bMaxAccum);
	settings.setMaxAccumulatedLookTime(maxAccum);
	settings.setIsMaxLookAwayTime(bMaxLA);
	settings.setMaxLookAwayTime(maxLA);
	settings.setRepeatTrialOnMaxLookAwayTime(bRepeat);
	settings.setIsMaxStimulusTime(bMaxStim);
	settings.setMaxStimulusTime(maxStim);
	settings.setMeasureStimulusTimeFromOnset(bMeasOnset);
	settings.setMeasureStimulusTimeFromLooking(bMeasLook);
	settings.setIsMaxNoLookTime(bMaxNoLook);
	settings.setMaxNoLookTime(maxNoLook);


	return stream;
}

bool Habit::operator==(const HPhaseSettings& lhs, const HPhaseSettings& rhs)
{
	return	lhs.getId() == rhs.getId() &&
			lhs.getIsEnabled() == rhs.getIsEnabled() &&
			lhs.getPhaseType() == rhs.getPhaseType() &&
			lhs.getNTrials() == rhs.getNTrials() &&
			lhs.getUseLookingCriteria() == rhs.getUseLookingCriteria() &&
			(!lhs.getUseLookingCriteria() || (lhs.getUseLookingCriteria() &&
					lhs.getIsSingleLook() == rhs.getIsSingleLook() &&
					lhs.getIsMaxAccumulatedLookTime() == rhs.getIsMaxAccumulatedLookTime() &&
					(!lhs.getIsMaxAccumulatedLookTime() || (lhs.getIsMaxAccumulatedLookTime() &&
							lhs.getMaxAccumulatedLookTime() == rhs.getMaxAccumulatedLookTime())))) &&
			lhs.getIsMaxLookAwayTime() == rhs.getIsMaxLookAwayTime() &&
			(!lhs.getIsMaxLookAwayTime() ||
					(lhs.getMaxLookAwayTime() == rhs.getMaxLookAwayTime() && lhs.getRepeatTrialOnMaxLookAwayTime() == rhs.getRepeatTrialOnMaxLookAwayTime())) &&
			lhs.getIsMaxStimulusTime() == rhs.getIsMaxStimulusTime() &&
			(!lhs.getIsMaxStimulusTime() ||
					(lhs.getIsMaxStimulusTime() && lhs.getMaxStimulusTime() == rhs.getMaxStimulusTime() &&
					lhs.getMeasureStimulusTimeFromOnset() == rhs.getMeasureStimulusTimeFromOnset() &&
					lhs.getMeasureStimulusTimeFromLooking() == rhs.getMeasureStimulusTimeFromLooking())) &&
			lhs.getIsMaxNoLookTime() == rhs.getIsMaxNoLookTime() &&
			(!lhs.getIsMaxNoLookTime() || (lhs.getIsMaxNoLookTime() && lhs.getMaxNoLookTime() == rhs.getMaxNoLookTime()));
}

void HPhaseSettings::loadFromDB(int id, int type)
{
	Habit::MainDao maindao;
	maindao.getHPhaseSettingsForExperiment(id, type, this);
}


bool HPhaseSettings::saveToDB(int id)
{
	Habit::MainDao dao;
	return dao.addOrUpdateHPhaseSettings(id, this);
}
