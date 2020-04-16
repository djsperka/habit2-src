/*
 * HMMStim.h
 *
 *  Created on: Oct 17, 2019
 *      Author: dan
 */

#ifndef HMMSTIM_H_
#define HMMSTIM_H_

#include "Source.h"
#include <map>
#include <memory>

// HMMStim would correspond to StimulusSettings - the collection of individual sources (files)
// that comprise the "stimulus" to be presented. The MMStimPosition corresponds to the
// PlayerPositionType, L/R/C/ISS, and more generally provides a video stream on the src pad if
// ppt is a video type, and audio on the src pad is an audio type. Its up the the application
// to decide what to do with all of them.
namespace hmm {

class Stim
{
public:
	//typedef std::unique_ptr<Source> source_ptr;
	typedef Source* SourceP;
	typedef std::map<HMMStimPosition, SourceP> HMMStimPosSourceMap;
private:
	GstElement *m_pipeline;
	std::string m_prefix;
	HMMStimState m_stimState;
	HMMStimPosSourceMap m_sourceMap;
public:
	Stim(GstElement *pipeline, const std::string& prefix): m_pipeline(pipeline), m_prefix(prefix), m_stimState(HMMStimState::NONE) {};
	virtual ~Stim();
	GstElement *pipeline() const { return m_pipeline; }
	void setStimState(HMMStimState state) { m_stimState = state; }
	HMMStimState getStimState() const { return m_stimState; }
	std::map<HMMStimPosition, SourceP>& sourceMap() { return m_sourceMap; }
	const std::string& prefix() { return m_prefix; }

	// set parent on source, add to this.
	void addSource(HMMStimPosition pos, SourceP s);
	Source *getSource(HMMStimPosition pos);

	// set up blocking probes and sync
	void preroll(GstElement *pipeline);

	// block playing sources with idle probe
	void stop();

	// flushing seek on (already blocked) sources
	void rewind();
};
} // end namespace

#endif /* HMMSTIM_H_ */
