/*
 * HabitStimFactory.cpp
 *
 *  Created on: Nov 21, 2019
 *      Author: dan
 */

#include "HabitStimFactory.h"
#include "HMM.h"
#include "stimulussettings.h"

using namespace hmm;

HabitStimFactory::HabitStimFactory(const Habit::StimulusDisplayInfo& sdi)
: m_sdi(sdi)
{
	// create stimulus settings for the background.
	Habit::StimulusSettings background("background", sdi.getBackGroundColor());
	addStimulusSettings(background);
}

HabitStimFactory::~HabitStimFactory()
{
}

hmm::HMMStimID HabitStimFactory::addStimulusSettings(const Habit::StimulusSettings& settings)
{
	hmm::HMMStimID id = (hmm::HMMStimID)((unsigned long)m_ssMap.size());
	m_ssMap.insert(std::make_pair(id, settings));
	return id;
}

hmm::Stim* HabitStimFactory::operator()(hmm::HMMStimID id, hmm::HMM& mm)
{
	hmm::Stim *pstim = NULL;
	if (m_ssMap.count(id) == 0)
		throw std::runtime_error("StimFactory(id) not found");
	else
		pstim = makeHabitStim(m_ssMap[id], m_sdi, mm);

	return pstim;
}

hmm::Source *HabitStimFactory::makeSource(const Habit::StimulusInfo& info, hmm::HMM& mm, void *userdata, hmm::HMMSourceType stype)
{
	hmm::Source *psrc=NULL;
	if (info.isColor() || info.isBackground())
	{
		psrc = mm.makeSourceFromColor(info.getColor().rgba());
	}
	else
	{
		psrc = mm.makeSourceFromFile(info.getAbsoluteFileName().toStdString(), stype, userdata, info.isLoopPlayBack(), info.getVolume());
	}
	return psrc;
}


hmm::Stim *HabitStimFactory::makeHabitStim(const Habit::StimulusSettings& ss, const Habit::StimulusDisplayInfo& sdi, hmm::HMM& mm)
{
	hmm::Source *psrc;
	Habit::StimulusInfo info;
	hmm::Stim *pstim=new hmm::Stim();
	pstim->setStimState(hmm::HMMStimState::INITIALIZING);

	// This counter is triggered once all the sources have been prerolled.
	// It needs to know how many sources there are (which are being prerolled), and it is passed to the counter
	// used for each source - when they are triggered (i.e. each stream in the source is prerolled),
	// they decrement this counter.
	// Note that below we increment this counter for each file-based source (not the
	// bin-based videotestsrc, as it doesn't need prerolling process as the files do)
	hmm::StimPrerollCounter *pstimCounter = new hmm::StimPrerollCounter(pstim, 0);

	// Now a bit of Habit messiness. The StimulusSettings object as originally written had
	// slots for 4 stimuli, and which ones were used depended on the StimulusDisplay settings for the experiment -
	// single screen, dual screen, etc.
	// In the HMM, the Port configuration is equivalent to the StimulusDisplay settings, but with
	// one complication: The "StimPosition" used here is similar to the "HPlayerPositionType" (left, center, right, iss)
	// used in habit. The complication is that the "StimPosition" is arbitrary here, whereas it had definite values
	// assigned in Habit. I've accounted for this by making the integer values for constants HMMSTimPosition::LEFT etc
	// equivalent to their HPlayerPositionType counterparts "number()" value.

	if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutSingle)
	{
		// center
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_CENTER, makeSource(ss.getCenterStimulusInfo(), mm,
				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// iss?
		if (sdi.getUseISS())
		{
			pstimCounter->increment();
			SourcePrerollCounter* psourceCounter = new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter);
			pstim->addSource(HMM::STIMPOS_AUDIO, makeSource(ss.getIndependentSoundInfo(), mm, psourceCounter, HMMSourceType::AUDIO_ONLY));
		}
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		// left
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_LEFT, makeSource(ss.getLeftStimulusInfo(), mm,
				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// right
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_RIGHT, makeSource(ss.getRightStimulusInfo(), mm,
				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// iss?
		if (sdi.getUseISS())
		{
			pstimCounter->increment();
			pstim->addSource(HMM::STIMPOS_AUDIO, makeSource(ss.getIndependentSoundInfo(), mm,
					new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
					HMMSourceType::AUDIO_ONLY));
		}
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		// left
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_LEFT, makeSource(ss.getLeftStimulusInfo(), mm,
				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// right
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_RIGHT, makeSource(ss.getRightStimulusInfo(), mm,
				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// center
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_CENTER, makeSource(ss.getCenterStimulusInfo(), mm,
				new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// iss?
		if (sdi.getUseISS())
		{
			pstimCounter->increment();
			pstim->addSource(HMM::STIMPOS_AUDIO, makeSource(ss.getIndependentSoundInfo(), mm,
					new SourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
					HMMSourceType::AUDIO_ONLY));
		}
	}

	return pstim;
}

