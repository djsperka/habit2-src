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

class Port
{
	typedef std::map<HMMStimPosition, GstElement *> HMMPortPosEleMap;
	HMMPortPosEleMap m_mapPosVideo;				// video only; StimPosition <--> GstElement (downstream)
	HMMPortPosEleMap m_mapPosAudio;				// audio only; StimPosition <--> GstElement (mixer downstream)

public:
	Port();
	virtual ~Port();

	void addVideoEle(HMMStimPosition pos, GstElement *ele);
	void addAudioEle(HMMStimPosition pos, GstElement *mixer);

	std::vector<HMMStimPosition> getVideoPositions() const;
	std::vector<HMMStimPosition> getAudioPositions() const;

	void connect(Stim& stim);	// assumes not connected, will throw otherwise
	void disconnect();				// might not be connected, will be OK if not
};

}
#endif /* HMMPORT_H_ */
