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
static const QString f_sVersion21("2000021");


HPhaseSettings::HPhaseSettings(int phase_id)
: m_id(phase_id)
, m_bEnabled(false)
, m_name()
, m_seqno(0)
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
, m_habituationSettings()
, m_stimuli()
{};

HPhaseSettings::HPhaseSettings(const HPhaseSettings& ts)
: m_id(ts.getId())
, m_bEnabled(ts.getIsEnabled())
, m_name(ts.getName())
, m_seqno(ts.getSeqno())
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
, m_habituationSettings(ts.habituationSettings())
, m_stimuli(ts.stimuli())
{};

HPhaseSettings& HPhaseSettings::operator=(const HPhaseSettings& rhs)
{
	if (this != &rhs)
	{
		m_id = rhs.getId();
		m_bEnabled = rhs.getIsEnabled();
		m_name = rhs.getName();
		m_seqno = rhs.getSeqno();
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
		m_habituationSettings = rhs.habituationSettings();
		m_stimuli = rhs.stimuli();
	}
	return *this;
}

HPhaseSettings HPhaseSettings::clone() const
{
	HPhaseSettings settings(*this);
	settings.setId(-1);
	return settings;
}

QDataStream & Habit::operator<< (QDataStream& stream, const HPhaseSettings& settings)
{
	stream	<< f_sVersion21
			<< settings.getId() << settings.getIsEnabled()
			<< settings.getName() << settings.getSeqno()
			<< settings.getUseLookingCriteria() << settings.getIsSingleLook() << settings.getIsMaxAccumulatedLookTime()
			<< settings.getMaxAccumulatedLookTime() << settings.getIsMaxLookAwayTime()
			<< settings.getMaxLookAwayTime() << settings.getRepeatTrialOnMaxLookAwayTime()
			<< settings.getIsMaxStimulusTime()
			<< settings.getMaxStimulusTime() << settings.getMeasureStimulusTimeFromOnset()
			<< settings.getMeasureStimulusTimeFromLooking()
			<< settings.getIsMaxNoLookTime() << settings.getMaxNoLookTime()
			<< settings.habituationSettings() << settings.stimuli();
//	stream	<< f_sVersion18
//			<< settings.getId() << settings.getIsEnabled() << settings.getPhaseType().number() << settings.getNTrials()
//			<< settings.getUseLookingCriteria() << settings.getIsSingleLook() << settings.getIsMaxAccumulatedLookTime()
//			<< settings.getMaxAccumulatedLookTime() << settings.getIsMaxLookAwayTime()
//			<< settings.getMaxLookAwayTime() << settings.getRepeatTrialOnMaxLookAwayTime()
//			<< settings.getIsMaxStimulusTime()
//			<< settings.getMaxStimulusTime() << settings.getMeasureStimulusTimeFromOnset()
//			<< settings.getMeasureStimulusTimeFromLooking()
//			<< settings.getIsMaxNoLookTime() << settings.getMaxNoLookTime();
	return stream;
}

QDebug Habit::operator<<(QDebug dbg, const HPhaseSettings& settings)
{
	dbg.nospace() << "Id " << settings.getId() << "Enabled " << settings.getIsEnabled() << " Name " << settings.getName() << " Seqno " << settings.getSeqno()
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
	int id, nphase, seqno=0;
	unsigned int n, maxAccum, maxLA, maxStim, maxNoLook;
	bool bEnabled, bUseLooking, bSingle, bMaxAccum, bMaxLA, bMaxStim, bMeasOnset, bMeasLook, bMaxNoLook, bRepeat;
	QString sVersion, sName;
	Habit::HabituationSettings habituationSettings;
	Habit::StimuliSettings stimuliSettings;

	// Save position in stream in case this is an old version
	qint64 pos = stream.device()->pos();
	stream >> sVersion;
	if (sVersion == f_sVersion21)
	{
		stream >> id >> bEnabled >> sName >> seqno >> bUseLooking >> bSingle >> bMaxAccum >> maxAccum >> bMaxLA >> maxLA >> bRepeat >> bMaxStim >> maxStim >> bMeasOnset >> bMeasLook >> bMaxNoLook >> maxNoLook >> habituationSettings >> stimuliSettings;
		settings.setHabituationSettings(habituationSettings);
		settings.setStimuli(stimuliSettings);
	}
	else if (sVersion == f_sVersion18)
	{
		stream >> id >> bEnabled >> nphase >> n >> bUseLooking >> bSingle >> bMaxAccum >> maxAccum >> bMaxLA >> maxLA >> bRepeat >> bMaxStim >> maxStim >> bMeasOnset >> bMeasLook >> bMaxNoLook >> maxNoLook;

		// dummy values of name, seqno for older versions
		sName = getPhaseType(nphase).name();
		seqno = nphase;
	}
	else
	{
		// reset stream to position before trying to read version
		stream.device()->seek(pos);

		// and now read
		stream >> id >> bEnabled >> nphase >> n >> bUseLooking >> bSingle >> bMaxAccum >> maxAccum >> bMaxLA >> maxLA >> bMaxStim >> maxStim >> bMeasOnset >> bMeasLook >> bMaxNoLook >> maxNoLook;

		// default was to NOT repeat
		bRepeat = false;

		// dummy values of name, seqno for older versions
		sName = getPhaseType(nphase).name();
		seqno = nphase;

	}

	settings.setId(id);
	settings.setIsEnabled(bEnabled);
	settings.setName(sName);
	settings.setSeqno(seqno);
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
			//lhs.getPhaseType() == rhs.getPhaseType() &&
			//lhs.getNTrials() == rhs.getNTrials() &&
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
			(!lhs.getIsMaxNoLookTime() || (lhs.getIsMaxNoLookTime() && lhs.getMaxNoLookTime() == rhs.getMaxNoLookTime())) &&
			lhs.habituationSettings() == rhs.habituationSettings() &&
			lhs.stimuli() == rhs.stimuli();
}


void HPhaseSettings::loadFromDB(int phaseId)
{
	Habit::MainDao maindao;
	maindao.getHPhaseSettings(phaseId, *this);
	maindao.getStimuliSettings(phaseId, this->stimuli());
	maindao.getHabituationSettingsForPhase(phaseId, this->habituationSettings());
	return;
}


void HPhaseSettings::saveToDB(int experimentID)
{
	Habit::MainDao dao;
	dao.addOrUpdateHPhaseSettings(experimentID, *this);
	m_habituationSettings.saveToDB(this->getId());
	m_stimuli.saveToDB(this->getId());
	return;
}
