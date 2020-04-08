/*
 * StimFactory.h
 *
 *  Created on: Nov 20, 2019
 *      Author: dan
 */

#ifndef STIMFACTORY_H_
#define STIMFACTORY_H_

#include <map>
#include <string>
#include "HMMTypes.h"


// This factory separates the HMM (which only operates on Stim, Source, Stream)
// from the details of the configuration. A subclass, HabitStimFactory, does this
// for Habit (StimulusSettings, StimulusDisplayInfo, ...)
// The operator(id) returns a Stim*, all ready to go (or throws an exception).
//
// Stim* stim = factory()(stimid, hmm, prefix);
//
// prefix is prepended to all element names - make each stim have a unique prefix
//
// This stim has initial elements created, and elements are in
//
// The Stim created are owned by the caller. The relevant GstElement objects are
// ref'd and should be unref'd by the destructor of the Stim, Source, and Stream (TODO - NOT IMPLEMENTED????)
//
// The GstElements created here should be left in the READY state - They will be sync'd with the parent (the
// pipeline) when its time to preroll them.
//
// ID 0 is reserved for the background/default stimulus, so the subclass should create the background Stim in
// its constructor.

namespace hmm
{
	class HMM;
	class Stim;
}

class StimFactory
{

public:
	StimFactory() {};
	virtual ~StimFactory() {};
	virtual hmm::Stim* operator()(hmm::HMMStimID id, hmm::HMM& mm, const std::string& prefix) = 0;
	virtual hmm::Stim* background(hmm::HMM& mm) = 0;
};




#endif /* STIMFACTORY_H_ */
