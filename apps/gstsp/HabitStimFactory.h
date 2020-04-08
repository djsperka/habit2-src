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
	QDir m_dir;
	hmm::HMMStimID m_bkgdID;
	std::map<hmm::HMMStimID, Habit::StimulusSettings> m_ssMap;

	hmm::Stim *makeHabitStim(const Habit::StimulusSettings& ss, GstElement *pipeline, const std::string& prefix);
	void addSourceToStim(hmm::Stim *pstim, const std::string& prefix, hmm::HMMStimPosition pos, hmm::HMMSourceType stype, GstElement *pipeline, const Habit::StimulusInfo& info);

public:
	HabitStimFactory(const Habit::StimulusDisplayInfo& sdi, const QDir& rootDir);
	virtual ~HabitStimFactory();
	hmm::HMMStimID addStimulusSettings(const Habit::StimulusSettings& ss);
	hmm::Stim* operator()(hmm::HMMStimID id, hmm::HMM& mm, const std::string& prefix);
	hmm::Stim* background(hmm::HMM& mm);
};

#endif /* HABITSTIMFACTORY_H_ */
