/*
 * StimFactory.h
 *
 *  Created on: Nov 20, 2019
 *      Author: dan
 */

#ifndef STIMFACTORY_H_
#define STIMFACTORY_H_

#include <map>
#include "HMMTypes.h"


// This factory separates the HMM (which only operates on Stim, Source, Stream)
// from Habit (StimulusSettings, StimulusDisplayInfo, ...)
// The operator(id) returns a Stim*, all ready to go (or throws an exception).
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
	virtual hmm::Stim* operator()(hmm::HMMStimID id, hmm::HMM& mm) = 0;
};




#endif /* STIMFACTORY_H_ */
