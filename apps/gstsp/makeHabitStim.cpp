/*
 * makeHabitStim.cpp
 *
 *  Created on: Nov 20, 2019
 *      Author: dan
 */

#include "makeHabitStim.h"
#include "stimulusinfo.h"
#include "Counter.h"

using namespace hmm;

Source *makeSource(const Habit::StimulusInfo& info, hmm::HMM& mm, void *userdata, HMMSourceType stype)
{
	Source *psrc=NULL;
	if (info.isColor() || info.isBackground())
	{
		throw std::runtime_error("HMM::makeStim - cannot do color.");
	}
	else
	{
		psrc = mm.makeSourceFromFile(info.getAbsoluteFileName().toStdString(), stype, userdata, info.isLoopPlayBack(), info.getVolume());
	}
	return psrc;
}


Stim *makeHabitStim(const Habit::StimulusSettings& ss, const Habit::StimulusDisplayInfo& sdi, hmm::HMM& mm)
{
	Source *psrc;
	Habit::StimulusInfo info;
	Stim *pstim=new Stim();
	pstim->setStimState(HMMStimState::INITIALIZING);

	// This counter is triggered once all the sources have been prerolled.
	// It needs to know how many sources there are (which are being prerolled), and it is passed to the counter
	// used for each source - when they are triggered (i.e. each stream in the source is prerolled),
	// they decrement this counter.
	// Note that below we increment this counter for each file-based source (not the
	// bin-based videotestsrc, as it doesn't need prerolling process as the files do)
	StimPrerollCounter *pstimCounter = new StimPrerollCounter(pstim, 0);

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
				new FileSourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// iss?
		if (sdi.getUseISS())
		{
			pstimCounter->increment();
			FileSourcePrerollCounter* psourceCounter = new FileSourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter);
			pstim->addSource(HMM::STIMPOS_AUDIO, makeSource(ss.getIndependentSoundInfo(), mm, psourceCounter, HMMSourceType::AUDIO_ONLY));
		}
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutLeftRight)
	{
		// left
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_LEFT, makeSource(ss.getLeftStimulusInfo(), mm,
				new FileSourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// right
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_RIGHT, makeSource(ss.getRightStimulusInfo(), mm,
				new FileSourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// iss?
		if (sdi.getUseISS())
		{
			pstimCounter->increment();
			pstim->addSource(HMM::STIMPOS_AUDIO, makeSource(ss.getIndependentSoundInfo(), mm,
					new FileSourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
					HMMSourceType::AUDIO_ONLY));
		}
	}
	else if (sdi.getStimulusLayoutType() == HStimulusLayoutType::HStimulusLayoutTriple)
	{
		// left
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_LEFT, makeSource(ss.getLeftStimulusInfo(), mm,
				new FileSourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// right
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_RIGHT, makeSource(ss.getRightStimulusInfo(), mm,
				new FileSourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// center
		pstimCounter->increment();
		pstim->addSource(HMM::STIMPOS_CENTER, makeSource(ss.getCenterStimulusInfo(), mm,
				new FileSourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
				(sdi.getUseISS() ? HMMSourceType::VIDEO_ONLY : HMMSourceType::AUDIO_VIDEO)));

		// iss?
		if (sdi.getUseISS())
		{
			pstimCounter->increment();
			pstim->addSource(HMM::STIMPOS_AUDIO, makeSource(ss.getIndependentSoundInfo(), mm,
					new FileSourcePrerollCounter(psrc, mm.pipeline(), 1, pstimCounter),
					HMMSourceType::AUDIO_ONLY));
		}
	}

	return pstim;
}

