/*
 * HMMStim.h
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#ifndef HMMSTIM_H_
#define HMMSTIM_H_

#include "HMMSource.h"
#include <map>
#include <memory>

// HMMStim would correspond to StimulusSettings - the collection of individual sources (files)
// that comprise the "stimulus" to be presented. The MMStimPosition corresponds to the
// PlayerPositionType, L/R/C/ISS, and more generally provides a video stream on the src pad if
// ppt is a video type, and audio on the src pad is an audio type. Its up the the application
// to decide what to do with all of them.

class HMMStim
{
public:
	typedef std::unique_ptr<HMMSource> source_ptr;
	typedef std::map<HMMStimPosition, source_ptr> HMMStimPosSourceMap;
private:
	HMMStimState m_stimState;
	HMMStimPosSourceMap m_sourceMap;
public:
	HMMStim(): m_stimState(HMMStimState::NONE) {};
	virtual ~HMMStim() {};
	void addSource(HMMStimPosition, HMMSource* psrc);
	HMMSource *getSource(HMMStimPosition pos);
	void setStimState(HMMStimState state) { m_stimState = state; }
	HMMStimState getStimState() const { return m_stimState; }
	std::map<HMMStimPosition, source_ptr>& sourceMap() { return m_sourceMap; }
};



#endif /* HMMSTIM_H_ */
