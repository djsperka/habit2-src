/*
 * HMMPort.h
 *
 *  Created on: Oct 16, 2019
 *      Author: dan
 */

#ifndef HMMPORT_H_
#define HMMPORT_H_

#include "HMMTypes.h"
#include <gst/gst.h>
#include <map>
#include <vector>

namespace hmm {

class Stim;

// The Port class represents the tail end of the gstreamer pipeline -- the downstream end where the actual business of
// presenting the stimuli is done (where the sinks are found).
// The Port only knows that a certain "position" will accept input from an upstream portion of a pipeline.
// The port keeps separate listings of video and audio sinks. The listings are maps, where the HMMStimPosition
// is the key, and a GstElement is the value. A src pad will be connected to the element at each position for
// each stimulus. It is required that each position here have a corresponding Source object.
// For audio, there is a wrinkle, as the stim position of the audio source doesn't matter in all cases.
// When a dedicated audio source is provided, then the audio stream from that source is mapped to the audio
// ele found with the STIMPOS_AUDIO position. If no such source is provided (as when ISS is not used in a
// habit expt, but sound comes instead from the audio track(s) of the video sources, each such audio stream is
// mapped to the audio element (which is generally an audio mixer element).
// The assumption, at this point at least, is that there is just a single audio output position. The structure here
// allows for more, but no such behavior is implemented.
//
// The map(s) here only tell us what type of output will be used. It tells us how to map the video sources
// to sinks, as a 1-1 correspondence is required (i.e. if STIMPOS_LEFT is one of the positions in the video map,
// then the incoming stimulus must have a source with a video stream at STIMPOS_LEFT as well.
//
// If, however, STIMPOS_AUDIO is in the audio map, then we will take whatever audio streams the Stim generates.
// When the Stim/Source objects are created, the destination of the audio streams is indicated by whether the source
// type is AUDIO_ONLY, AUDIO_VIDEO, or something else (VIDEO_ONLY).

class Port
{
	typedef std::map<HMMStimPosition, GstElement *> HMMPortPosEleMap;
	HMMPortPosEleMap m_mapPosVideo;				// video only; StimPosition <--> GstElement (downstream)
	HMMPortPosEleMap m_mapPosAudio;				// audio only; StimPosition <--> GstElement (mixer downstream)
	HMMPortPosEleMap m_mapPosVideoSink;			// video only;
	HMMPortPosEleMap m_mapPosAudioSink;				// audio only;

public:
	Port();
	virtual ~Port();

	void addVideoEle(HMMStimPosition pos, GstElement *ele);
	void addAudioEle(HMMStimPosition pos, GstElement *mixer);
	void addVideoSink(HMMStimPosition pos, GstElement *sink);
	void addAudioSink(HMMStimPosition pos, GstElement *sink);

	std::vector<HMMStimPosition> getVideoPositions() const;
	std::vector<HMMStimPosition> getAudioPositions() const;

	void setWidget(HMMStimPosition pos, GValue& value);

	void connect(Stim& stim);	// Plugs all srcs from Stim into this port. Assumes not connected, will throw otherwise
	void disconnect();				// might not be connected, will be OK if not
};

}
#endif /* HMMPORT_H_ */
