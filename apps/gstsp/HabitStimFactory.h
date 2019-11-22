/*
 * HabitStimFactory.h
 *
 *  Created on: Nov 21, 2019
 *      Author: dan
 */

#ifndef HABITSTIMFACTORY_H_
#define HABITSTIMFACTORY_H_

#include "StimFactory.h"
#include "stimulussettings.h"
#include "stimulusdisplayinfo.h"
#include "Source.h"
#include "Stim.h"

class HabitStimFactory: public StimFactory
{
	Habit::StimulusDisplayInfo m_sdi;
	std::map<hmm::HMMStimID, Habit::StimulusSettings> m_ssMap;
	hmm::Source *makeSource(const Habit::StimulusInfo& info, hmm::HMM& mm, void *userdata, hmm::HMMSourceType stype);
	hmm::Stim *makeHabitStim(const Habit::StimulusSettings& ss, const Habit::StimulusDisplayInfo& sdi, hmm::HMM& mm);
public:
	HabitStimFactory(const Habit::StimulusDisplayInfo& sdi);
	virtual ~HabitStimFactory();
	hmm::HMMStimID addStimulusSettings(const Habit::StimulusSettings& ss);
	hmm::Stim* operator()(hmm::HMMStimID id, hmm::HMM& mm);
};

#endif /* HABITSTIMFACTORY_H_ */
